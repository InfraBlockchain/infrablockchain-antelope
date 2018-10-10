#include <yosemite/testing/yx_tester.hpp>

using namespace yosemite::testing;

typedef uint64_t id_type;
typedef string uri_type;

class nft_tester : public yx_tester {
public:

   nft_tester() {
      init_yosemite_contracts();

      create_accounts({N(alice), N(bob), N(carol)});
      produce_blocks();
   }

   fc::variant get_stats(int64_t token_symbol, const account_name &issuer) {
      vector<char> data = get_row_by_account(YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, token_symbol, N(tstats), issuer);
      return data.empty() ? fc::variant() : abi_ser_nft.binary_to_variant("stats_type", data, abi_serializer_max_time);
   }

   fc::variant get_accounts(const account_name &owner, const account_name &issuer) {
      vector<char> data = get_row_by_account(YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, owner, N(taccounts), issuer);
      return data.empty() ? fc::variant() : abi_ser_nft.binary_to_variant("accounts_type", data, abi_serializer_max_time);
   }

   fc::variant get_token(id_type token_id, account_name issuer) {
      vector<char> data = get_row_by_account(YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT, issuer, N(nftokens), token_id);
      FC_ASSERT(!data.empty(), "empty token");
      return data.empty() ? fc::variant() : abi_ser_nft.binary_to_variant("nftokens_type", data, abi_serializer_max_time);
   }

   action_result set_kyc_rule(const string &ysymbol, uint8_t type, uint16_t kyc_flags) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(setkycrule), mvo()
            ("ysymbol", _ysymbol)
            ("type", type)
            ("kyc", kyc_flags), _ysymbol.issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT);
   }

   action_result set_options(const string &ysymbol, uint16_t options, bool reset) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(setoptions), mvo()
                               ("ysymbol", _ysymbol)
                               ("options", options)
                               ("reset", reset),
                         _ysymbol.issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT);
   }

   action_result freeze_account(const string &ysymbol, const vector<account_name> &accs, bool freeze) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(freezeacc), mvo()
                               ("ysymbol", _ysymbol)
                               ("accs", accs)
                               ("freeze", freeze),
                         _ysymbol.issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT);
   }

   action_result create(const string &ysymbol, uint16_t can_set_options) {
      yx_symbol _ysymbol = yx_symbol::from_string(ysymbol);
      return push_action(N(create), mvo()
                               ("ysymbol", _ysymbol)
                               ("can_set_options", can_set_options),
                         _ysymbol.issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT);
   }

   template<typename T>
   action_result
   issue(const account_name &to, const string &token, const T &ids, const vector<string> &uris, const string &name, const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(issue), mvo()
                               ("to", to)
                               ("token", _token)
                               ("ids", ids)
                               ("uris", uris)
                               ("name", name)
                               ("memo", memo),
                         _token.issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT
      );
   }

   template<typename T>
   action_result transferid(account_name from,
                            account_name to,
                            account_name issuer,
                            const T &ids,
                            const string &memo) {
      return push_action(N(transferid), mvo()
                               ("from", from)
                               ("to", to)
                               ("issuer", issuer)
                               ("ids", ids)
                               ("memo", memo),
                         from, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT
      );
   }

   action_result transfer(account_name from,
                          account_name to,
                          const string &token,
                          const string &memo) {
      auto _token = yx_asset::from_string(token);
      return push_action(N(transfer), mvo()
                               ("from", from)
                               ("to", to)
                               ("token", _token)
                               ("memo", memo),
                         from, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT
      );
   }

   template<typename T>
   action_result redeem(account_name issuer, const T &ids, const string &memo) {
      return push_action(N(redeem), mvo()
                               ("issuer", issuer)
                               ("ids", ids)
                               ("memo", memo),
                         issuer, abi_ser_nft, YOSEMITE_NON_FUNGIBLE_TOKEN_ACCOUNT
      );
   }
};

