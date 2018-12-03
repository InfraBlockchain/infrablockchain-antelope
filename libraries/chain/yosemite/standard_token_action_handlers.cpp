/**
 *  @file chain/yosemite/standard_token_action_handlers.cpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */

#include <yosemite/chain/standard_token_action_handlers.hpp>
#include <yosemite/chain/standard_token_action_types.hpp>
#include <yosemite/chain/standard_token_database.hpp>
#include <yosemite/chain/exceptions.hpp>

#include <eosio/chain/controller.hpp>
#include <eosio/chain/transaction_context.hpp>
#include <eosio/chain/apply_context.hpp>
#include <eosio/chain/transaction.hpp>

namespace yosemite { namespace chain {

   using namespace eosio::chain;

   void apply_yosemite_built_in_action_settokenmeta(apply_context& context) {
      auto action = context.act.data_as_built_in_common_action<settokenmeta>();
      try {
         int64_t url_size = action.url.size();
         int64_t desc_size = action.description.size();

         EOS_ASSERT(action.symbol.valid(), token_meta_validate_exception, "Invalid token symbol");
         EOS_ASSERT(url_size > 0 && url_size <= 255, token_meta_validate_exception, "Invalid token url size");
         EOS_ASSERT(desc_size > 0 && desc_size <= 255, token_meta_validate_exception, "Invalid token description size");

         // only the account owner can set token metadata for the own token
         context.require_authorization(context.receiver);

         auto& db = context.db;

         auto set_token_meta_lambda = [&action,url_size,desc_size](token_meta_object& token_meta) {
            token_meta.symbol = action.symbol;
            token_meta.url.resize(url_size);
            memcpy( token_meta.url.data(), action.url.data(), url_size );
            token_meta.description.resize(desc_size);
            memcpy( token_meta.description.data(), action.description.data(), desc_size );
         };

         auto token_meta_ptr = db.find<token_meta_object, by_token_id>(context.receiver);
         if ( token_meta_ptr ) {
            EOS_ASSERT(token_meta_ptr->symbol != action.symbol || token_meta_ptr->url != action.url.c_str() || token_meta_ptr->description != action.description.c_str(),
               token_meta_validate_exception, "Attempting update token metadata, but new metadata is same as old one");

            db.modify(*token_meta_ptr, set_token_meta_lambda);
         } else {
            db.create<token_meta_object>(set_token_meta_lambda);
            context.add_ram_usage(context.receiver, (int64_t)(config::billable_size_v<token_meta_object>));
         }

      } FC_CAPTURE_AND_RETHROW( (action) )
   }

} } // namespace yosemite::chain
