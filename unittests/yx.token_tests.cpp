#include <yosemite/testing/yx_tester.hpp>

using namespace yosemite::testing;

class yx_token_tester : public yx_tester {
public:
   yx_token_tester() {
      init_yosemite_contracts();

      create_accounts({N(tkprovider), N(tkprovider2)});
      produce_blocks();
   }
};

BOOST_AUTO_TEST_SUITE(yx_token_tests)

   BOOST_FIXTURE_TEST_CASE(required_auth_test, yx_token_tester) try {

      yx_symbol _ysymbol = yx_symbol::from_string("4,ETH@tkprovider");
      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_USER_TOKEN_ACCOUNT, N(create), N(user1), mvo()
                          ("ysymbol", _ysymbol)
                          ("can_set_options", 0)),
                          missing_auth_exception);

      auto result = token_create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      auto _token = yx_asset::from_string("1.0000 ETH@tkprovider");
      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_USER_TOKEN_ACCOUNT, N(issue), N(user1), mvo()
                          ("to", "user1")
                          ("token", _token)
                          ("memo", "")),
                          missing_auth_exception);

      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_USER_TOKEN_ACCOUNT, N(redeem), N(user1), mvo()
                          ("token", _token)
                          ("memo", "")),
                          missing_auth_exception);

      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_USER_TOKEN_ACCOUNT, N(transfer), N(user2), mvo()
                          ("from", "user1")
                          ("to", "user2")
                          ("token", _token)
                          ("memo", "")),
                          missing_auth_exception);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(create_test, yx_token_tester) try {
      auto result = token_create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("assertion failure with message: already created", result);
      produce_blocks(1);

      result = token_create("4,ETH@tkprovider2", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_create("4,ETH@tkprovider3", 0);
      BOOST_REQUIRE_EQUAL("action's authorizing actor 'tkprovider3' does not exist", result);
      produce_blocks(1);

      symbol _symbol = symbol::from_string("4,ETH");
      auto stats = token_get_stats(_symbol.value(), N(tkprovider));
      BOOST_REQUIRE_EQUAL("0.0000 ETH", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("tkprovider", stats["supply"]["issuer"].get_string());

      stats = token_get_stats(_symbol.value(), N(tkprovider2));
      BOOST_REQUIRE_EQUAL("0.0000 ETH", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("tkprovider2", stats["supply"]["issuer"].get_string());

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(issue_test, yx_token_tester) try {

      auto result = token_create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      /*
      auto accounts = token_get_accounts(N(user1), N(tkprovider));
      string str;
      fc::from_variant(accounts["token"], str);
      BOOST_REQUIRE_EQUAL("10000.0000 ETH@tkprovider", str);
      */

      produce_blocks(1);

      result = token_issue(N(user1), "-1.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = token_issue(N(user1), "0.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = token_issue(N(user1), "1.000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = token_issue(N(user1), "10000.0000 ETH@user2", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = token_issue(N(user1), "10000.00 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: cannot issue the native token with this action",
            result);

      //overflow test
      result = token_issue(N(user1), "461168601842738.7903 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token amount cannot be more than 2^62 - 1", result);

      // memo = 256 bytes
      result = token_issue(N(user2), "10000.0000 ETH@tkprovider",
                     "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);

      // memo = 257 bytes
      result = token_issue(N(user2), "10000.0000 ETH@tkprovider",
                     "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(redeem_test, yx_token_tester) try {

      auto result = token_create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_transfer(N(user1), N(tkprovider), "9000.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      BOOST_REQUIRE_EXCEPTION(token_redeem("11000.0000 ETH@tkprovider", ""),
                              eosio_assert_message_exception,
                              eosio_assert_message_is("redeem token exceeds supply amount"));

      BOOST_REQUIRE_EXCEPTION(token_redeem("9500.0000 ETH@tkprovider", ""),
                              eosio_assert_message_exception,
                              eosio_assert_message_is("insufficient token balance"));

      token_redeem("8000.0000 ETH@tkprovider", "my redeem");
      produce_blocks(1);

      symbol _symbol = symbol::from_string("4,ETH");
      auto stats = token_get_stats(_symbol.value(), N(tkprovider));
      BOOST_REQUIRE_EQUAL("2000.0000 ETH", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("tkprovider", stats["supply"]["issuer"].get_string());

      result = token_redeem_with_simple_result("-1.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = token_redeem_with_simple_result("0.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = token_redeem_with_simple_result("1.000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = token_redeem_with_simple_result("1.000 ETH@tkprovider2", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = token_redeem_with_simple_result("10000.00 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: cannot redeem the native token with this action", result);

      // memo = 256 bytes
      result = token_redeem_with_simple_result("1.0000 ETH@tkprovider",
                                         "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);

      // memo = 257 bytes
      result = token_redeem_with_simple_result("1.0000 ETH@tkprovider",
                                         "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(transfer_test, yx_token_tester) try {
      auto result = token_create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("assertion failure with message: already created", result);
      produce_blocks(1);

      result = token_create("4,ETH@tkprovider2", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      create_accounts({N(user3)});
      produce_blocks();

      result = token_issue(N(user1), "5000.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_issue(N(user1), "5000.0000 ETH@tkprovider2", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      /*
      auto accounts = token_get_accounts(N(user1), N(tkprovider));
      REQUIRE_MATCHING_OBJECT(accounts, mvo()
            ("token", "5000.0000 ETH@tkprovider")
      );

      accounts = token_get_accounts(N(user1), N(tkprovider2));
      REQUIRE_MATCHING_OBJECT(accounts, mvo()
            ("token", "5000.0000 ETH@tkprovider2")
      );
      */

      result = token_transfer(N(user1), N(user2), "7000.0000 ETH@tkprovider", "my transfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: insufficient token balance",
                          result);
      produce_blocks(1);

      result = token_transfer(N(user1), N(user2), "5000.0000 ETH@tkprovider", "my transfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      /*
      accounts = token_get_accounts(N(user1), N(tkprovider2));
      REQUIRE_MATCHING_OBJECT(accounts, mvo()
            ("token", "")
      );
      */

      result = token_wptransfer(N(user1), N(user2), "1000.0000 ETH@tkprovider2", N(tkprovider2), "my wptransfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      /*
      accounts = token_get_accounts(N(user1), N(tkprovider));
      BOOST_REQUIRE_EQUAL("", accounts);

      accounts = token_get_accounts(N(user2), N(tkprovider));
      REQUIRE_MATCHING_OBJECT(accounts, mvo()
            ("amount", "9900.00 DKRW")
      );
      */

      result = token_transfer(N(user1), N(user2), "-1.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive token", result);

      result = token_transfer(N(user1), N(user2), "0.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive token", result);

      result = token_transfer(N(user1), N(user2), "1.000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = token_transfer(N(user1), N(user2), "1.0000 ETH@tkprovider3", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = token_transfer(N(user1), N(user2), "10000.00 DKRW@d2", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: cannot transfer native token with this contract; use yx.ntoken", result);

      //overflow test
      result = token_issue(N(user3), "361168601842738.7903 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_transfer(N(user3), N(user2), "361068601842738.7903 ETH@tkprovider", "my transfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_issue(N(user3), "361168601842738.7903 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token amount cannot be more than 2^62 - 1", result);
      produce_blocks(1);

      // memo = 256 bytes
      result = token_transfer(N(user3), N(user2), "1.0000 ETH@tkprovider",
                         "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      // memo = 257 bytes
      result = token_transfer(N(user3), N(user2), "1.0000 ETH@tkprovider",
                         "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(kyc_test, yx_token_tester) try {

      auto result = set_id_info(N(tkprovider), N(d1), 0, 15, 0, "");

      const string token_symbol = "4,ETH@tkprovider";
      result = token_create(token_symbol, 0b0000000000000100); // == TOKEN_CAN_SET_OPTIONS_SET_KYC_RULE
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      token_set_kyc_rule(token_symbol, 0, 15); // TOKEN_KYC_RULE_TYPE_TRANSFER_SEND == 0
      token_set_kyc_rule(token_symbol, 1, 15); // TOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE == 1
      produce_blocks();

      result = token_issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for to account is failed", result);
      produce_blocks();

      result = set_id_info(N(user1), N(d1), 0, 6, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for to account is failed", result);
      produce_blocks();

      result = set_id_info(N(user1), N(d1), 0, 15, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_transfer(N(user2), N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for from account is failed", result);
      produce_blocks();

      result = set_id_info(N(user2), N(d1), 0, 15, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_issue(N(user2), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_transfer(N(user2), N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(freeze_test, yx_token_tester) try {
      const string token_symbol = "4,ETH@tkprovider";
      auto result = token_create(token_symbol, 0b0000000000000011);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_freeze_account(token_symbol, {N(user1)}, true);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: account is frozen by token issuer", result);
      produce_blocks();
 
      result = token_freeze_account(token_symbol, {N(user1)}, false);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_freeze_account(token_symbol, {N(user2)}, true);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: account is frozen by token issuer", result);
      produce_blocks();

      result = token_freeze_account(token_symbol, {N(user2)}, false);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_set_options(token_symbol, 1, true); // 1 == TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token transfer is frozen", result);
      produce_blocks();

      result = token_set_options(token_symbol, 0, true);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();
   } FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
