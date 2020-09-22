/**
 *  @file contracts/sys.tokenabi/sys.tokenabi.hpp
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/symbol.hpp>

#include <string>

namespace infrablockchain { namespace contract {

   using namespace eosio;
   using std::string;

   /**
    * Every account on InfraBlockchain can process built-in standard token actions (settokenmeta, issue, transfer, redeem)
    * without custom smart contract code deployed to an account.
    * This system contract (sys.tokenabi) provide standard application binary interface(abi) for standard token actions.
    * Client tools such as infra-cli encoding InfraBlockchain transactions could refer 'sys.tokenabi' contract account
    * to retrieve abi information for standard token actions.
    */
   class tokenabi : public eosio::contract {
   public:
      explicit tokenabi( account_name self ) : contract(self) {}

      /**
       * Set token meta info
       * @param sym - token symbol (precision, symbol name)
       * @param url - web site url providing token information managed by token issuer
       * @param desc - token description
       */
      void settokenmeta( const symbol_type sym, const string& url, const string& desc );

      /**
       * Issue new token
       * @param t - token id (token account name)
       * @param to - account name receiving the issued tokens
       * @param qty - token quantity (amount, symbol) to issue
       * @param tag - user tag string to identity a specific issue action (application-specific purpose)
       */
      void issue( const account_name t, const account_name to, const asset& qty, const string& tag );

      /**
       * Transfer token
       * @param t - token id (token account name)
       * @param from - account name sending tokens
       * @param to - account name receiving tokens
       * @param qty - token quantity (amount, symbol) to transfer
       * @param tag - user tag string to identity a specific transfer action (application-specific purpose)
       */
      void transfer( const account_name t, const account_name from, const account_name to, const asset& qty, const string& tag );

      /**
       * Transaction fee payment
       * if current token account is selected as a system token,
       * 'txfee' actions are generated from InfraBlockchain core after processing actions on a submitted transaction
       * @param t - token id (token account name)
       * @param payer - account name paying transaction fee
       * @param fee - token quantity (amount, symbol) being charged as transaction fee
       */
      void txfee( const account_name t, const account_name payer, const asset& fee );

      /**
       * Redeem(burn) token
       * only token issuer can redeem(burn) its own token
       * @param qty token quantity (amount, symbol) to redeem
       * @param tag user tag string to identity a specific redeem action (application-specific purpose)
       */
      void redeem( const asset& qty, const string& tag );
   };

} } /// namespace infrablockchain::contract
