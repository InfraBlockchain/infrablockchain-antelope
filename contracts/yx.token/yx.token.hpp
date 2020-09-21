/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#pragma once

#include <musl/upstream/include/bits/stdint.h>
#include <infrablockchainlib/token.hpp>
#include <infrablockchainlib/number.hpp>
#include <string>

// DEPRECATED

namespace yosemite { namespace non_native_token {

   using namespace eosio;
   using std::string;

   /* scope = token symbol */
   struct delegated_issue {
      uint64_t id = 0;
      account_name account;
      yx_asset limit{};
      yx_asset issued{}; // cannot exceed limit
      account_name entrusted_issuer{};

      uint64_t primary_key() const { return id; }
      uint128_t by_secondary_key() const {
         // issuer | account
         return to_uint128(limit.get_yx_symbol().issuer, account);
      }
   };

   // DEPRECATED
   class yx_token : public token {
   public:
      explicit yx_token(account_name self) : token(self) {
      }

      void issue(const account_name &to, const yx_asset &token, const string &memo);
      void redeem(const yx_asset &token, const string &memo);
      void transfer(account_name from, account_name to, const yx_asset &token, const string &memo) override;
      void setuilimit(const account_name &to, const yx_asset &limit);
      /** Token is issued by the user till issue limit. */
      void issuebyuser(const account_name &user, const account_name &to, const yx_asset &token, const string &memo);
      /** The user entrusts its issue authority to another account. Currently, another account is only the token issuer. */
      void entrustui(const account_name &user, const account_name &to, const yx_symbol &ysymbol);
      void changeissued(const account_name &user, const yx_asset &delta, bool decrease);

   protected:
      void inner_check_create_parameters(const yx_symbol &ysymbol, uint16_t can_set_options) override;

   private:
      void increase_supply(const yx_asset &token) const;
   };

   using delegated_issue_table = eosio::multi_index<N(delissue), delegated_issue,
         indexed_by<N(secondary), const_mem_fun<delegated_issue, uint128_t, &delegated_issue::by_secondary_key> >
   >;

}}
