#pragma once

#include <eosio/chain/types.hpp>

namespace infrablockchain { namespace chain { namespace config {

   const static name infrablockchain_system_account_name { "eosio"_n };
   const static name infrablockchain_standard_token_interface_abi_account_name { "sys.tokenabi"_n };
   const static name infrablockchain_sys_tx_fee_account_name { "sys.txfee"_n };

} } } // namespace infrablockchain::chain::config
