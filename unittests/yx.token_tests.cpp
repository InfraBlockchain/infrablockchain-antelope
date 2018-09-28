#include <yosemite/testing/yx_tester.hpp>

using namespace yosemite::testing;

class yx_token_tester : public yx_tester {
public:
   yx_token_tester() {
      init_yosemite_contracts();

      create_accounts({N(tkprovider), N(tkprovider2)});
   }

   fc::variant get_stats(int64_t token_symbol, const account_name &issuer) {
      vector<char> data = get_row_by_account(YOSEMITE_USER_TOKEN_ACCOUNT, token_symbol, N(tstats), issuer);
      return data.empty() ? fc::variant() : abi_ser_token.binary_to_variant("stats_type", data, abi_serializer_max_time);
   }

   fc::variant get_accounts(const account_name &owner, const account_name &issuer) {
      vector<char> data = get_row_by_account(YOSEMITE_USER_TOKEN_ACCOUNT, owner, N(taccounts), issuer);
      return data.empty() ? fc::variant() : abi_ser_token.binary_to_variant("accounts_type", data, abi_serializer_max_time);
   }

   action_result set_kyc_rule(const string &ysymbol, uint8_t type, uint16_t kyc_flags) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(setkycrule), mvo()
            ("ysymbol", _ysymbol)
            ("type", type)
            ("kyc", kyc_flags), _ysymbol.issuer, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   action_result set_options(const string &ysymbol, uint16_t options, bool reset) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(setoptions), mvo()
            ("ysymbol", _ysymbol)
            ("options", options)
            ("reset", reset),
            _ysymbol.issuer, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   action_result freeze_account(const string &ysymbol, const vector<account_name> &accs, bool freeze) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(freezeacc), mvo()
            ("ysymbol", _ysymbol)
            ("accs", accs)
            ("freeze", freeze),
            _ysymbol.issuer, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   action_result create(const string &ysymbol, uint16_t can_set_options) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(create), mvo()
            ("ysymbol", _ysymbol)
            ("can_set_options", can_set_options),
            _ysymbol.issuer, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   action_result issue(const account_name &to, const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(issue), mvo()
            ("to", to)
            ("token", _token)
            ("memo", memo), _token.issuer, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   transaction_trace_ptr redeem(const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return base_tester::push_action(YOSEMITE_USER_TOKEN_ACCOUNT, N(redeem), _token.issuer, mvo()
            ("token", _token)
            ("memo", memo));
   }

   action_result redeem_with_simple_result(const string &token, const string &memo) {
      try {
         redeem(token, memo);
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   action_result transfer(account_name from, account_name to, const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(transfer), mvo()
            ("from", from)
            ("to", to)
            ("token", _token)
            ("memo", memo), from, abi_ser_token, YOSEMITE_USER_TOKEN_ACCOUNT);
   }

   action_result
   wptransfer(account_name from, account_name to, const string &token, account_name payer, const string &memo) {
      try {
         auto _token = yx_asset::from_string(token);
         base_tester::push_action(YOSEMITE_USER_TOKEN_ACCOUNT, N(wptransfer),
                                  vector<account_name>{from, payer}, mvo()
                                        ("from", from)
                                        ("to", to)
                                        ("token", _token)
                                        ("payer", payer)
                                        ("memo", memo));
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

};

BOOST_AUTO_TEST_SUITE(yx_token_tests)

   BOOST_FIXTURE_TEST_CASE(required_auth_test, yx_token_tester) try {

      yx_symbol _ysymbol = yx_symbol::from_string("4,ETH@tkprovider");
      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_USER_TOKEN_ACCOUNT, N(create), N(user1), mvo()
                          ("ysymbol", _ysymbol)
                          ("can_set_options", 0)),
                          missing_auth_exception);

      auto result = create("4,ETH@tkprovider", 0);
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
      auto result = create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("assertion failure with message: already created", result);
      produce_blocks(1);

      result = create("4,ETH@tkprovider2", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = create("4,ETH@tkprovider3", 0);
      BOOST_REQUIRE_EQUAL("action's authorizing actor 'tkprovider3' does not exist", result);
      produce_blocks(1);

      symbol _symbol = symbol::from_string("4,ETH");
      auto stats = get_stats(_symbol.value(), N(tkprovider));
      REQUIRE_MATCHING_OBJECT(stats, mvo()
            ("issuer", "tkprovider")
            ("supply", "0.0000 ETH")
            ("can_set_options", 0)
            ("options", 0)
            ("kyc_rules", vector<uint8_t>{})
            ("kyc_rule_flags", vector<uint16_t>{})
      );

      stats = get_stats(_symbol.value(), N(tkprovider2));
      REQUIRE_MATCHING_OBJECT(stats, mvo()
            ("issuer", "tkprovider2")
            ("supply", "0.0000 ETH")
            ("can_set_options", 0)
            ("options", 0)
            ("kyc_rules", vector<uint8_t>{})
            ("kyc_rule_flags", vector<uint16_t>{})
      );
   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(issue_test, yx_token_tester) try {

      auto result = create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      /*
      auto accounts = get_accounts(N(user1), N(tkprovider));
      log_to_console(accounts["token"].as_string());
      BOOST_REQUIRE_EQUAL(accounts["token"].as_string(), "10000.0000 ETH@tkprovider");
      */

      produce_blocks(1);

      result = issue(N(user1), "-1.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = issue(N(user1), "0.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = issue(N(user1), "1.000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = issue(N(user1), "10000.0000 ETH@user2", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = issue(N(user1), "10000.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: cannot issue the native token with this action",
            result);

      //overflow test
      result = issue(N(user1), "461168601842738.7903 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token amount cannot be more than 2^62 - 1", result);

      // memo = 256 bytes
      result = issue(N(user2), "10000.0000 ETH@tkprovider",
                     "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);

      // memo = 257 bytes
      result = issue(N(user2), "10000.0000 ETH@tkprovider",
                     "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(redeem_test, yx_token_tester) try {

      auto result = create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = transfer(N(user1), N(tkprovider), "9000.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      BOOST_REQUIRE_EXCEPTION(redeem("11000.0000 ETH@tkprovider", ""),
                              eosio_assert_message_exception,
                              eosio_assert_message_is("redeem token exceeds supply amount"));

      BOOST_REQUIRE_EXCEPTION(redeem("9500.0000 ETH@tkprovider", ""),
                              eosio_assert_message_exception,
                              eosio_assert_message_is("insufficient token balance"));

      redeem("8000.0000 ETH@tkprovider", "my redeem");
      produce_blocks(1);

      symbol _symbol = symbol::from_string("4,ETH");
      auto stats = get_stats(_symbol.value(), N(tkprovider));
      REQUIRE_MATCHING_OBJECT(stats, mvo()
            ("issuer", "tkprovider")
            ("supply", "2000.0000 ETH")
            ("can_set_options", 0)
            ("options", 0)
            ("kyc_rules", vector<uint8_t>{})
            ("kyc_rule_flags", vector<uint16_t>{})
      );

      result = redeem_with_simple_result("-1.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = redeem_with_simple_result("0.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = redeem_with_simple_result("1.000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = redeem_with_simple_result("1.000 ETH@tkprovider2", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = redeem_with_simple_result("10000.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: cannot redeem the native token with this action", result);

      // memo = 256 bytes
      result = redeem_with_simple_result("1.0000 ETH@tkprovider",
                                         "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);

      // memo = 257 bytes
      result = redeem_with_simple_result("1.0000 ETH@tkprovider",
                                         "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(transfer_test, yx_token_tester) try {
      auto result = create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = create("4,ETH@tkprovider", 0);
      BOOST_REQUIRE_EQUAL("assertion failure with message: already created", result);
      produce_blocks(1);

      result = create("4,ETH@tkprovider2", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      create_accounts({N(user3)});
      produce_blocks();

      result = issue(N(user1), "5000.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = issue(N(user1), "5000.0000 ETH@tkprovider2", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      /*
      auto accounts = get_accounts(N(user1), N(tkprovider));
      REQUIRE_MATCHING_OBJECT(accounts, mvo()
            ("token", "5000.0000 ETH@tkprovider")
      );

      accounts = get_accounts(N(user1), N(tkprovider2));
      REQUIRE_MATCHING_OBJECT(accounts, mvo()
            ("token", "5000.0000 ETH@tkprovider2")
      );
      */

      result = transfer(N(user1), N(user2), "7000.0000 ETH@tkprovider", "my transfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: insufficient token balance",
                          result);
      produce_blocks(1);

      result = transfer(N(user1), N(user2), "5000.0000 ETH@tkprovider", "my transfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      /*
      accounts = get_accounts(N(user1), N(tkprovider2));
      REQUIRE_MATCHING_OBJECT(accounts, mvo()
            ("token", "")
      );
      */

      result = wptransfer(N(user1), N(user2), "1000.0000 ETH@tkprovider2", N(tkprovider2), "my wptransfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      /*
      accounts = get_accounts(N(user1), N(tkprovider));
      BOOST_REQUIRE_EQUAL("", accounts);

      accounts = get_accounts(N(user2), N(tkprovider));
      REQUIRE_MATCHING_OBJECT(accounts, mvo()
            ("amount", "9900.0000 DKRW")
      );
      */

      result = transfer(N(user1), N(user2), "-1.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive token", result);

      result = transfer(N(user1), N(user2), "0.0000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive token", result);

      result = transfer(N(user1), N(user2), "1.000 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = transfer(N(user1), N(user2), "1.0000 ETH@tkprovider3", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token is not yet created", result);

      result = transfer(N(user1), N(user2), "10000.0000 DKRW@d2", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: cannot transfer native token with this contract; use yx.ntoken", result);

      //overflow test
      result = issue(N(user3), "361168601842738.7903 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = transfer(N(user3), N(user2), "361068601842738.7903 ETH@tkprovider", "my transfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = issue(N(user3), "361168601842738.7903 ETH@tkprovider", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token amount cannot be more than 2^62 - 1", result);
      produce_blocks(1);

      // memo = 256 bytes
      result = transfer(N(user3), N(user2), "1.0000 ETH@tkprovider",
                         "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      // memo = 257 bytes
      result = transfer(N(user3), N(user2), "1.0000 ETH@tkprovider",
                         "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(kyc_test, yx_token_tester) try {

      auto result = set_id_info(N(tkprovider), N(d1), 0, 15, 0, "");

      const string token_symbol = "4,ETH@tkprovider";
      result = create(token_symbol, 0b0000000000000100); // == TOKEN_CAN_SET_OPTIONS_SET_KYC_RULE
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      set_kyc_rule(token_symbol, 0, 15); // TOKEN_KYC_RULE_TYPE_TRANSFER_SEND == 0
      set_kyc_rule(token_symbol, 1, 15); // TOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE == 1
      produce_blocks();

      result = issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for to account is failed", result);
      produce_blocks();

      result = set_id_info(N(user1), N(d1), 0, 6, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for to account is failed", result);
      produce_blocks();

      result = set_id_info(N(user1), N(d1), 0, 15, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = transfer(N(user2), N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for from account is failed", result);
      produce_blocks();

      result = set_id_info(N(user2), N(d1), 0, 15, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = issue(N(user2), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = transfer(N(user2), N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(freeze_test, yx_token_tester) try {
      const string token_symbol = "4,ETH@tkprovider";
      auto result = create(token_symbol, 0b0000000000000011);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = issue(N(user1), "10000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = freeze_account(token_symbol, {N(user1)}, true);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: account is frozen by token issuer", result);
      produce_blocks();
 
      result = freeze_account(token_symbol, {N(user1)}, false);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = freeze_account(token_symbol, {N(user2)}, true);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: account is frozen by token issuer", result);
      produce_blocks();

      result = freeze_account(token_symbol, {N(user2)}, false);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = set_options(token_symbol, 1, true); // 1 == TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token transfer is frozen", result);
      produce_blocks();

      result = set_options(token_symbol, 0, true);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = transfer(N(user1), N(user2), "1000.0000 ETH@tkprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();
   } FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
