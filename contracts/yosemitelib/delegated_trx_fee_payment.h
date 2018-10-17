/**
 *  @file
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosiolib/types.h>

extern "C" {

   /**
    *  @defgroup YOSEMITE Delegated Transaction Fee Payment API
    *  @ingroup contractdev
    *  @brief Defines API for Delegated-Transaction-Fee-Payment
    */

   /**
    *  @defgroup YOSEMITE Delegated Transaction Fee Payment C API
    *  @ingroup dtrxfeepayapi
    *  @brief Defines API for Delegated-Transaction-Fee-Payment
    *  @{
    */

   /**
    *  Get the delegated transaction fee payer account name
    *  @brief Get the delegated transaction fee payer account to which transaction fee is charged
    *  @return the transaction fee payer account name
    */
   account_name delegated_trx_fee_payer();

   ///@ } dtrxfeepayapi
}
