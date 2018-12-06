/**
 *  @file
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosiolib/types.h>

extern "C" {

   /**
    *  @defgroup YOSEMITE Transaction Fee Payer API
    *  @ingroup contractdev
    *  @brief Defines API for Delegated-Transaction-Fee-Payment
    */

   /**
    *  @defgroup YOSEMITE Transaction Fee Payer C API
    *  @ingroup trxfeepayerapi
    *  @brief Defines API for Transaction-Fee-Payer
    *  @{
    */

   /**
    *  Get the transaction fee payer account name
    *  @brief Get the transaction fee payer account to which transaction fee is charged
    *  @return the transaction fee payer account name
    */
   account_name trx_fee_payer();

   ///@ } trxfeepayerapi
}
