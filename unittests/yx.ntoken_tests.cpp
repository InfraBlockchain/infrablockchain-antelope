#include <yosemite/testing/yx_tester.hpp>

using namespace yosemite::testing;

class yx_ntoken_tester : public yx_tester {
public:
   yx_ntoken_tester() {
      init_yosemite_contracts();

      // turn off KYC for test convenience
      set_kyc_rule(0, 0);
      set_kyc_rule(1, 0);
      produce_blocks();
   }

   fc::variant get_stats(const account_name &depo) {
      vector<char> data = get_row_by_account(YOSEMITE_NATIVE_TOKEN_ACCOUNT, depo, N(ntstats), N(basicstats));
      return data.empty() ? fc::variant() : abi_ser_ntoken.binary_to_variant("stats_type", data, abi_serializer_max_time);
   }

   fc::variant get_accounts(const account_name &acc, const account_name &depo) {
      vector<char> data = get_row_by_account(YOSEMITE_NATIVE_TOKEN_ACCOUNT, acc, N(ntaccounts), depo);
      return data.empty() ? fc::variant() : abi_ser_ntoken.binary_to_variant("account_type", data, abi_serializer_max_time);
   }

   fc::variant get_accounts_total(const account_name &acc) {
      vector<char> data = get_row_by_account(YOSEMITE_NATIVE_TOKEN_ACCOUNT, acc, N(ntaccountstt), N(totalbal));
      return data.empty() ? fc::variant() : abi_ser_ntoken.binary_to_variant("account_total_type", data,
                                                                      abi_serializer_max_time);
   }

   action_result set_kyc_rule(uint8_t type, uint16_t kyc_flags) {
      return push_action(N(setkycrule), mvo()
            ("type", type)
            ("kyc", kyc_flags), config::system_account_name, abi_ser_ntoken);
   }

