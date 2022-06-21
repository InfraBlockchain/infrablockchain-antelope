/**
 *  @file chain/infrablockchain/standard_token_manager.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <eosio/chain/core_symbol.hpp>
#include <eosio/chain/config.hpp>
#include <eosio/chain/exceptions.hpp>
#include <eosio/chain/database_utils.hpp>
#include <eosio/chain/trace.hpp>

#include <infrablockchain/chain/standard_token_manager.hpp>
#include <infrablockchain/chain/infrablockchain_global_property_object.hpp>
#include <infrablockchain/chain/standard_token_action_types.hpp>
#include <infrablockchain/chain/exceptions.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   using standard_token_db_index_set = index_set<
      token_meta_multi_index,
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
         using section_t = typename decltype(utils)::index_t::value_type;
         snapshot->write_section<section_t>([this]( auto& section ){
            decltype(utils)::walk(_db, [this, &section]( const auto &row ) {
               section.add_row(row, _db);
            });
         });
      });
   }

   void standard_token_manager::read_from_snapshot( const snapshot_reader_ptr& snapshot ) {
      standard_token_db_index_set::walk_indices([this, &snapshot]( auto utils ){
         using section_t = typename decltype(utils)::index_t::value_type;
         snapshot->read_section<section_t>([this]( auto& section ) {
            bool more = !section.empty();
            while(more) {
               decltype(utils)::create(_db, [this, &section, &more]( auto &row ) {
                  more = section.read_row(row, _db);
               });
            }
         });
      });
   }

   void standard_token_manager::set_token_meta_info( apply_context& context, const token_id_type &token_id, const standard_token::settokenmeta &token_meta ) {

      auto url_size = token_meta.url.size();
      auto desc_size = token_meta.desc.size();

      EOS_ASSERT( token_meta.sym.valid(), token_meta_validate_exception, "invalid token symbol" );
      EOS_ASSERT( url_size > 0 && url_size <= 255, token_meta_validate_exception, "invalid token url size" );
      EOS_ASSERT( desc_size > 0 && desc_size <= 255, token_meta_validate_exception, "invalid token description size" );

      auto* token_meta_obj_ptr = _db.find<token_meta_object, by_token_id>( token_id );
      if ( token_meta_obj_ptr ) {
         EOS_ASSERT( token_meta_obj_ptr->sym.value() == token_meta.sym.value(), token_meta_validate_exception, "token symbol cannot be modified once it is set" );
         EOS_ASSERT( token_meta_obj_ptr->url.compare(0, token_meta_obj_ptr->url.size(), token_meta.url.data(), token_meta.url.size()) != 0
                     || token_meta_obj_ptr->desc.compare(0, token_meta_obj_ptr->desc.size(), token_meta.desc.data(), token_meta.desc.size()) != 0,
                     token_meta_validate_exception, "attempting update token metadata, but new metadata is same as old one" );

         _db.modify( *token_meta_obj_ptr, [&](token_meta_object& token_meta_obj) {
            token_meta_obj.url.assign(token_meta.url.data(), url_size);
            token_meta_obj.desc.assign(token_meta.desc.data(), desc_size);
         } );
      } else {
         _db.create<token_meta_object>( [&](token_meta_object& token_meta_obj) {
            token_meta_obj.token_id = token_id;
            token_meta_obj.sym = token_meta.sym;
            token_meta_obj.url.assign(token_meta.url.data(), url_size);
            token_meta_obj.desc.assign(token_meta.desc.data(), desc_size);
         } );

         context.add_ram_usage(token_id, (int64_t)(config::billable_size_v<token_meta_object>) );
      }
   }

   const token_meta_object* standard_token_manager::get_token_meta_object( const token_id_type &token_id ) const {
      return _db.find<token_meta_object, by_token_id>(token_id);
   }

   const symbol standard_token_manager::get_token_symbol( const token_id_type& token_id ) const {
      auto* token_meta_obj_ptr = get_token_meta_object(token_id);
      EOS_ASSERT( token_meta_obj_ptr, token_not_yet_created_exception, "token not yet created for the account ${token_id}", ("token_id", token_id) );
      return (*token_meta_obj_ptr).sym;
   }

   const share_type standard_token_manager::get_token_total_supply( const token_id_type& token_id ) const {
      auto* token_meta_obj_ptr = get_token_meta_object(token_id);
      EOS_ASSERT( token_meta_obj_ptr, token_not_yet_created_exception, "token not yet created for the account ${token_id}", ("token_id", token_id) );
      return (*token_meta_obj_ptr).total_supply;
   }

   void standard_token_manager::update_token_total_supply( const token_meta_object* token_meta_obj_ptr, share_type delta ) {
      _db.modify<token_meta_object>( *token_meta_obj_ptr, [&](token_meta_object& token_meta) {
         token_meta.total_supply += delta;
      });
   }

   void standard_token_manager::add_token_balance( apply_context& context, token_id_type token_id, account_name owner, share_type value ) {

      EOS_ASSERT( context.get_receiver() == token_id, invalid_token_balance_update_access_exception, "add_token_balance : action context receiver mismatches token-id" );

      auto* balance_obj_ptr = _db.find<token_balance_object, by_token_account>( boost::make_tuple(token_id, owner) );
      if ( balance_obj_ptr ) {
         _db.modify<token_balance_object>( *balance_obj_ptr, [&]( token_balance_object& balance_obj ) {
            balance_obj.balance += value;
         });
      } else {
         _db.create<token_balance_object>( [&](token_balance_object& balance_obj) {
            balance_obj.token_id = token_id;
            balance_obj.account = owner;
            balance_obj.balance = value;
         });

         context.add_ram_usage(owner, (int64_t)(config::billable_size_v<token_balance_object>) );
      }
   }

   void standard_token_manager::subtract_token_balance( apply_context& context, token_id_type token_id, account_name owner, share_type value ) {

      EOS_ASSERT( context.get_receiver() == token_id, invalid_token_balance_update_access_exception, "subtract_token_balance : action context receiver mismatches token-id" );

      auto* balance_obj_ptr = _db.find<token_balance_object, by_token_account>(boost::make_tuple(token_id, owner));
      if ( balance_obj_ptr ) {
         share_type cur_balance = balance_obj_ptr->balance;
         EOS_ASSERT( cur_balance >= value, insufficient_token_balance_exception, "account ${account} has insufficient token balance (token-id: ${token_id})", ("account", owner)("token_id", token_id) );

         if ( cur_balance == value ) {
            _db.remove( *balance_obj_ptr );

            context.add_ram_usage(owner, -(int64_t)(config::billable_size_v<token_balance_object>) );
         } else {
            _db.modify<token_balance_object>( *balance_obj_ptr, [&]( token_balance_object& balance_obj ) {
               balance_obj.balance -= value;
            });
         }

      } else {
         EOS_ASSERT( false, insufficient_token_balance_exception, "account ${account} has insufficient token balance (token-id: ${token_id})", ("account", owner)("token_id", token_id) );
      }
   }

   share_type standard_token_manager::get_token_balance( const token_id_type& token_id, const account_name& account ) const {
      auto* balance_obj_ptr = _db.find<token_balance_object, by_token_account>(boost::make_tuple(token_id, account));
      if ( balance_obj_ptr ) {
         return (*balance_obj_ptr).balance;
      } else {
         return share_type(0);
      }
   }

   int64_t standard_token_manager::set_system_token_list( vector<system_token> system_tokens ) {

      auto& ibgpo = _db.get<infrablockchain_global_property_object>();
      auto& current_sys_tokens = ibgpo.system_token_list.system_tokens;

      if( std::equal( system_tokens.begin(), system_tokens.end(),
                      current_sys_tokens.begin(), current_sys_tokens.end() ) ) {
         return -1; // the system token list does not change
      }

      system_token_list new_sys_token_list;
      new_sys_token_list.version = ibgpo.system_token_list.version + 1;
      new_sys_token_list.system_tokens = std::move(system_tokens);

      int64_t version = new_sys_token_list.version;

      _db.modify( ibgpo, [&]( auto& ibgp ) {
         ibgp.system_token_list = new_sys_token_list;
      });
      return version;
   }

   int standard_token_manager::get_system_token_count() const {
      return static_cast<int>(_db.get<infrablockchain_global_property_object>().system_token_list.system_tokens.size());
   }

   system_token_list standard_token_manager::get_system_token_list() const {
      return _db.get<infrablockchain_global_property_object>().system_token_list.to_system_token_list();
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
            result.sys_tokens.emplace_back(sys_token_id, asset(token_balance, token_symbol));

            share_type weighted_token_balance = token_balance;
            if ( sys_token.token_weight != system_token::token_weight_1x ) {
               eosio::chain::uint128_t weighted_token_balance_128 = ((eosio::chain::uint128_t)token_balance * sys_token.token_weight) / (eosio::chain::uint128_t)system_token::token_weight_1x;

               EOS_ASSERT(weighted_token_balance_128 <= std::numeric_limits<share_type>::max(), weighted_system_token_balance_overflow_exception,
                          "weighted system token balance overflow (account: ${account}, system-token-id: ${sys_token_id})", ("account", account)("sys_token_id", sys_token_id) );
               weighted_token_balance = (share_type)weighted_token_balance_128;
            }

            EOS_ASSERT(total_balance <= std::numeric_limits<share_type>::max() - weighted_token_balance, weighted_total_system_token_balance_per_account_overflow_exception,
                       "weighted total system token balance overflow (account: ${account})", ("account", account) );
            total_balance += weighted_token_balance;
         }
      }
      result.total = asset(total_balance, symbol(CORE_SYMBOL));
      return result;
   }

} } // namespace infrablockchain::chain
