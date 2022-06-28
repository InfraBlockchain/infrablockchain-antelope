#pragma once

#include <eosio/chain/types.hpp>

namespace infrablockchain { namespace chain {

   using namespace eosio::chain;

   const static name infrablockchain_system_account_name { N(eosio) };
   const static name infrablockchain_standard_token_interface_abi_account_name { N(sys.tokenabi) };
   const static name infrablockchain_sys_tx_fee_account_name { N(sys.txfee) };

} } // namespace infrablockchain::chain