   action_result nissue(const account_name &to, const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(nissue), mvo()
            ("to", to)
            ("token", _token)
            ("memo", memo), _token.issuer, abi_ser_ntoken);
   }

   transaction_trace_ptr nredeem(const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(nredeem), _token.issuer, mvo()
            ("token", _token)
            ("memo", memo));
   }

   action_result nredeem_with_simple_result(const string &token, const string &memo) {
      try {
         nredeem(token, memo);
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   action_result transfer(account_name from, account_name to, const string &amount, const string &memo) {
      return push_action(N(transfer), mvo()
            ("from", from)
            ("to", to)
            ("amount", amount)
            ("memo", memo), from, abi_ser_ntoken);
   }

   action_result
   wptransfer(account_name from, account_name to, const string &amount, account_name payer, const string &memo) {
      try {
         base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(wptransfer),
                                  vector<account_name>{from, payer}, mvo()
                                        ("from", from)
                                        ("to", to)
                                        ("amount", amount)
                                        ("payer", payer)
                                        ("memo", memo));
         return success();
      } catch (const fc::exception &ex) {
         return error(ex.top_message());
      }
   }

   action_result ntransfer(account_name from, account_name to, const string &token, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(ntransfer), mvo()
            ("from", from)
            ("to", to)
            ("token", _token)
            ("memo", memo), from, abi_ser_ntoken);
   }

   action_result
   wpntransfer(account_name from, account_name to, const string &token, account_name payer, const string &memo) {
      try {
         auto _token = yx_asset::from_string(token);
         base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(wpntransfer),
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

BOOST_AUTO_TEST_SUITE(yx_ntoken_tests)

   BOOST_FIXTURE_TEST_CASE(required_auth_test, yx_ntoken_tester) try {

      auto _token = yx_asset::from_string("1.0000 DKRW@d1");
      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(nissue), N(user1), mvo()
            ("to", "user1")
            ("token", _token)
            ("memo", "")),
                          missing_auth_exception);

      _token = yx_asset::from_string("1.0000 DKRW@d1");
      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(nredeem), N(user1), mvo()
            ("token", _token)
            ("memo", "")),
                          missing_auth_exception);

      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(transfer), N(user2), mvo()
            ("from", "user1")
            ("to", "user2")
            ("amount", "1.0000 DKRW")
            ("memo", "")),
                          missing_auth_exception);

      auto result = nissue(N(user1), "5000.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(wptransfer), N(user1), mvo()
            ("from", "user1")
            ("to", "user2")
            ("amount", "1.0000 DKRW")
            ("payer", "d1")
            ("memo", "")),
                          missing_auth_exception);

      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(ntransfer), N(user2), mvo()
            ("from", "user1")
            ("to", "user2")
            ("token", _token)
            ("memo", "")),
                          missing_auth_exception);

      BOOST_REQUIRE_THROW(base_tester::push_action(YOSEMITE_NATIVE_TOKEN_ACCOUNT, N(wpntransfer), N(user1), mvo()
            ("from", "user1")
            ("to", "user2")
            ("token", _token)
            ("payer", "d1")
            ("memo", "")),
                          missing_auth_exception);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(nissue_test, yx_ntoken_tester) try {

      auto result = nissue(N(user1), "10000.0000 DKRW@d1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      auto stats = get_stats(N(d1));
      REQUIRE_MATCHING_OBJECT(stats, mvo()
            ("key", "basicstats")
                  ("supply", "10000.0000 DKRW")
                  ("options", "0")
      );

      auto accounts_total = get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", "10000.0000 DKRW")
      );

      /*
      auto accounts = get_accounts(N(user1), N(d1));
      REQUIRE_MATCHING_OBJECT(accounts, mvo()
              ("token", fc::json::from_string("{\"amount\":\"10000.0000 DKRW\",\"issuer\":\"d1\"}"))
      );
      */

      produce_blocks(1);

      result = nissue(N(user1), "-1.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = nissue(N(user1), "0.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = nissue(N(user1), "0.000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: invalid native token", result);

      result = nissue(N(user1), "10000.0000 DKRW@user2", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: issuer account is not system depository", result);

      result = nissue(N(user1), "10000.0000 DUSD@d1", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: cannot issue non-native token with this operation or wrong precision is specified",
            result);

      //overflow test
      result = nissue(N(user1), "461168601842738.7903 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: cannot issue token more than 2^62 - 1", result);

      // memo = 256 bytes
      result = nissue(N(user2), "10000.0000 DKRW@d1",
                      "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);

      // memo = 257 bytes
      result = nissue(N(user2), "10000.0000 DKRW@d1",
                      "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(nredeem_test, yx_ntoken_tester) try {

      auto result = nissue(N(user1), "10000.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = transfer(N(user1), N(d1), "9000.0000 DKRW", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      BOOST_REQUIRE_EXCEPTION(nredeem("10000.0000 DKRW@d1", ""),
                              eosio_assert_message_exception,
                              eosio_assert_message_is("insufficient native token of the specified depository"));

      BOOST_REQUIRE_EXCEPTION(nredeem("9000.0000 DKRW@d1", ""),
                              eosio_assert_message_exception,
                              eosio_assert_message_is("payer account cannot afford transaction fee"));

      result = nissue(N(d1), "10000.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      nredeem("9000.0000 DKRW@d1", "my redeem");
      produce_blocks(1);

      auto stats = get_stats(N(d1));
      REQUIRE_MATCHING_OBJECT(stats, mvo()
            ("key", "basicstats")
                  ("supply", "11000.0000 DKRW")
                  ("options", "0")
      );

      auto accounts_total = get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", "900.0000 DKRW")
      );

      // check transaction fee
      accounts_total = get_accounts_total(YOSEMITE_TX_FEE_ACCOUNT);
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", "1100.0000 DKRW")
      );

      result = nredeem_with_simple_result("9001.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: insufficient native token of the specified depository",
                          result);

      result = nredeem_with_simple_result("11001.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: insufficient supply of the native token of the specified depository",
            result);

      result = nredeem_with_simple_result("-1.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = nredeem_with_simple_result("0.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must be positive token", result);

      result = nredeem_with_simple_result("0.000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: invalid native token", result);

      result = nredeem_with_simple_result("10000.0000 DKRW@user2", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: issuer account is not system depository", result);

      // memo = 256 bytes
      result = nredeem_with_simple_result("1.0000 DKRW@d1",
                                          "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);

      // memo = 257 bytes
      result = nredeem_with_simple_result("1.0000 DKRW@d1",
                                          "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(transfer_test, yx_ntoken_tester) try {

      create_accounts({N(d2), N(user3)});
      produce_blocks();

      prepare_system_depository(N(d2));
      produce_blocks();

      auto result = nissue(N(user1), "5000.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = nissue(N(user1), "5000.0000 DKRW@d2", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      auto accounts_total = get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", "10000.0000 DKRW")
      );

      result = transfer(N(user1), N(user2), "7000.0000 DKRW", "my transfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      accounts_total = get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", "2900.0000 DKRW")
      );

      accounts_total = get_accounts_total(N(user2));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", "7000.0000 DKRW")
      );

      result = wptransfer(N(user1), N(user2), "2000.0000 DKRW", N(d2), "my wptransfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: payer account cannot afford transaction fee", result);
      produce_blocks(1);

      result = wptransfer(N(user1), N(user2), "2900.0000 DKRW", N(user2), "my wptransfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      accounts_total = get_accounts_total(N(user1));
      BOOST_REQUIRE_EQUAL("", accounts_total); // check if 0.0000 DKRW

      result = transfer(N(user1), N(user2), "-1.0000 DKRW", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive amount", result);

      result = transfer(N(user1), N(user2), "0.0000 DKRW", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive amount", result);

      result = transfer(N(user1), N(user2), "0.000 DKRW", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: invalid native token", result);

      result = transfer(N(user1), N(user2), "10000.0000 DKRW", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: from account cannot afford native token amount", result);

      result = transfer(N(user1), N(user2), "10000.0000 DUSD", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: only native token is supported; use yx.token::transfer instead", result);

      //overflow test
      result = nissue(N(user3), "361168601842738.7903 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = transfer(N(user3), N(user2), "361068601842738.7903 DKRW", "my transfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = nissue(N(user3), "361168601842738.7903 DKRW@d2", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = transfer(N(user3), N(user2), "361068601842738.7903 DKRW", "my transfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token amount cannot be more than 2^62 - 1", result);
      produce_blocks(1);

      // memo = 256 bytes
      result = transfer(N(user3), N(user2), "1.0000 DKRW",
                        "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      // memo = 257 bytes
      result = transfer(N(user3), N(user2), "1.0000 DKRW",
                        "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(ntransfer_test, yx_ntoken_tester) try {

      create_accounts({N(d2), N(user3)});
      produce_blocks();

      prepare_system_depository(N(d2));
      produce_blocks();

      auto result = nissue(N(user1), "5000.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = nissue(N(user1), "5000.0000 DKRW@d2", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      auto accounts_total = get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", "10000.0000 DKRW")
      );

      result = ntransfer(N(user1), N(user2), "7000.0000 DKRW@d1", "my ntransfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: insufficient native token of the specified depository",
                          result);
      produce_blocks(1);

      result = ntransfer(N(user1), N(user2), "5000.0000 DKRW@d1", "my ntransfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      accounts_total = get_accounts_total(N(user1));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", "4900.0000 DKRW")
      );

      result = wpntransfer(N(user1), N(user2), "4900.0000 DKRW@d2", N(d2), "my wpntransfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: payer account cannot afford transaction fee", result);
      produce_blocks(1);

      result = nissue(N(d2), "5000.0000 DKRW@d2", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = wpntransfer(N(user1), N(user2), "4900.0000 DKRW@d2", N(d2), "my wptransfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      accounts_total = get_accounts_total(N(user1));
      BOOST_REQUIRE_EQUAL("", accounts_total);

      accounts_total = get_accounts_total(N(user2));
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", "9900.0000 DKRW")
      );

      result = ntransfer(N(user1), N(user2), "-1.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive amount", result);

      result = ntransfer(N(user1), N(user2), "0.0000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: must transfer positive amount", result);

      result = ntransfer(N(user1), N(user2), "0.000 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("assertion failure with message: invalid native token", result);

      result = ntransfer(N(user1), N(user2), "10000.0000 DUSD@d2", "");
      BOOST_REQUIRE_EQUAL(
            "assertion failure with message: only native token is supported; use yx.token::transfer instead", result);

      //overflow test
      result = nissue(N(user3), "361168601842738.7903 DKRW@d1", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = ntransfer(N(user3), N(user2), "361068601842738.7903 DKRW@d1", "my transfer");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = nissue(N(user3), "361168601842738.7903 DKRW@d2", "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = ntransfer(N(user3), N(user2), "361068601842738.7903 DKRW@d2", "my transfer");
      BOOST_REQUIRE_EQUAL("assertion failure with message: token amount cannot be more than 2^62 - 1", result);
      produce_blocks(1);

      // memo = 256 bytes
      result = ntransfer(N(user3), N(user2), "1.0000 DKRW@d1",
                         "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      // memo = 257 bytes
      result = ntransfer(N(user3), N(user2), "1.0000 DKRW@d1",
                         "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");
      BOOST_REQUIRE_EQUAL("assertion failure with message: memo has more than 256 bytes", result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(kyc_test, yx_ntoken_tester) try {

      auto result = nissue(N(user2), "11000.0000 DKRW@d1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      // set KYC rule as default
      set_kyc_rule(0, 15); // NTOKEN_KYC_RULE_TYPE_TRANSFER_SEND == 0
      set_kyc_rule(1, 15); // NTOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE == 1
      produce_blocks();

      result = nissue(N(user1), "10000.0000 DKRW@d1", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for to account is failed", result);
      produce_blocks();

      result = set_id_info(N(user1), N(d1), 0, 6, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = nissue(N(user1), "10000.0000 DKRW@d1", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for to account is failed", result);
      produce_blocks();

      result = set_id_info(N(user1), N(d1), 0, 15, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = ntransfer(N(user2), N(user1), "10000.0000 DKRW@d1", "my memo");
      BOOST_REQUIRE_EQUAL("assertion failure with message: KYC authentication for from account is failed", result);
      produce_blocks();

      result = set_id_info(N(user2), N(d1), 0, 15, 0, "");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = ntransfer(N(user2), N(user1), "10000.0000 DKRW@d1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(txfee_test, yx_ntoken_tester) try {
      create_accounts({N(d2)});
      produce_blocks();

      prepare_system_depository(N(d2));
      produce_blocks();

      auto result = nissue(N(user1), "50.0000 DKRW@d1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = nissue(N(user1), "500.0000 DKRW@d2", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = transfer(N(user1), N(user2), "100.0000 DKRW", "my transfer for txfee");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      // check transaction fee
      auto accounts_total = get_accounts_total(YOSEMITE_TX_FEE_ACCOUNT);
      REQUIRE_MATCHING_OBJECT(accounts_total, mvo()
            ("amount", "100.0000 DKRW")
      );

   } FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
