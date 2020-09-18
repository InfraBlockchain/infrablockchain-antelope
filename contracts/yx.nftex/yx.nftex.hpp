/**
 * @file
 * @copyright defined in yosemite-public-blockchain/LICENSE
 */
#include <string>
#include <eosiolib/time.hpp>
#include <infrablockchainlib/yx_contract.hpp>

namespace yosemite {

   using namespace eosio;
   using id_type = uint64_t;
   using std::string;

   struct yx_nft {
      yx_symbol ysymbol;
      id_type id;

      uint128_t get_orderkey() const {
         // issuer | nft_id
         uint128_t result(ysymbol.issuer);
         result <<= 64;
         return result | id;
      }
   };

   class nft_exchange : public yx_contract {
      static const uint32_t MAX_EXPIRATION_SECONDS = 604800; // 1 week in seconds

      void check_param(const account_name &order_account, const yx_nft &nft,
                       const yx_asset &price, const string &memo,
                       const time_point_sec &expiration = time_point_sec{0}) const;

   public:
      explicit nft_exchange(account_name self) : yx_contract(self) {}

      void buy(account_name buyer, const yx_nft &nft, const yx_asset &price, const string &memo);
      void buynt(account_name buyer, const yx_nft &nft, const asset &price, const string &memo);
      void sell(account_name seller, const yx_nft &nft, const yx_asset &price, const time_point_sec &expiration, const string &memo);
      void sellnt(account_name seller, const yx_nft &nft, const asset &price, const time_point_sec &expiration, const string &memo);
      void cancelsell(const yx_nft &nft);
   };

   /* scope = token symbol */
   struct sell_order {
      uint64_t id = 0; // internal managed id
      uint64_t nft_id = 0; // NFT token id
      yx_symbol ysymbol; // NFT symbol and issuer
      account_name seller{};
      yx_asset price;
      time_point_sec expiration{};

      uint64_t primary_key() const { return id; }
      uint128_t by_orderkey() const {
         // issuer | nft_id
         uint128_t result(ysymbol.issuer);
         result <<= 64;
         return result | nft_id;
      }
      uint64_t by_owner() const { return seller; }
   };

   using sellbook = multi_index<N(sellbook), sell_order,
         indexed_by<N(orderkey), const_mem_fun<sell_order, uint128_t, &sell_order::by_orderkey> >,
         indexed_by<N(byowner), const_mem_fun<sell_order, uint64_t, &sell_order::by_owner> > >;
}
