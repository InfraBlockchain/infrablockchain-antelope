/**
 *  @file
 *  @copyright defined in yosemite/LICENSE
 */
#pragma once

#include <yosemitelib/token.hpp>
#include <string>

namespace yosemite { namespace non_native_token {

   using namespace eosio;
   using std::string;

   class yx_token : public token {
   public:
      explicit yx_token(account_name self) : token(self) {
      }

      void issue(const account_name &to, const yx_asset &token, const string &memo);
      void redeem(const yx_asset &token, const string &memo);
      void transfer(account_name from, account_name to, const yx_asset &token, const string &memo) override;

   protected:
      void inner_check_create_parameters(const yx_symbol &ysymbol, uint16_t can_set_options) override;
   };

}}
