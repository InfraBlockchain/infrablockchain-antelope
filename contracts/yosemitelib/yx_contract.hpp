/**
 * @file
 * @copyright defined in yosemite-public-blockchain/LICENSE
 */
#pragma once

#include <eosiolib/contract.hpp>
#include <yosemitelib/yx_asset.hpp>

namespace yosemite {

   using namespace eosio;

   /**
    * @defgroup contracttype Yosemite Contract Type
    * @ingroup types
    * @brief Defines Yosemite contract type which is %base class for every YOSEMITE contract
    *
    * @{
    *
    */

   /**
    * @brief %Base class for YOSEMITE contract.
    * @details %Base class for YOSEMITE contract. %A new contract should derive from this class, so it can make use of EOSIO_ABI macro.
    */
   class yx_contract : public contract {
   public:
      /**
       * Construct a new contract given the contract name
       *
       * @brief Construct a new contract object.
       * @param n - The name of this contract
       */
      explicit yx_contract(account_name n) : contract(n) {}

   protected:
      void transfer_token_as_inline(account_name from, account_name to, const yx_asset &token, const std::string &memo);
   };

/// @} contracttype
} /// namespace yosemite
