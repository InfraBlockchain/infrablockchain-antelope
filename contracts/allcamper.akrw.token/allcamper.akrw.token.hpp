#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/symbol.hpp>

#include <string>

namespace allcamper {

   using namespace eosio;
   using std::string;

   class akrw_token : public eosio::contract {
   public:
      explicit akrw_token( account_name self ) : contract(self) {}

      /**
       * Set token meta info
       * @param sym - token symbol (precision, symbol name)
       * @param url - web site url providing token information managed by token issuer
       * @param desc - token description
       */
      // just inherit built-in 'settokenmeta' action (INFRABLOCKCHAIN chain core)
      //void settokenmeta( const symbol_type sym, const string& url, const string& desc );

      /**
       * Issue new token
       * @param t - token id (token account name)
       * @param to - account name receiving the issued tokens
       * @param qty - token quantity (amount, symbol) to issue
       * @param tag - user tag string to identify a specific issue action (application-specific purpose)
       */
      void issue( const account_name t, const account_name to, const asset& qty, const string& tag );

      /**
       * Transfer token
       * @param t - token id (token account name)
       * @param from - account name sending tokens
       * @param to - account name receiving tokens
       * @param qty - token quantity (amount, symbol) to transfer
       * @param tag - user tag string to identify a specific transfer action (application-specific purpose)
       */
      void transfer( const account_name t, const account_name from, const account_name to, const asset& qty, const string& tag );

      /**
       * Transaction fee payment
       * if current token account is selected as a system token,
       * 'txfee' actions are generated from INFRABLOCKCHAIN blockchain core after processing actions on a submitted transaction
       * @param t - token id (token account name)
       * @param payer - account name paying transaction fee
       * @param fee - token quantity (amount, symbol) being charged as transaction fee
       */
      void txfee( const account_name t, const account_name payer, const asset& fee );

      /**
       * Redeem(burn) token
       * only token issuer can redeem(burn) its own token
       * @param qty token quantity (amount, symbol) to redeem
       * @param tag user tag string to identify a specific redeem action (application-specific purpose)
       */
      // just inherit built-in 'redeem' action (INFRABLOCKCHAIN chain core)
      //void redeem( const asset& qty, const string& tag );
   };

} /// namespace allcamper
