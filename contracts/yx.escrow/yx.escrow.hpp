/**
 * @file
 * @copyright defined in yosemite-public-blockchain/LICENSE
 */
#pragma once

#include <yosemitelib/native_token.hpp>
#include <yosemitelib/token.hpp>

namespace yosemite {
   using std::string;
   using eosio::asset;

   class token_escrow_contract : public eosio::contract {
   public:
      explicit token_escrow_contract(account_name self) : contract(self) {
      }

      void escrow(account_name thirdparty, uint64_t id, account_name from, account_name to, const yx_asset &token, const string &memo);
      void escrownt(account_name thirdparty, uint64_t id, account_name from, account_name to, const eosio::asset &amount,
                    const string &memo);
      void proceed(account_name thirdparty, uint64_t id);
      void cancel(account_name thirdparty, uint64_t id);

   private:
      void transfer_token_as_inline(account_name from, account_name to, const yx_asset &token, const string &memo);

      /* scope = thirdparty */
      struct escrow_info {
         uint64_t id = 0;
         yx_asset token{};
         account_name from{};
         account_name to{};
         string memo{};

         uint64_t primary_key() const { return id; }
      };

      using escrow_info_index = eosio::multi_index<N(escrows), escrow_info>;
   };
}
