/**
 *  @file chain/yosemite/standard_token_action_types.cpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */

#include <eosio/chain/types.hpp>
#include <yosemite/chain/standard_token_action_types.hpp>

namespace yosemite { namespace chain { namespace token {

   using namespace eosio::chain;

   bool is_yosemite_standard_token_action(action_name action) {
      return action == settokenmeta::get_name() || action == issue::get_name();
   }

} } } // namespace yosemite::chain::token
