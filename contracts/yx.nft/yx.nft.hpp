/**
 *  @file
 *  @copyright defined in yosemite-public-blockchain/LICENSE
 */
#pragma once

#include <yosemitelib/token.hpp>
#include <string>

namespace yosemite { namespace non_native_token {
   using namespace eosio;
   using std::string;
   using uuid = uint128_t;
   using id_type = uint64_t;
   using uri_type = string;

   class nft : public token {
   public:
      explicit nft(account_name self) : token(self), tokens(self, self) {}

      void issue(account_name to, const yx_asset &token, const vector<string> &uris, const string &name,
                 const string &memo);
      void redeem(account_name owner, id_type token_id);
      void transferid(account_name from, account_name to, id_type id, const string &memo);
      void transfer(account_name from, account_name to, const yx_asset &token, const string &memo) override;

      // @abi table tokens i64
      struct nftoken {
         id_type id;          // Unique 64 bit identifier,
         uri_type uri;        // RFC 3986
         account_name owner;  // token owner
         yx_asset value;      // token value (1 XXX)
         string name;         // token name

         id_type primary_key() const { return id; }
         account_name get_owner() const { return owner; }
         string get_uri() const { return uri; }
         asset get_value() const { return value; }
         uint64_t get_symbol() const { return value.symbol.name(); }
         uint64_t get_name() const { return string_to_name(name.c_str()); }

         uuid get_global_id() const {
            uint128_t self_128 = static_cast<uint128_t>(N(_self));
            uint128_t id_128 = static_cast<uint128_t>(id);
            uint128_t res = (self_128 << 64) | (id_128);
            return res;
         }

         string get_unique_name() const {
            string unique_name = name + "#" + std::to_string(id);
            return unique_name;
         }
      };

      using token_index = eosio::multi_index<N(nftokens), nftoken,
            indexed_by<N(byowner), const_mem_fun<nftoken, account_name, &nftoken::get_owner> >,
            indexed_by<N(bysymbol), const_mem_fun<nftoken, uint64_t, &nftoken::get_symbol> >,
            indexed_by<N(byname), const_mem_fun<nftoken, uint64_t, &nftoken::get_name> > >;

   protected:
      void inner_check_create_parameters(const yx_symbol &ysymbol, uint16_t can_set_options) override;

   private:
      token_index tokens;

   };

}}
