/**
 *  @file chain/infrablockchain/standard_token_manager.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <infrablockchain/chain/standard_token_manager.hpp>
#include <infrablockchain/chain/infrablockchain_global_property_database.hpp>
#include <infrablockchain/chain/standard_token_action_types.hpp>
#include <infrablockchain/chain/exceptions.hpp>
#include <infrablockchain/chain/native_token_symbol.hpp>

#include <eosio/chain/config.hpp>
#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/database_utils.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   using standard_token_db_index_set = index_set<
      token_info_multi_index,
      token_balance_multi_index
   >;

   standard_token_manager::standard_token_manager( chainbase::database& db )
      : _db(db) {
   }

   void standard_token_manager::add_indices() {
      standard_token_db_index_set::add_indices(_db);
   }

   void standard_token_manager::initialize_database() {

   }

   void standard_token_manager::add_to_snapshot( const snapshot_writer_ptr& snapshot ) const {
      standard_token_db_index_set::walk_indices([this, &snapshot]( auto utils ){
         snapshot->write_section<typename decltype(utils)::index_t::value_type>([this]( auto& section ){
            decltype(utils)::walk(_db, [this, &section]( const auto &row ) {
               section.add_row(row, _db);
            });
         });
      });
   }

   void standard_token_manager::read_from_snapshot( const snapshot_reader_ptr& snapshot ) {
      standard_token_db_index_set::walk_indices([this, &snapshot]( auto utils ){
         snapshot->read_section<typename decltype(utils)::index_t::value_type>([this]( auto& section ) {
            bool more = !section.empty();
            while(more) {
               decltype(utils)::create(_db, [this, &section, &more]( auto &row ) {
                  more = section.read_row(row, _db);
               });
            }
         });
      });
   }

   void standard_token_manager::set_token_meta_info( apply_context& context, const token_id_type &token_id, const token::settokenmeta &token_meta ) {

      auto url_size = token_meta.url.size();
      auto desc_size = token_meta.desc.size();

      EOS_ASSERT( token_meta.sym.valid(), token_meta_validate_exception, "invalid token symbol" );
      EOS_ASSERT( url_size > 0 && url_size <= 255, token_meta_validate_exception, "invalid token url size" );
      EOS_ASSERT( desc_size > 0 && desc_size <= 255, token_meta_validate_exception, "invalid token description size" );

      auto set_token_meta_lambda = [&token_meta, token_id, url_size, desc_size](token_meta_object& token_meta_obj) {
         token_meta_obj.token_id = token_id;
         token_meta_obj.sym = token_meta.sym;
         token_meta_obj.url.resize(url_size);
         memcpy( token_meta_obj.url.data(), token_meta.url.data(), url_size );
         token_meta_obj.desc.resize(desc_size);
         memcpy( token_meta_obj.desc.data(), token_meta.desc.data(), desc_size );
      };

      auto* token_meta_ptr = _db.find<token_meta_object, by_token_id>(token_id);
      if ( token_meta_ptr ) {
         EOS_ASSERT( token_meta_ptr->sym.value() == token_meta.sym.value(), token_meta_validate_exception, "token symbol cannot be modified once it is set" );
         EOS_ASSERT( token_meta_ptr->url != token_meta.url.c_str() || token_meta_ptr->desc != token_meta.desc.c_str(),
                     token_meta_validate_exception, "attempting update token metadata, but new metadata is same as old one" );

         _db.modify( *token_meta_ptr, set_token_meta_lambda );
      } else {
         _db.create<token_meta_object>( set_token_meta_lambda );

         context.add_ram_usage(token_id, (int64_t)(config::billable_size_v<token_meta_object>));
      }
   }

   const token_meta_object* standard_token_manager::get_token_meta_info( const token_id_type &token_id ) const {
      return _db.find<token_meta_object, by_token_id>(token_id);
   }

   const symbol& standard_token_manager::get_token_symbol( const token_id_type& token_id ) const {
      auto* token_meta_ptr = get_token_meta_info(token_id);
      EOS_ASSERT( token_meta_ptr, token_not_yet_created_exception, "token not yet created for the account ${token_id}", ("token_id", token_id) );
      return (*token_meta_ptr).sym;
   }

   const share_type& standard_token_manager::get_token_total_supply( const token_id_type& token_id ) const {
      auto* token_meta_ptr = get_token_meta_info(token_id);
      EOS_ASSERT( token_meta_ptr, token_not_yet_created_exception, "token not yet created for the account ${token_id}", ("token_id", token_id) );
      return (*token_meta_ptr).total_supply;
   }

   void standard_token_manager::update_token_total_supply( const token_meta_object* token_meta_ptr, share_type delta ) {
      _db.modify<token_meta_object>( *token_meta_ptr, [&](token_meta_object& token_meta) {
         token_meta.total_supply += delta;
      });
   }

   void standard_token_manager::add_token_balance( apply_context& context, token_id_type token_id, account_name owner, share_type value ) {

      EOS_ASSERT( context.receiver == token_id, invalid_token_balance_update_access_exception, "add_token_balance : action context receiver mismatches token-id" );

      auto* balance_ptr = _db.find<token_balance_object, by_token_account>(boost::make_tuple(token_id, owner));
      if ( balance_ptr ) {
         _db.modify<token_balance_object>( *balance_ptr, [&]( token_balance_object& balance_obj ) {
            balance_obj.balance += value;
         });
      } else {
         _db.create<token_balance_object>( [&](token_balance_object& balance_obj) {
            balance_obj.token_id = token_id;
            balance_obj.account = owner;
            balance_obj.balance = value;
         });

         context.add_ram_usage(owner, (int64_t)(config::billable_size_v<token_balance_object>));
      }
   }

   void standard_token_manager::subtract_token_balance( apply_context& context, token_id_type token_id, account_name owner, share_type value ) {

      EOS_ASSERT( context.receiver == token_id, invalid_token_balance_update_access_exception, "subtract_token_balance : action context receiver mismatches token-id" );

      auto* balance_ptr = _db.find<token_balance_object, by_token_account>(boost::make_tuple(token_id, owner));
      if ( balance_ptr ) {
         share_type cur_balance = balance_ptr->balance;
         EOS_ASSERT( cur_balance >= value, insufficient_token_balance_exception, "account ${account} has insufficient_token_balance", ("account", owner) );

         if ( cur_balance == value ) {
            _db.remove( *balance_ptr );
            context.add_ram_usage(owner, -(int64_t)(config::billable_size_v<token_balance_object>));
         } else {
            _db.modify<token_balance_object>( *balance_ptr, [&]( token_balance_object& balance_obj ) {
               balance_obj.balance -= value;
            });
         }

      } else {
         EOS_ASSERT( false, insufficient_token_balance_exception, "account ${account} has insufficient_token_balance", ("account", owner) );
      }
   }

   share_type standard_token_manager::get_token_balance( const token_id_type& token_id, const account_name& account ) const {
      auto* balance_ptr = _db.find<token_balance_object, by_token_account>(boost::make_tuple(token_id, account));
      if ( balance_ptr ) {
         return (*balance_ptr).balance;
      } else {
         return share_type(0);
      }
   }

   void standard_token_manager::pay_transaction_fee( transaction_context& trx_context, account_name fee_payer, uint32_t fee_amount ) {

      share_type fee_remaining = fee_amount;

      auto& sys_tokens = _db.get<infrablockchain_global_property_object>().system_token_list;

      for( const auto& sys_token : sys_tokens.system_tokens ) {

         auto sys_token_id = sys_token.token_id;

         auto* balance_ptr = _db.find<token_balance_object, by_token_account>(boost::make_tuple(sys_token_id, fee_payer));
         if ( balance_ptr ) {

            share_type fee_for_this_token = fee_remaining;
            share_type cur_balance = balance_ptr->balance;

            if (sys_token.token_weight != system_token::token_weight_1x) {
               fee_for_this_token = fee_for_this_token * system_token::token_weight_1x / sys_token.token_weight;
               if ( cur_balance >= fee_for_this_token ) {
                  fee_remaining = 0;
               } else {
                  fee_for_this_token = cur_balance;
                  fee_remaining -= cur_balance * sys_token.token_weight / system_token::token_weight_1x;
               }
            } else {
               if ( cur_balance >= fee_for_this_token ) {
                  fee_remaining = 0;
               } else {
                  fee_for_this_token = cur_balance;
                  fee_remaining -= cur_balance;
               }
            }

            auto token_meta_ptr = _db.find<token_meta_object, by_token_id>(sys_token_id);
            EOS_ASSERT( token_meta_ptr != nullptr, infrablockchain_transaction_fee_exception, "no token meta info for system token ${token_id}", ("token_id", sys_token_id) );

            // dispatch 'txfee' action for this system token
            trx_context.trace->action_traces.emplace_back();
            trx_context.dispatch_action( trx_context.trace->action_traces.back(),
               action { vector<permission_level>{ {fee_payer, config::active_name} },
                        sys_token_id,
                        token::txfee{ sys_token_id, fee_payer, asset(fee_for_this_token, token_meta_ptr->sym) }
               } );

            if (fee_remaining <= 0) break;
         }
      }

      EOS_ASSERT( fee_remaining <= 0, infrablockchain_transaction_fee_exception, "fee payer ${payer} does not have enough system token", ("payer", fee_payer) );
   }


   int64_t standard_token_manager::set_system_token_list( vector<system_token> system_tokens ) {

      auto& ygpo = _db.get<infrablockchain_global_property_object>();
      auto& current_sys_tokens = ygpo.system_token_list.system_tokens;

      if( std::equal( system_tokens.begin(), system_tokens.end(),
                      current_sys_tokens.begin(), current_sys_tokens.end() ) ) {
         return -1; // the system token list does not change
      }

      system_token_list_type new_sys_token_list;
      new_sys_token_list.version = ygpo.system_token_list.version + 1;
      new_sys_token_list.system_tokens = std::move(system_tokens);

      int64_t version = new_sys_token_list.version;

      _db.modify( ygpo, [&]( auto& ygp ) {
         ygp.system_token_list = std::move(new_sys_token_list);
      });
      return version;
   }

   int standard_token_manager::get_system_token_count() const {
      return static_cast<int>(_db.get<infrablockchain_global_property_object>().system_token_list.system_tokens.size());
   }

   vector<system_token> standard_token_manager::get_system_token_list() const {
      auto& active_sys_tokens = _db.get<infrablockchain_global_property_object>().system_token_list.system_tokens;

      vector<system_token> sys_tokens;
      sys_tokens.reserve( active_sys_tokens.size() );

      for(const auto& sys_token : active_sys_tokens ) {
         sys_tokens.push_back({sys_token.token_id, sys_token.token_weight});
      }
      return sys_tokens;
   }

   system_token_balance standard_token_manager::get_system_token_balance( const account_name& account ) const {
      system_token_balance result;

      share_type total_balance = 0;

      auto& active_sys_tokens = _db.get<infrablockchain_global_property_object>().system_token_list.system_tokens;
      for(const auto& sys_token : active_sys_tokens ) {
         system_token_id_type sys_token_id = sys_token.token_id;
         share_type token_balance = get_token_balance( sys_token_id, account );
         if (token_balance > 0) {
            const symbol& token_symbol = get_token_symbol( sys_token_id );
            if ( sys_token.token_weight == system_token::token_weight_1x ) {
               total_balance += token_balance;
            } else {
               total_balance += (token_balance * sys_token.token_weight) / system_token::token_weight_1x;
            }
            result.sys_tokens.emplace_back(sys_token_id, asset(token_balance, token_symbol));
         }
      }
      result.total = asset(total_balance, symbol(INFRABLOCKCHAIN_NATIVE_TOKEN_SYMBOL));
      return result;
   }

} } // namespace infrablockchain::chain
