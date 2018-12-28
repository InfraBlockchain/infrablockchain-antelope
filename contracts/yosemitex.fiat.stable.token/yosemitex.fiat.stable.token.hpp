/**
 *  @file contracts/yosemitex.fiat.stable.token/yosemitex.fiat.stable.token.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/symbol.hpp>

#include <string>

namespace yosemitex { namespace contract {

   using namespace eosio;
   using std::string;

   class fiat_stable_token : public eosio::contract {
   public:
      explicit fiat_stable_token( account_name self ) : contract(self) {}

      /**
       * Set token meta info
       * @param sym - token symbol (precision, symbol name)
       * @param url - web site url providing token information managed by token issuer
       * @param desc - token description
       */
      // just inherit built-in 'settokenmeta' action (YOSEMITE chain core)
      //void settokenmeta( symbol_type sym, string url, string desc );

      /**
       * Issue new token
       * @param t - token id (token account name)
       * @param to - account name receiving the issued tokens
       * @param qty - token quantity (amount, symbol) to issue
       * @param tag - user tag string to identity a specific issue action (application-specific purpose)
       */
      void issue( account_name t, account_name to, asset qty, string tag );

      /**
       * Transfer token
       * @param t - token id (token account name)
       * @param from - account name sending tokens
       * @param to - account name receiving tokens
       * @param qty - token quantity (amount, symbol) to transfer
       * @param tag - user tag string to identity a specific transfer action (application-specific purpose)
       */
      void transfer( account_name t, account_name from, account_name to, asset qty, string tag );

      /**
       * Transaction fee payment
       * if current token account is selected as a system token,
       * 'txfee' actions are generated from YOSEMITE blockchain core after processing actions on a submitted transaction
       * @param t - token id (token account name)
       * @param payer - account name paying transaction fee
       * @param fee - token quantity (amount, symbol) being charged as transaction fee
       */
      void txfee( account_name t, account_name payer, asset fee );

      /**
       * Redeem(burn) token
       * only token issuer can redeem(burn) its own token
       * @param qty token quantity (amount, symbol) to redeem
       * @param tag user tag string to identity a specific redeem action (application-specific purpose)
       */
      // just inherit built-in 'redeem' action (YOSEMITE chain core)
      //void redeem( asset qty, string tag );
   };

} } /// namespace yosemite::contract
