#include <yosemite/testing/yx_tester.hpp>
#include <fc/reflect/reflect.hpp>
#include "fnv1a.hpp"

using namespace yosemite::chain;
using namespace yosemite::testing;

typedef uint64_t id_type;

struct yx_nft {
   yx_symbol ysymbol;
   id_type id;
};

FC_REFLECT(yx_nft, (ysymbol)(id))

class nftex_tester : public yx_tester {

public:

   nftex_tester() {
      init_yosemite_contracts();

      create_accounts({N(alice), N(bob), N(carol), N(gameprovider)});
      produce_blocks();
   }

   uint64_t make_scope(const yx_symbol &nft_symbol) const {
      string sym = nft_symbol.to_string();
      return static_cast<uint64_t>(hash_32_fnv1a(sym.c_str(), static_cast<int>(sym.size())));
   }

   fc::variant get_sell_order(const string &nft_symbol, uint64_t id) {
      uint64_t scope = make_scope(yx_symbol::from_string(nft_symbol));
      vector<char> data = get_row_by_account(YOSEMITE_NON_FUNGIBLE_TOKEN_EXCHANGE_ACCOUNT, scope, N(sellbook), id);
      return data.empty() ? fc::variant() : abi_ser_nftex.binary_to_variant("sell_order", data, abi_serializer_max_time);
   }

   action_result
   buy(account_name buyer, const string &nft_symbol, id_type id, const string &price, const string &memo) {
      yx_nft nft{yx_symbol::from_string(nft_symbol), id};
      auto _price = yx_asset::from_string(price);
      return push_action(N(buy), mvo()
                               ("buyer", buyer)
                               ("nft", nft)
                               ("price", _price)
                               ("memo", memo),
                         buyer, abi_ser_nftex, YOSEMITE_NON_FUNGIBLE_TOKEN_EXCHANGE_ACCOUNT
      );
   }

   action_result
   sell(account_name seller, const string &nft_symbol, id_type id, const string &price, const string &expiration, const string &memo) {
      yx_nft nft{yx_symbol::from_string(nft_symbol), id};
      auto _price = yx_asset::from_string(price);
      return push_action(N(sell), mvo()
                               ("seller", seller)
                               ("nft", nft)
                               ("price", _price)
                               ("expiration", expiration)
                               ("memo", memo),
                         seller, abi_ser_nftex, YOSEMITE_NON_FUNGIBLE_TOKEN_EXCHANGE_ACCOUNT
      );
   }

   action_result
   cancelsell(account_name seller, const string &nft_symbol, id_type id) {
      yx_nft nft{yx_symbol::from_string(nft_symbol), id};
      return push_action(N(cancelsell), mvo()
                               ("nft", nft),
                         seller, abi_ser_nftex, YOSEMITE_NON_FUNGIBLE_TOKEN_EXCHANGE_ACCOUNT
      );
   }

