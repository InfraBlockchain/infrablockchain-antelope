/**
 *  @file chain/webassembly/infrablockchain/system_token_intrinsics.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <eosio/chain/webassembly/interface.hpp>
#include <eosio/chain/apply_context.hpp>

#include <infrablockchain/chain/standard_token_manager.hpp>
#include <infrablockchain/chain/system_token_list.hpp>

#include <vector>
#include <set>

namespace eosio { namespace chain { namespace webassembly {

   ///////////////////////////////////////////////////////////////
   /// InfraBlockchain System Token Core API (Intrinsics)

   /**
    *  Get System Token Count
    *  @brief get the number of active system tokens authorized by block producers and used as transaction fee payment token
    *
    *  @return the number of system tokens
    */
   uint32_t interface::get_system_token_count() const {
       return context.control.get_standard_token_manager().get_system_token_count();
   }

   /**
    * Get System Token List
    * @brief Retrieve the system token list
    *
    * @param[out] packed_system_token_list - output buffer of the system token list (vector<infrablockchain_system_token>), only retrieved if sufficent size to hold packed data.
    *
    * return the number of bytes copied to the buffer, or number of bytes required if the buffer is empty.
    */
   uint32_t interface::get_system_token_list_packed( legacy_span<char> packed_system_token_list ) const {
       std::vector<infrablockchain::chain::system_token> system_tokens = std::move(context.control.get_standard_token_manager().get_system_token_list().system_tokens);

       auto s = fc::raw::pack_size( system_tokens );
       if( packed_system_token_list.size() == 0 ) return s;

       if ( s <= packed_system_token_list.size() ) {
           datastream<char*> ds( packed_system_token_list.data(), s );
           fc::raw::pack(ds, system_tokens);
           return s;
       }
       return 0;
   }

   /**
    * Set System Token List
    * @brief set the list of system tokens (vector<infrablockchain_system_token>) used as transaction fee payment token.
    *        2/3+ block producers have to sign any modification of system token list.
    *
    * @param packed_system_token_list - packed data of system_token vector in the appropriate system token order
    *
    * @return -1 if setting new system token list was unsuccessful, otherwise returns the version of the new system token list
    */
   int64_t interface::set_system_token_list_packed( legacy_span<const char> packed_system_token_list ) {

       //context.require_authorization(config::system_account_name);

       datastream<const char*> ds( packed_system_token_list.data(), packed_system_token_list.size() );
       std::vector<infrablockchain::chain::system_token> system_tokens;
       fc::raw::unpack(ds, system_tokens);
       EOS_ASSERT(system_tokens.size() <= infrablockchain::chain::max_system_tokens, wasm_execution_error, "System token list exceeds the maximum system token count for this chain");

       // check that system tokens are unique
       std::set<infrablockchain::chain::system_token_id_type> unique_sys_tokens;
       for (const auto& sys_token : system_tokens) {
           EOS_ASSERT( context.is_account(sys_token.token_id), wasm_execution_error, "system token list includes a nonexisting account" );
           EOS_ASSERT( sys_token.valid(), wasm_execution_error, "system token list includes an invalid value" );
           unique_sys_tokens.insert(sys_token.token_id);
       }
       EOS_ASSERT( system_tokens.size() == unique_sys_tokens.size(), wasm_execution_error, "duplicate system token id in system token list" );

       return context.control.get_mutable_standard_token_manager().set_system_token_list( std::move(system_tokens) );
   }


}}} /// eosio::chain::webassembly
