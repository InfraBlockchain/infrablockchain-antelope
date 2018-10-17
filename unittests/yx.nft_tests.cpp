#include <yosemite/testing/yx_tester.hpp>

using namespace yosemite::testing;

class nft_tester : public yx_tester {
public:
   nft_tester() {
      init_yosemite_contracts();

      create_accounts({N(alice), N(bob), N(carol)});
      produce_blocks();
   }
};

BOOST_AUTO_TEST_SUITE(yx_nft_tests)

   BOOST_FIXTURE_TEST_CASE(create_tests, nft_tester) try {
      yx_symbol _ysymbol = yx_symbol::from_string("0,NFT@alice");

      auto result = nft_create("0,NFT@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto stats = nft_get_stats(_ysymbol.tsymbol.value(), N(alice));
      BOOST_REQUIRE_EQUAL("0 NFT", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", stats["supply"]["issuer"].get_string());

      BOOST_REQUIRE_EQUAL("action's authorizing actor 'dummy' does not exist",
                          nft_create("0,TKN@dummy", 0)
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("already created"),
                          nft_create("0,NFT@alice", 0)
      );

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE (issue_multi_tests, nft_tester) try {

      yx_symbol _ysymbol = yx_symbol::from_string("0,TKN@alice");

      auto result = nft_create("0,TKN@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri1", "uri2", "uri3", "uri4", "uri5"};

      result = nft_issue(N(alice), "5 TKN@alice", vector<id_type>{0, 1, 2, 3, 4}, uris, "nft1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto stats = nft_get_stats(_ysymbol.tsymbol.value(), N(alice));
      BOOST_REQUIRE_EQUAL("5 TKN", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", stats["supply"]["issuer"].get_string());

      for (auto i = 0; i < 5; i++) {
         auto tokenval = nft_get_token(static_cast<id_type>(i), N(alice));
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

      auto result = nft_create("0,TKN@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri"};

      result = nft_issue(N(bob), "1 TKN@alice", vector<id_type>{0}, uris, "nft1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto stats = nft_get_stats(_ysymbol.tsymbol.value(), N(alice));
      BOOST_REQUIRE_EQUAL("1 TKN", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", stats["supply"]["issuer"].get_string());

      auto tokenval = nft_get_token(0, N(alice));
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
                          nft_issue(N(dummy), "1 TKN@alice", vector<id_type>{1}, uris, "nft1", "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("the precision of non-fungible token must be 0"),
                          nft_issue(N(alice), "1.05 TKN@alice", vector<id_type>{1}, uris, "nft1", "my memo")
      );

      string memo;
      for (auto i = 0; i < 100; i++) {
         memo += "hola";
      }

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("memo has more than 256 bytes"),
                          nft_issue(N(alice), "1 TKN@alice", vector<id_type>{1}, uris, "nft1", memo)
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token is not yet created"),
                          nft_issue(N(alice), "1 TTT@alice", vector<id_type>{1}, uris, "nft1", "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("must be positive token"),
                          nft_issue(N(alice), "-1 TKN@alice", vector<id_type>{1}, uris, "nft1", "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("mismatch between token amount and the number of ids provided"),
                          nft_issue(N(alice), "2 TKN@alice", vector<id_type>{1}, uris, "nft1", "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("mismatch between token amount and the number of uris provided"),
                          nft_issue(N(alice), "2 TKN@alice", vector<id_type>{1, 2}, uris, "nft1", "my memo")
      );

      uris.push_back("uri2");

      BOOST_REQUIRE_EQUAL(success(),
                          nft_issue(N(alice), "2 TKN@alice", vector<id_type>{1, 2}, uris, "nft1", "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token with specified ID already exists"),
                          nft_issue(N(alice), "2 TKN@alice", vector<id_type>{1, 2}, uris, "nft1", "my memo")
      );

      string big_uri;
      for (auto i = 0; i < 26; i++) {
         big_uri += "1234567890";
      }

      vector<string> error_uris{uris.begin(), uris.end()};
      error_uris.push_back(big_uri);
      BOOST_REQUIRE_EQUAL(wasm_assert_msg("uri has more than 256 bytes"),
                          nft_issue(N(alice), "3 TKN@alice", vector<id_type>{3, 4, 5}, error_uris, "nft1", "my memo")
      );

      string name;
      for (auto i = 0; i < 10; i++) {
         name += "nft1";
      }

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("name has more than 32 bytes"),
                          nft_issue(N(alice), "2 TKN@alice", vector<id_type>{3, 4}, uris, name, "my memo")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("name is empty"),
                          nft_issue(N(alice), "2 TKN@alice", vector<id_type>{3, 4}, uris, "", "my memo")
      );

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(transfer_tests, nft_tester) try {
      auto result = nft_create("0,NFT@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri"};

      result = nft_issue(N(alice), "1 NFT@alice", vector<id_type>{0}, uris, "nft1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto tokenval = nft_get_token(0, N(alice));
      BOOST_REQUIRE_EQUAL(0, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("uri", tokenval["uri"]);
      BOOST_REQUIRE_EQUAL("alice", tokenval["owner"]);
      BOOST_REQUIRE_EQUAL("nft1", tokenval["name"]);
      BOOST_REQUIRE_EQUAL("1 NFT", tokenval["value"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", tokenval["value"]["issuer"].get_string());

      result = nft_transfer(N(alice), N(bob), "1 NFT@alice", "send a token to bob");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      tokenval = nft_get_token(0, N(alice));
      BOOST_REQUIRE_EQUAL(0, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("bob", tokenval["owner"]);

      result = nft_transfer(N(alice), N(bob), "1 NFT@alice", "error nft_transfer");
      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token is not found or is not owned by from account"), result);

      result = nft_transfer(N(alice), N(bob), "1.00 NFT@alice", "error nft_transfer");
      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token amount must be 1 with precision 0"), result);

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(transferid_tests, nft_tester) try {
      auto result = nft_create("0,NFT@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri", "uri2", "uri3"};

      result = nft_issue(N(alice), "3 NFT@alice", vector<id_type>{0, 1, 2}, uris, "nft1", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<id_type> ids = {0, 1};
      result = nft_transferid(N(alice), N(bob), N(alice), ids, "send token 0 and 1 to bob");
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

      auto tokenval = nft_get_token(1, N(alice));
      BOOST_REQUIRE_EQUAL(1, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("uri2", tokenval["uri"]);
      BOOST_REQUIRE_EQUAL("bob", tokenval["owner"]);
      BOOST_REQUIRE_EQUAL("nft1", tokenval["name"]);
      BOOST_REQUIRE_EQUAL("1 NFT", tokenval["value"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", tokenval["value"]["issuer"].get_string());

      result = nft_transferid(N(bob), N(carol), N(alice), vector<id_type>{1}, "send token 1 to carol");
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

      auto tokenval_1 = nft_get_token(1, N(alice));
      BOOST_REQUIRE_EQUAL(1, tokenval_1["id"]);
      BOOST_REQUIRE_EQUAL("uri2", tokenval_1["uri"]);
      BOOST_REQUIRE_EQUAL("carol", tokenval_1["owner"]);
      BOOST_REQUIRE_EQUAL("nft1", tokenval_1["name"]);
      BOOST_REQUIRE_EQUAL("1 NFT", tokenval_1["value"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", tokenval_1["value"]["issuer"].get_string());

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("from account does not own token with specified ID"),
                          nft_transferid(N(alice), N(carol), N(alice), vector<id_type>{0}, "send from non-owner")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("from and to account cannot be the same"),
                          nft_transferid(N(alice), N(alice), N(alice), vector<id_type>{1}, "send to the issuer")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("to account does not exist"),
                          nft_transferid(N(alice), N(dummy), N(alice), vector<id_type>{1}, "send to non-existing")
      );

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token with specified ID does not exist"),
                          nft_transferid(N(alice), N(bob), N(alice), vector<id_type>{3}, "no token id")
      );

      string memo;
      for (auto i = 0; i < 100; i++) {
         memo += "longmemo";
      }

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("memo has more than 256 bytes"),
                          nft_transferid(N(alice), N(bob), N(alice), vector<id_type>{1}, memo)
      );
   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(redeem_tests, nft_tester) try {

      yx_symbol _ysymbol = yx_symbol::from_string("0,NFT@alice");

      auto result = nft_create("0,NFT@alice", 0);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri1", "uri2"};

      result = nft_issue(N(alice), "2 NFT@alice", vector<id_type>{0, 1}, uris, "nft1", "nft_issue 2 tokens");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = nft_redeem(N(alice), vector<id_type>{1}, "nft_redeem token 1");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto stats = nft_get_stats(_ysymbol.tsymbol.value(), N(alice));
      BOOST_REQUIRE_EQUAL("1 NFT", stats["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", stats["supply"]["issuer"].get_string());

      /*
      auto alice_balance = get_account(N(alice), "0,NFT");
      REQUIRE_MATCHING_OBJECT(alice_balance, mvo()
            ("balance", "1 NFT")
      );
      */

      result = nft_transferid(N(alice), N(bob), N(alice), vector<id_type>{0}, "send token 0 to bob");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("token not owned by issuer"),
                          nft_redeem(N(alice), vector<id_type>{0}, "nft_redeem token")
      );

      result = nft_transferid(N(bob), N(alice), N(alice), vector<id_type>{0}, "send token 0 to bob");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      string memo;
      for (auto i = 0; i < 100; i++) {
         memo += "longmemo";
      }

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("memo has more than 256 bytes"),
                          nft_redeem(N(alice), vector<id_type>{0}, memo)
      );

      result = nft_redeem(N(alice), vector<id_type>{0}, "nft_redeem token 0");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      auto stats2 = nft_get_stats(_ysymbol.tsymbol.value(), N(alice));
      BOOST_REQUIRE_EQUAL("0 NFT", stats2["supply"]["amount"].get_string());
      BOOST_REQUIRE_EQUAL("alice", stats2["supply"]["issuer"].get_string());

   } FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
