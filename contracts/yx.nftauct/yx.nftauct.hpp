/**
 *  @copyright defined in LICENSE
 */

#pragma once

#include <eosiolib/time.hpp>
#include <string>
#include <yosemitelib/yx_asset.hpp>
#include <yosemitelib/yx_contract.hpp>

namespace yosemite {

   using namespace eosio;
   using std::string;
   using id_type = uint64_t;

   /* auction item id used as abi input parameter */
   struct yx_nft {
      yx_symbol ysymbol{};
      id_type id = 0;

      uint128_t get_orderkey() const {
         // issuer | nft_id
         uint128_t result{ysymbol.issuer};
         result <<= 64;
         return result | id;
      }
   };

   /* scope = NFT issuer */
   struct auction_item_info {
      uint64_t id = 0; // NFT id per issuer
      account_name nft_owner = 0;
      yx_asset start_price{};
      asset end_price{};
      asset min_inc_price{};
      time_point_sec expiration{};
      string iteminfo{};
      uint8_t options = 0;
      account_name last_bidder{};
      asset last_bid_price{};
      time_point_sec last_bid_time{};

      uint64_t primary_key() const { return id; }
      account_name by_owner() const { return nft_owner; }
      account_name get_price_token_issuer() const { return start_price.issuer; }
   };

   class nft_auction : public yx_contract {
   public:
      explicit nft_auction(account_name self) : yx_contract(self) {
      }

      void create(const yx_nft &nft, account_name nft_owner, const yx_asset &start_price,
                  const asset &end_price, const asset &min_inc_price,
                  const string &iteminfo, const time_point_sec &expiration, uint8_t options);

      void bid(const yx_nft &nft, account_name bidder, const yx_asset &price);
      void complete(const yx_nft &nft);
      void remove(const yx_nft &nft);
   private:
      void do_complete(const yx_nft &nft, const auction_item_info &info, account_name last_bidder, account_name from);
   };

   using auction_items_index = eosio::multi_index<N(nftauctitems), auction_item_info,
         indexed_by<N(byowner), const_mem_fun<auction_item_info, uint64_t, &auction_item_info::by_owner> > >;
}