   void test_setup() {
      auto result = nft_create("0,GIT@gameprovider", 0); // GIT = Game Item Token(NFT)
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      vector<string> uris = {"uri1", "uri2", "uri3", "uri4", "uri5"};

      result = nft_issue(N(alice), "0,GIT@gameprovider", vector<id_type>{0, 1, 2, 3, 4}, uris, "my game", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      uris = {"uri6", "uri7", "uri8", "uri9", "uri10"};

      result = nft_issue(N(bob), "0,GIT@gameprovider", vector<id_type>{5, 6, 7, 8, 9}, uris, "my game", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks();

      result = token_create("2,GMT@gameprovider", 0); // GMT = Game Money Token
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_issue(N(alice), "10000.00 GMT@gameprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      result = token_issue(N(bob), "10000.00 GMT@gameprovider", "my memo");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);
   }

};

BOOST_AUTO_TEST_SUITE(yx_nftex_tests)

   BOOST_FIXTURE_TEST_CASE(sell_tests, nftex_tester) try {
      test_setup();

      auto _now = now();
      auto expiration = time_point_sec{_now + 600};
      auto wrong_expiration = time_point_sec{_now - 600};
      auto too_long_expiration = time_point_sec{_now + 604800 + 10};

      auto result = sell(N(alice), "0,GIT@gameprovider", 3, "1000.00 GMT@gameprovider", expiration.to_iso_string(), "normal sell");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      auto tokenval = nft_get_token(static_cast<id_type>(3), N(gameprovider));
      BOOST_REQUIRE_EQUAL(3, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("yx.nftex", tokenval["owner"]);

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("sell order with the id already exists"),
                          sell(N(alice), "0,GIT@gameprovider", 3, "1000.00 GMT@gameprovider", expiration.to_iso_string(), ""));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("nft does not exist"),
                          sell(N(alice), "0,NONE@gameprovider", 1, "1000.00 GMT@gameprovider", expiration.to_iso_string(), ""));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("invalid issuer account"),
                          sell(N(alice), "0,GIT@xxxxxxxxxxxx", 1, "1000.00 GMT@gameprovider", expiration.to_iso_string(), ""));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("price token does not exist"),
                          sell(N(alice), "0,GIT@gameprovider", 1, "1000.00 NONE@gameprovider", expiration.to_iso_string(), ""));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("only positive price is allowed"),
                          sell(N(alice), "0,GIT@gameprovider", 1, "-1000.00 GMT@gameprovider", expiration.to_iso_string(), ""));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("memo has more than 256 bytes"),
                          sell(N(alice), "0,GIT@gameprovider", 1, "1000.00 GMT@gameprovider", expiration.to_iso_string(),
                               "x1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456"));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("wrong expiration : already expired or too close to now"),
                          sell(N(alice), "0,GIT@gameprovider", 1, "1000.00 GMT@gameprovider", wrong_expiration.to_iso_string(),
                               "wrong expiration"));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("wrong expiration : already expired or too close to now"),
                          sell(N(alice), "0,GIT@gameprovider", 1, "1000.00 GMT@gameprovider", time_point_sec{_now + 10}.to_iso_string(),
                               "wrong expiration"));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("expiration is too long"),
                          sell(N(alice), "0,GIT@gameprovider", 1, "1000.00 GMT@gameprovider", too_long_expiration.to_iso_string(),
                               "wrong expiration"));
   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(buy_tests, nftex_tester) try {
      test_setup();

      auto _now = now();
      auto expiration = time_point_sec{_now + 600};

      auto result = sell(N(alice), "0,GIT@gameprovider", 3, "1000.00 GMT@gameprovider", expiration.to_iso_string(), "normal sell");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("price is different"),
                          buy(N(bob), "0,GIT@gameprovider", 3, "1001.00 GMT@gameprovider", ""));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("NFT with the specified id is not in the sell order book"),
                          buy(N(bob), "0,GIT@gameprovider", 4, "1000.00 GMT@gameprovider", ""));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("seller and buyer are the same"),
                          buy(N(alice), "0,GIT@gameprovider", 3, "1000.00 GMT@gameprovider", ""));

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("account doesn't have token"),
                          buy(N(carol), "0,GIT@gameprovider", 3, "1000.00 GMT@gameprovider", ""));

      result = buy(N(bob), "0,GIT@gameprovider", 3, "1000.00 GMT@gameprovider", "normal buy");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      auto tokenval = nft_get_token(static_cast<id_type>(3), N(gameprovider));
      BOOST_REQUIRE_EQUAL(3, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("bob", tokenval["owner"]);

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("NFT with the specified id is not in the sell order book"),
                          buy(N(bob), "0,GIT@gameprovider", 3, "1000.00 GMT@gameprovider", ""));

   } FC_LOG_AND_RETHROW()

   BOOST_FIXTURE_TEST_CASE(cancel_tests, nftex_tester) try {
      test_setup();

      auto _now = now();
      auto expiration = time_point_sec{_now + 600};

      auto result = sell(N(alice), "0,GIT@gameprovider", 3, "1000.00 GMT@gameprovider", expiration.to_iso_string(), "normal sell");
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      auto tokenval = nft_get_token(static_cast<id_type>(3), N(gameprovider));
      BOOST_REQUIRE_EQUAL(3, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("yx.nftex", tokenval["owner"]);

      result = cancelsell(N(alice), "0,GIT@gameprovider", 3);
      BOOST_REQUIRE_EQUAL("", result);
      produce_blocks(1);

      tokenval = nft_get_token(static_cast<id_type>(3), N(gameprovider));
      BOOST_REQUIRE_EQUAL(3, tokenval["id"]);
      BOOST_REQUIRE_EQUAL("alice", tokenval["owner"]);

      BOOST_REQUIRE_EQUAL(wasm_assert_msg("NFT with the specified id is not in the sell order book"),
                          buy(N(bob), "0,GIT@gameprovider", 3, "1000.00 GMT@gameprovider", ""));

   } FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
