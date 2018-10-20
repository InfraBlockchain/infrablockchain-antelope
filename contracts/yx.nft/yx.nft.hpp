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
   using id_type = uint64_t;
   using uri_type = string;

   class nft : public token {
   public:
      explicit nft(account_name self) : token(self) {}

      void issue(account_name to, const yx_symbol &ysymbol, const vector<id_type> &ids, const vector<string> &uris, const string &name,
                 const string &memo);
      void redeem(account_name issuer, const vector <id_type> &ids, const string &memo);
      void transferid(account_name from, account_name to, account_name issuer, const vector<id_type> &ids, const string &memo);
      void transfer(account_name from, account_name to, const yx_asset &token, const string &memo) override;

      /* scope = issuer */
      // @abi table tokens i64
      struct nftoken {
         id_type id;          // Unique 64 bit identifier
         uri_type uri;        // RFC 3986
         account_name owner;  // token owner
         yx_asset value;      // token value (1 XXX)
         string name;         // token name

         id_type primary_key() const { return id; }
         account_name get_owner() const { return owner; }
         uint64_t get_symbol() const { return value.symbol.value; }
      };

      using token_index = eosio::multi_index<N(nftokens), nftoken,
            indexed_by<N(byowner), const_mem_fun<nftoken, account_name, &nftoken::get_owner> >,
            indexed_by<N(bysymbol), const_mem_fun<nftoken, uint64_t, &nftoken::get_symbol> > >;

   protected:
      void inner_check_create_parameters(const yx_symbol &ysymbol, uint16_t can_set_options) override;

   };

}}
