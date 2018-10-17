/**
 * @file
 * @copyright defined in yosemite-public-blockchain/LICENSE
 */
#include <string>
#include <eosiolib/time.hpp>
#include <yosemitelib/yx_contract.hpp>

namespace yosemite {

   using namespace eosio;
   using id_type = uint64_t;
   using std::string;

   struct yx_nft {
      yx_symbol ysymbol;
      id_type id;
   };

   class nft_exchange : public yx_contract {
      static const uint32_t MURMUR3HASH_SEED = 4234316459;
      static const uint32_t MAX_EXPIRATION_SECONDS = 604800; // 1 week in seconds

      uint64_t make_scope(const yx_symbol &nft_symbol) const;

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

   struct sell_order {
      uint64_t id = 0; // id of NFT
      yx_symbol ysymbol; // symbol of NFT
      account_name seller{};
      yx_asset price;
      time_point_sec expiration{};

      uint64_t primary_key() const { return id; }
      uint64_t by_owner() const { return seller; }
   };

   using sellbook = multi_index<N(sellbook), sell_order,
         indexed_by<N(byowner), const_mem_fun<sell_order, uint64_t, &sell_order::by_owner> > >;
}