BOOST_AUTO_TEST_SUITE(yx_nft_tests)

   BOOST_FIXTURE_TEST_CASE(create_tests, nft_tester) try {
      yx_symbol _ysymbol = yx_symbol::from_string("0,NFT@alice");

      auto result = create("0,NFT@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto stats = get_stats(_ysymbol.tsymbol.value(), N(alice));
      BOOST_REQUIRE_EQUAL("0 NFT", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", stats["supply"]["issuer"].get_string());

      BOOST_REQUIRE_EQUAL("action's authorizing actor 'dummy' does not exist",
                          create("0,TKN@dummy", 0)
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("already created"),
                          create("0,NFT@alice", 0)
      );

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE (issue_multi_tests, nft_tester) try {

      yx_symbol _ysymbol = yx_symbol::from_string("0,TKN@alice");

      auto result = create("0,TKN@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri1", "uri2", "uri3", "uri4", "uri5"};

      result = issue(N(alice), "5 TKN@alice", vector<id_type>{0, 1, 2, 3, 4}, uris, "nft1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto stats = get_stats(_ysymbol.tsymbol.value(), N(alice));
      BOOST_REQUIRE_EQUAL("5 TKN", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", stats["supply"]["issuer"].get_string());

      for (auto i = 0; i < 5; i++) {
         auto tokenval = get_token(static_cast<id_type>(i), N(alice));
         BOOST_REQUIRE_EQUAL(i, tokenval["id"]);
         BOOST_REQUIRE_EQUAL(uris[i], tokenval["uri"]);
         BOOST_REQUIRE_EQUAL("alice", tokenval["owner"]);
         BOOST_REQUIRE_EQUAL("nft1", tokenval["name"]);
         BOOST_REQUIRE_EQUAL("1 TKN", tokenval["value"]["amount"].get_string());
         BOOST_REQUIRE_EQUAL("alice", tokenval["value"]["issuer"].get_string());
      }

      /*
      auto alice_balance = get_accounts(N(alice), "0,TKN");
      REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
         ("balance", "5 TKN")
      );
      */

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(issue_tests, nft_tester) try {

      yx_symbol _ysymbol = yx_symbol::from_string("0,TKN@alice");

      auto result = create("0,TKN@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri"};

      result = issue(N(bob), "1 TKN@alice", vector<id_type>{0}, uris, "nft1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto stats = get_stats(_ysymbol.tsymbol.value(), N(alice));
      BOOST_REQUIRE_EQUAL("1 TKN", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", stats["supply"]["issuer"].get_string());

      auto tokenval = get_token(0, N(alice));
      BOOST_REQUIRE_EQUAL(0, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("uri", tokenval["uri"]);
      BOOST_REQUIRE_EQUAL("bob", tokenval["owner"]);
      BOOST_REQUIRE_EQUAL("nft1", tokenval["name"]);
      BOOST_REQUIRE_EQUAL("1 TKN", tokenval["value"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", tokenval["value"]["issuer"].get_string());

      /*
      auto bob_balance = get_account(N(bob), "0,TKN");
      REQUIRE_MATCHING_OBJECT( bob_balance, mvo()
         ("balance", "1 TKN")
      );
      */

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("to account does not exist"),
                          issue(N(dummy), "1 TKN@alice", vector<id_type>{1}, uris, "nft1", "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("the precision of non-fungible token must be 0"),
                          issue(N(alice), "1.05 TKN@alice", vector<id_type>{1}, uris, "nft1", "my memo")
      );

      string memo;
      for (auto i = 0; i < 100; i++) {
         memo += "hola";
      }

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("memo has more than 256 bytes"),
                          issue(N(alice), "1 TKN@alice", vector<id_type>{1}, uris, "nft1", memo)
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token is not yet created"),
                          issue(N(alice), "1 TTT@alice", vector<id_type>{1}, uris, "nft1", "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("must be positive token"),
                          issue(N(alice), "-1 TKN@alice", vector<id_type>{1}, uris, "nft1", "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("mismatch between token amount and the number of ids provided"),
                          issue(N(alice), "2 TKN@alice", vector<id_type>{1}, uris, "nft1", "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("mismatch between token amount and the number of uris provided"),
                          issue(N(alice), "2 TKN@alice", vector<id_type>{1, 2}, uris, "nft1", "my memo")
      );

      uris.push_back("uri2");

      BOOST_REQUIRE_EQUAL(success(),
                          issue(N(alice), "2 TKN@alice", vector<id_type>{1, 2}, uris, "nft1", "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token with specified ID already exists"),
                          issue(N(alice), "2 TKN@alice", vector<id_type>{1, 2}, uris, "nft1", "my memo")
      );

      vector<string> error_uris{uris.begin(), uris.end()};
      error_uris.push_back("12345678901234567890123456789012345678901234567890123456789012345");
      BOOST_REQUIRE_EQUAL(wasm_assert_msg("uri has more than 64 bytes"),
                          issue(N(alice), "3 TKN@alice", vector<id_type>{3, 4, 5}, error_uris, "nft1", "my memo")
      );

      string name;
      for (auto i = 0; i < 10; i++) {
         name += "nft1";
      }

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("name has more than 32 bytes"),
                          issue(N(alice), "2 TKN@alice", vector<id_type>{3, 4}, uris, name, "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("name is empty"),
                          issue(N(alice), "2 TKN@alice", vector<id_type>{3, 4}, uris, "", "my memo")
      );

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(transfer_tests, nft_tester) try {
      auto result = create("0,NFT@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri"};

      result = issue(N(alice), "1 NFT@alice", vector<id_type>{0}, uris, "nft1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto tokenval = get_token(0, N(alice));
      BOOST_REQUIRE_EQUAL(0, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("uri", tokenval["uri"]);
      BOOST_REQUIRE_EQUAL("alice", tokenval["owner"]);
      BOOST_REQUIRE_EQUAL("nft1", tokenval["name"]);
      BOOST_REQUIRE_EQUAL("1 NFT", tokenval["value"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", tokenval["value"]["issuer"].get_string());

      result = transfer(N(alice), N(bob), "1 NFT@alice", "send a token to bob");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      tokenval = get_token(0, N(alice));
      BOOST_REQUIRE_EQUAL(0, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("bob", tokenval["owner"]);

      result = transfer(N(alice), N(bob), "1 NFT@alice", "error transfer");
      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token is not found or is not owned by from account"), result);

      result = transfer(N(alice), N(bob), "1.00 NFT@alice", "error transfer");
      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token amount must be 1 with precision 0"), result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(transferid_tests, nft_tester) try {
      auto result = create("0,NFT@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri", "uri2", "uri3"};

      result = issue(N(alice), "3 NFT@alice", vector<id_type>{0, 1, 2}, uris, "nft1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<id_type> ids = {0, 1};
      result = transferid(N(alice), N(bob), N(alice), ids, "send token 0 and 1 to bob");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      /*
      auto alice_balance = get_account(N(alice), "0,NFT");
      REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
         ("balance", "1 NFT")
      );

      auto bob_balance = get_account(N(bob), "0,NFT");
      REQUIRE_MATCHING_OBJECT( bob_balance, mvo()
         ("balance", "2 NFT")
      );
      */

      auto tokenval = get_token(1, N(alice));
      BOOST_REQUIRE_EQUAL(1, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("uri2", tokenval["uri"]);
      BOOST_REQUIRE_EQUAL("bob", tokenval["owner"]);
      BOOST_REQUIRE_EQUAL("nft1", tokenval["name"]);
      BOOST_REQUIRE_EQUAL("1 NFT", tokenval["value"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", tokenval["value"]["issuer"].get_string());

      result = transferid(N(bob), N(carol), N(alice), vector<id_type>{1}, "send token 1 to carol");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      /*
      auto bob_balance1 = get_account(N(bob), "0,NFT");
      REQUIRE_MATCHING_OBJECT( bob_balance1, mvo()
           ("balance", "1 NFT")
      );

      auto carol_balance = get_account(N(carol), "0,NFT");
      REQUIRE_MATCHING_OBJECT( carol_balance, mvo()
           ("balance", "1 NFT")
      );
      */

      auto tokenval_1 = get_token(1, N(alice));
      BOOST_REQUIRE_EQUAL(1, tokenval_1["id"]);
      BOOST_REQUIRE_EQUAL("uri2", tokenval_1["uri"]);
      BOOST_REQUIRE_EQUAL("carol", tokenval_1["owner"]);
      BOOST_REQUIRE_EQUAL("nft1", tokenval_1["name"]);
      BOOST_REQUIRE_EQUAL("1 NFT", tokenval_1["value"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", tokenval_1["value"]["issuer"].get_string());

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("from account does not own token with specified ID"),
                          transferid(N(alice), N(carol), N(alice), vector<id_type>{0}, "send from non-owner")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("from and to account cannot be the same"),
                          transferid(N(alice), N(alice), N(alice), vector<id_type>{1}, "send to the issuer")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("to account does not exist"),
                          transferid(N(alice), N(dummy), N(alice), vector<id_type>{1}, "send to non-existing")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token with specified ID does not exist"),
                          transferid(N(alice), N(bob), N(alice), vector<id_type>{3}, "no token id")
      );

      string memo;
      for (auto i = 0; i < 100; i++) {
         memo += "longmemo";
      }

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("memo has more than 256 bytes"),
                          transferid(N(alice), N(bob), N(alice), vector<id_type>{1}, memo)
      );
   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(redeem_tests, nft_tester) try {

      yx_symbol _ysymbol = yx_symbol::from_string("0,NFT@alice");

      auto result = create("0,NFT@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri1", "uri2"};

      result = issue(N(alice), "2 NFT@alice", vector<id_type>{0, 1}, uris, "nft1", "issue 2 tokens");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = redeem(N(alice), vector<id_type>{1}, "redeem token 1");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto stats = get_stats(_ysymbol.tsymbol.value(), N(alice));
      BOOST_REQUIRE_EQUAL("1 NFT", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", stats["supply"]["issuer"].get_string());

      /*
      auto alice_balance = get_account(N(alice), "0,NFT");
      REQUIRE_MATCHING_OBJECT(alice_balance, mvo()
            ("balance", "1 NFT")
      );
      */

      result = transferid(N(alice), N(bob), N(alice), vector<id_type>{0}, "send token 0 to bob");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token not owned by issuer"),
                          redeem(N(alice), vector<id_type>{0}, "redeem token")
      );

      result = transferid(N(bob), N(alice), N(alice), vector<id_type>{0}, "send token 0 to bob");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      string memo;
      for (auto i = 0; i < 100; i++) {
         memo += "longmemo";
      }

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("memo has more than 256 bytes"),
                          redeem(N(alice), vector<id_type>{0}, memo)
      );

      result = redeem(N(alice), vector<id_type>{0}, "redeem token 0");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto stats2 = get_stats(_ysymbol.tsymbol.value(), N(alice));
      BOOST_REQUIRE_EQUAL("0 NFT", stats2["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", stats2["supply"]["issuer"].get_string());

   } FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
