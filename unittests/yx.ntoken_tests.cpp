#include <yosemite/testing/yx_tester.hpp>

using namespace yosemite::testing;

class yx_ntoken_tester : public yx_tester {
public:
   yx_ntoken_tester() {
      init_yosemite_contracts();

      // turn off KYC for test convenience
      ntoken_set_kyc_rule(0, 0);
      ntoken_set_kyc_rule(1, 0);
      produce_blocks();
   }
};

BOOST_AUTO_TEST_SUITE(yx_ntoken_tests)

   BOOST_FIXTURE_TEST_CASE(required_auth_test, yx_ntoken_tester) try {
      auto _token = yx_asset::from_string(to_yx_asset_string(100, "d1"));
      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(nissue), N(user1), mvo()
            ("to", "user1")
            ("token", _token)
            ("memo", "")),
                          missing_auth_exception);

      _token = yx_asset::from_string(to_yx_asset_string(100, "d1"));
      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(nredeem), N(user1), mvo()
            ("token", _token)
            ("memo", "")),
                          missing_auth_exception);

      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(transfer), N(user2), mvo()
            ("from", "user1")
            ("to", "user2")
            ("amount", to_asset_string(100))
            ("memo", "")),
                          missing_auth_exception);

      auto result = nissue(N(user1), to_yx_asset_string(500000, "d1"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(ntransfer), N(user2), mvo()
            ("from", "user1")
            ("to", "user2")
            ("token", _token)
            ("memo", "")),
                          missing_auth_exception);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(nissue_test, yx_ntoken_tester) try {

      auto result = nissue(N(user1), to_yx_asset_string(1000000, "d1"), "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      auto stats = ntoken_get_stats(N(d1));
      REQUIRE_MATCHING_OBJECT(stats, mvo()
            ("key", "basicstats")
                  ("supply", to_asset_string(1000000))
                  ("options", "0")
      );

      auto accounts_total = ntoken_get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(1000000))
      );

      /*
      auto accounts = ntoken_get_accounts(N(user1), N(d1));
      REQUIRE_MATCHING_OBJECT(accounts, mvo()
              ("token", fc::json::from_string("{\"amount\":\"10000.00 DKRW\",\"issuer\":\"d1\"}"))
      );
      */

      produce_blocks(1);

      result = nissue(N(user1), to_yx_asset_string(-100, "d1"), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = nissue(N(user1), to_yx_asset_string(0, "d1"), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = nissue(N(user1), "1.000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: invalid native token", result);

      result = nissue(N(user1), "10000.00 DKRW@user2", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: issuer account is not system depository", result);

      result = nissue(N(user1), "10000.0000 DUSD@d1", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: cannot issue non-native token with this operation or wrong precision is specified",
            result);

      //overflow test
      result = nissue(N(user1), to_yx_asset_string(4611686018427387903L, "d1"), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: cannot issue token more than 2^62 - 1", result);

      // memo = 256 bytes
      result = nissue(N(user2), to_yx_asset_string(1000000, "d1"),
                      "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);

      // memo = 257 bytes
      result = nissue(N(user2), to_yx_asset_string(1000000, "d1"),
                      "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(nredeem_test, yx_ntoken_tester) try {

      auto result = nissue(N(user1), to_yx_asset_string(1000000, "d1"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = ntoken_transfer(N(user1), N(d1), to_asset_string(900000), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      BOOST_REQUIRE_EXCEPTION(nredeem(to_yx_asset_string(1000000, "d1"), ""),
                              eosio_assert_message_exception,
                              eosio_assert_message_is("insufficient native token of the specified depository"));

      BOOST_REQUIRE_EXCEPTION(nredeem(to_yx_asset_string(900000, "d1"), ""),
                              eosio_assert_message_exception,
                              eosio_assert_message_is("payer account cannot afford transaction fee"));

      result = nissue(N(d1), to_yx_asset_string(1000000, "d1"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      nredeem(to_yx_asset_string(900000, "d1"), "my redeem");
      produce_blocks(1);

      auto stats = ntoken_get_stats(N(d1));
      REQUIRE_MATCHING_OBJECT(stats, mvo()
            ("key", "basicstats")
            ("supply", to_asset_string(1100000))
            ("options", "0")
      );

      auto accounts_total = ntoken_get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(90000))
      );

      // check transaction fee
      accounts_total = ntoken_get_accounts_total(YOSEMITE_TX_FEE_ACCOUNT);
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(110000))
      );

      result = nredeem_with_simple_result(to_yx_asset_string(900100, "d1"), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: insufficient native token of the specified depository",
                          result);

      result = nredeem_with_simple_result(to_yx_asset_string(1100100, "d1"), "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: insufficient supply of the native token of the specified depository",
            result);

      result = nredeem_with_simple_result(to_yx_asset_string(-100, "d1"), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = nredeem_with_simple_result(to_yx_asset_string(0, "d1"), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = nredeem_with_simple_result("1.000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: invalid native token", result);

      result = nredeem_with_simple_result("10000.00 DKRW@user2", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: issuer account is not system depository", result);

      // memo = 256 bytes
      result = nredeem_with_simple_result(to_yx_asset_string(100, "d1"),
                                          "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);

      // memo = 257 bytes
      result = nredeem_with_simple_result(to_yx_asset_string(100, "d1"),
                                          "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(transfer_test, yx_ntoken_tester) try {

      create_accounts({N(d2), N(user3)});
      produce_blocks();

      prepare_system_depository(N(d2));
      produce_blocks();

      auto result = nissue(N(user1), to_yx_asset_string(500000, "d1"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = nissue(N(user1), to_yx_asset_string(500000, "d2"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      auto accounts_total = ntoken_get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(1000000))
      );

      result = ntoken_transfer(N(user1), N(user2), to_asset_string(700000), "my transfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      accounts_total = ntoken_get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(290000))
      );

      accounts_total = ntoken_get_accounts_total(N(user2));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(700000))
      );

      /*
      result = ntoken_wptransfer(N(user1), N(user2), to_asset_string(200000), N(d2), "my wptransfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: payer account cannot afford transaction fee", result);
      produce_blocks(1);

      result = ntoken_wptransfer(N(user1), N(user2), to_asset_string(290000), N(user2), "my wptransfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      accounts_total = ntoken_get_accounts_total(N(user1));
      BOOST_REQUIRE_EQUAL("", accounts_total); // check if 0.00 DKRW
      */

      result = ntoken_transfer(N(user1), N(user2), to_asset_string(-100), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive amount", result);

      result = ntoken_transfer(N(user1), N(user2), to_asset_string(0), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive amount", result);

      result = ntoken_transfer(N(user1), N(user2), "1.000 DKRW", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: invalid native token", result);

      result = ntoken_transfer(N(user1), N(user2), to_asset_string(1000000), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: from account cannot afford native token amount", result);

      result = ntoken_transfer(N(user1), N(user2), "10000.0000 DUSD", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: only native token is supported; use yx.token::transfer instead", result);

      //overflow test
      result = nissue(N(user3), to_yx_asset_string(3611686018427387903L, "d1"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = ntoken_transfer(N(user3), N(user2), to_asset_string(3610686018427387903L), "my transfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = nissue(N(user3), to_yx_asset_string(3611686018427387903L, "d2"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = ntoken_transfer(N(user3), N(user2), to_asset_string(3610686018427387903L), "my transfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token amount cannot be more than 2^62 - 1", result);
      produce_blocks(1);

      // memo = 256 bytes
      result = ntoken_transfer(N(user3), N(user2), to_asset_string(100),
                        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      // memo = 257 bytes
      result = ntoken_transfer(N(user3), N(user2), to_asset_string(100),
                        "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(ntransfer_test, yx_ntoken_tester) try {

      create_accounts({N(d2), N(user3)});
      produce_blocks();

      prepare_system_depository(N(d2));
      produce_blocks();

      auto result = nissue(N(user1), to_yx_asset_string(500000, "d1"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = nissue(N(user1), to_yx_asset_string(500000, "d2"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      auto accounts_total = ntoken_get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(1000000))
      );

      result = ntransfer(N(user1), N(user2), to_yx_asset_string(700000, "d1"), "my ntransfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: insufficient native token of the specified depository",
                          result);
      produce_blocks(1);

      result = ntransfer(N(user1), N(user2), to_yx_asset_string(500000, "d1"), "my ntransfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      accounts_total = ntoken_get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(490000))
      );

      /*
      result = wpntransfer(N(user1), N(user2), to_yx_asset_string(490000, "d2"), N(d2), "my wpntransfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: payer account cannot afford transaction fee", result);
      produce_blocks(1);
      */

      result = nissue(N(d2), to_yx_asset_string(500000, "d2"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      /*
      result = wpntransfer(N(user1), N(user2), to_yx_asset_string(490000, "d2"), N(d2), "my wptransfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);
      */

      result = ntransfer(N(user1), N(user2), to_yx_asset_string(490000, "d2"), "my ntransfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: payer account cannot afford transaction fee", result);
      produce_blocks(1);

      result = nissue(N(user1), to_yx_asset_string(500000, "d2"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = ntransfer(N(user1), N(user2), to_yx_asset_string(490000, "d2"), "my ntransfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      accounts_total = ntoken_get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(490000))
      );

      accounts_total = ntoken_get_accounts_total(N(user2));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(990000))
      );

      result = ntransfer(N(user1), N(user2), to_yx_asset_string(-100, "d1"), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive amount", result);

      result = ntransfer(N(user1), N(user2), to_yx_asset_string(0, "d1"), "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive amount", result);

      result = ntransfer(N(user1), N(user2), "0.000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: invalid native token", result);

      result = ntransfer(N(user1), N(user2), "10000.0000 DUSD@d2", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: only native token is supported; use yx.token::transfer instead", result);

      //overflow test
      result = nissue(N(user3), to_yx_asset_string(3611686018427387903L, "d1"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = ntransfer(N(user3), N(user2), to_yx_asset_string(3610686018427387903L, "d1"), "my transfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = nissue(N(user3), to_yx_asset_string(3611686018427387903L, "d2"), "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = ntransfer(N(user3), N(user2), to_yx_asset_string(3610686018427387903L, "d2"), "my transfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token amount cannot be more than 2^62 - 1", result);
      produce_blocks(1);

      // memo = 256 bytes
      result = ntransfer(N(user3), N(user2), to_yx_asset_string(100, "d1"),
                         "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      // memo = 257 bytes
      result = ntransfer(N(user3), N(user2), to_yx_asset_string(100, "d1"),
                         "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(kyc_test, yx_ntoken_tester) try {

      auto result = nissue(N(user2), to_yx_asset_string(1100000, "d1"), "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      // set KYC rule as default
      ntoken_set_kyc_rule(0, 15); // NTOKEN_KYC_RULE_TYPE_TRANSFER_SEND == 0
      ntoken_set_kyc_rule(1, 15); // NTOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE == 1
      produce_blocks();

      result = nissue(N(user1), to_yx_asset_string(1000000, "d1"), "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for to account is failed", result);
      produce_blocks();

      result = set_id_info(N(user1), N(d1), 0, 6, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = nissue(N(user1), to_yx_asset_string(1000000, "d1"), "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for to account is failed", result);
      produce_blocks();

      result = set_id_info(N(user1), N(d1), 0, 15, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = ntransfer(N(user2), N(user1), to_yx_asset_string(1000000, "d1"), "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for from account is failed", result);
      produce_blocks();

      result = set_id_info(N(user2), N(d1), 0, 15, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = ntransfer(N(user2), N(user1), to_yx_asset_string(1000000, "d1"), "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(txfee_test, yx_ntoken_tester) try {
      create_accounts({N(d2)});
      produce_blocks();

      prepare_system_depository(N(d2));
      produce_blocks();

      auto result = nissue(N(user1), to_yx_asset_string(5000, "d1"), "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = nissue(N(user1), to_yx_asset_string(50000, "d2"), "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = ntoken_transfer(N(user1), N(user2), to_asset_string(10000), "my transfer for txfee");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      // check transaction fee
      auto accounts_total = ntoken_get_accounts_total(YOSEMITE_TX_FEE_ACCOUNT);
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", to_asset_string(10000))
      );

   } FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
