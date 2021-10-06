/**
 *  @file chain/infrablockchain/transaction_extensions.cpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */

#include <fc/io/raw.hpp>

#include <infrablockchain/chain/transaction_extensions.hpp>
#include <infrablockchain/chain/exceptions.hpp>

namespace infrablockchain { namespace chain {

   void transaction_fee_payer_tx_ext::reflector_init() {
      static_assert( fc::raw::has_feature_reflector_init_on_unpacked_reflected_types,
                     "transaction_fee_payer_tx_ext expects FC to support reflector_init" );

      EOS_ASSERT( fee_payer != account_name(), ill_formed_transaction_fee_payer_tx_ext,
                  "Transaction fee payer tx extension must have a non-empty fee-payer account",
      );
   }

} } /// infrablockchain::chain
