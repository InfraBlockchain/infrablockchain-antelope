/**
 *  @file contracts/yosemitex.credit.token/yosemitex.credit.token.hpp
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

   class credit_token : public eosio::contract {
   public:
      explicit credit_token( account_name self ) : contract(self) {}

      /**
       * Set token meta info
       * @param sym - token symbol (precision, symbol name)
       * @param url - web site url providing token information managed by token issuer
       * @param description - token description
       */
      // just inherit built-in 'settokenmeta' action (YOSEMITE chain core)
      //void settokenmeta( const symbol_type sym, const string& url, const string& desc );

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
       * 'txfee' actions are generated from YOSEMITE blockchain core after processing actions on a submitted transaction
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
      // just inherit built-in 'redeem' action (YOSEMITE chain core)
      //void redeem( const asset& qty, const string& tag );


      /**
       * Set Credit Limit
       * The token account owner can set a credit limit for each user account.
       * Only token owner account can send 'creditlimit' action
       * @param account - account name to set credit limit
       * @param credit_limit - credit limit amount within which the target account can issue new tokens without the permission of token owner account.
       * @param tag - user tag string to identity a specific creditlimit action (application-specific purpose)
       */
      void creditlimit( const account_name account, const asset& credit_limit, const string& tag );

      /**
       * Issue New Token By User Account Having Credit Limit
       * Within the credit limit allowed by token owner account, an account can issue new tokens without the permission of token owner account.
       * @param issuer - issuer account name having credit limit, issuer account must provide signature for creditissue action
       * @param to - account name receiving issued tokens, if 'to' != 'issuer', additional 'transfer' action will be generated
       * @param qty - token quantity (amount, symbol) to issue
       * @param tag - user tag string to identity a specific creditissue action (application-specific purpose)
       */
      void creditissue( const account_name issuer, const account_name to, const asset& qty, const string& tag );

      /**
       * Redeem Credit Tokens Issued By User Account
       * Whenever an account issues new tokens within the credit limit,
       * the issued credit amount is increased.
       * When the account repay the dept of issued credit tokens, the token account owner
       * can decrease the issued credit amount for the account to issue additional tokens within credit limit.
       * Only token owner account can send 'creditredeem' action.
       * @param account - account name to set credit limit
       * @param qty - token quantity (amount, symbol) to decrese issused credit amount
       * @param tag - user tag string to identity a specific creditredeem action (application-specific purpose)
       */
      void creditredeem( const account_name account, const asset& qty, const string& tag );

      /**
       * The credit token owner can log off-chain token transfer events on blockchain.
       * When a token transfer event occurs within the fund deposited to the token owner account (for anonymous token transfer transaction),
       * the token owner account could write evidence (hash of token transfer data) of off-chain token transfer.
       * 'offtransfer' action reveals only the token receiver account name, and the actual transfer event data (token sender/receiver, amount, signatures and additional data)
       * signed by token sender and anonymous token transfer service provider (token owner account) are stored privately on service side
       * for on-demand evidence presentation to compliance authorities.
       * @param t - token id (token account name)
       * @param to - account name receiving token in off-chain token transfer transaction
       * @param tag - hash (e.g. IPFS multihash base58 string) of off-chain token transfer transaction data
       */
      void offtransfer( const account_name t, const account_name to, const string& tag );
   };

   /**
    * The token account owner can set a credit limit for each user account
    * upto which an account can issue new tokens without the permission of token owner account.
    * Whenever an account issues new tokens within the credit limit,
    * the issued credit amount is increased.
    * When the account repay the dept of issued credit tokens, the token account owner
    * can decrease the issued credit amount for the account to issue additional tokens within credit limit.
    */
   struct credit_offering_info {
      account_name    account;
      int64_t         credit_limit;
      int64_t         credit_issued;

      uint64_t primary_key() const { return account; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( credit_offering_info, (account)(credit_limit)(credit_issued) )
   };

   typedef eosio::multi_index< N(creditoffer), credit_offering_info > credit_offering_table_idx;


} } /// namespace yosemite::contract
