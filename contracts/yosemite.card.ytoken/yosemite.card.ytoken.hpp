/**
 *  @file contracts/yosemite.card.ytoken/yosemite.card.ytoken.hpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/symbol.hpp>
#include <eosiolib/singleton.hpp>

#include <string>

namespace yosemitex { namespace contract {

   using namespace eosio;
   using std::string;

   /**
    * Yosemite Token, gift token backed stable coin.
    * Merchant-issuing gift tokens are redeemable for real goods services in the merchant shops.
    * All of the issued Yosemite Tokens (YTOKEN) are equally backed by the sum of the redeemable gift tokens
    * and the fiat-money-backed tokens which are used for fiat-money related business operations of yosemite card.
    * The issued Yosemite Tokens can be consumed at any merchant shop who has issued gift tokens for Yosemite Token.
    */
   class yosemite_card_ytoken : public eosio::contract {
   public:
      explicit yosemite_card_ytoken( account_name self ) : contract(self) {}

      /**
       * TODO
       *
       * @param merchant TODO
       * @param qty TODO
       * @param paid TODO
       * @param tag TODO
       */
      void ytokenissue( const account_name merchant, const asset& qty /* YTOKEN */, const asset& paid /* USD or YUSD */, const string& tag );

      /**
       * TODO
       *
       * @param merchant TODO
       * @param qty TODO
       * @param tag TODO
       */
      void ytokenburn( const account_name merchant, const asset& qty /* YTOKEN */, const string& tag );

      /**
       * TODO
       *
       * @param account TODO
       * @param qty TODO
       * @param redeemed TODO
       * @param tag TODO
       */
      void ytokenredeem( const account_name account, const asset& qty /* YTOKEN */, const asset& redeemed /* USD or YUSD */, const string& tag );

      /**
       * TODO
       *
       * @param merchant TODO
       * @param price TODO
       * @param credit TODO
       * @param ytoken TODO
       * @param reward TODO
       * @param tag TODO
       */
      void ytokenpay( const account_name merchant, const asset& price /* YTOKEN */, const asset& credit /* CREDIT */, const asset& ytoken /* YTOKEN */, const asset& reward /* YTOKEN */, const string& tag );


      /**
       * Issue Fiat(USD)-backed Yosemite Token
       *
       * @param qty TODO
       * @param tag TODO
       */
      void usdytissue( const asset& qty /* USD */, const string& tag );


      /**
       * TODO
       *
       * @param to TODO
       * @param qty TODO
       * @param tag TODO
       */
      void usdredeemto( const account_name to, const asset& qty /* USD or YUSD */, const string& tag );

      /**
       * TODO
       *
       * @param from TODO
       * @param to TODO
       * @param qty TODO
       * @param tag TODO
       */
      void yusdtransfer( const account_name from, const account_name to, const asset& qty /* YUSD */, const string& tag );

      /**
       * Redeem Fiat-Token Request
       *
       * @param account TODO
       * @param qty TODO
       * @param tag TODO
       */
      void yusdredeemrq( const account_name account, const asset& qty /* YUSD */, const string& tag );

      /**
       * Cancel Redeem-Fiat-Token-Request
       *
       * @param account TODO
       * @param qty TODO
       * @param tag TODO
       */
      void cnclyusdrdrq( const account_name account, const asset& qty /* YUSD */, const string& tag );


//      /**
//       * TODO
//       *
//       * @param account TODO
//       * @param qty TODO
//       * @param tag TODO
//       */
//      void yusdredeemed( const account_name account, const asset& qty /* YUSD */, const string& tag );

      /**
       * Set Credit Limit
       * The token account owner can set a credit limit for each user account.
       * Only token owner account can send 'creditlimit' action
       *
       * @param account - account name to set credit limit
       * @param credit_limit - credit limit amount within which the target account can issue new tokens without the permission of token owner account.
       * @param tag - user tag string to identity a specific creditlimit action (application-specific purpose)
       */
      void creditlimit( const account_name account, const asset& credit_limit /* CREDIT */, const string& tag );

      /**
       * Issue New Token By User Account Having Credit Limit
       * Within the credit limit allowed by token owner account, an account can issue new tokens without the permission of token owner account.
       *
       * @param issuer - issuer account name having credit limit, issuer account must provide signature for creditissue action
       * @param to - account name receiving issued tokens, if 'to' != 'issuer', additional 'transfer' action will be generated
       * @param qty - token quantity (amount, symbol) to issue
       * @param tag - user tag string to identity a specific creditissue action (application-specific purpose)
       */
      void creditissue( const account_name issuer, const account_name to, const asset& qty /* CREDIT */, const string& tag );

      /**
       * TODO
       *
       * @param from TODO
       * @param to TODO
       * @param qty TODO
       * @param tag TODO
       */
      void credittxfer( const account_name from, const account_name to, const asset& qty /* CREDIT */, const string& tag );

      /**
       * Redeem Credit Tokens Issued By User Account
       * Whenever an account issues new tokens within the credit limit,
       * the issued credit amount is increased.
       * When the account repay the dept of issued credit tokens, the token account owner
       * can decrease the issued credit amount for the account to issue additional tokens within credit limit.
       * Only token owner account can send 'creditredeem' action.
       *
       * @param account - account name to set credit limit
       * @param qty - token quantity (amount, symbol) to decrese issused credit amount
       * @param tag - user tag string to identity a specific creditredeem action (application-specific purpose)
       */
      void creditsettle( const account_name account, const asset& qty /* CREDIT */, const string& tag );

      /**
       * TODO
       *
       * @param qty TODO
       * @param tag TODO
       */
      void creditburn( const asset& qty /* CREDIT */, const string& tag );


      ///////////////////////////////////////////////////////
      /// YOSEMITE Standard Token Actions

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
      void redeem( const asset& qty, const string& tag );

   private:

      void issue_yosemite_token( const account_name ytoken_issuer, const int64_t ytoken_amount, bool write_last_issued );
      void burn_yosemite_token( const account_name ytoken_issuer, const int64_t ytoken_amount, bool is_consumed_by_customer );

      void stat_add_ytoken_issue_total_available( const int64_t ytoken_amount );
      void stat_subtract_ytoken_issue_total_available( const int64_t ytoken_amount );

      void add_yusd_token_balance( const account_name account, const int64_t yusd_token_amount );
      void subtract_yusd_token_balance( const account_name account, const int64_t yusd_token_amount );

      void stat_add_yusd_token_total_supply( const int64_t yusd_token_amount );
      void stat_subtract_yusd_token_total_supply( const int64_t yusd_token_amount );

      void add_credit_token_balance( const account_name account, const int64_t credit_token_amount );
      void subtract_credit_token_balance( const account_name account, const int64_t credit_token_amount );

      void stat_add_credit_token_total_offered_credit_limit( const int64_t credit_token_amount );
      void stat_subtract_credit_token_total_offered_credit_limit( const int64_t credit_token_amount );
      void stat_add_credit_token_total_supply( const int64_t credit_token_amount );
      void stat_subtract_credit_token_total_supply( const int64_t credit_token_amount );
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

   struct credit_token_balance_info {
      account_name    account;
      int64_t         credit_token_balance;

      uint64_t primary_key() const { return account; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( credit_token_balance_info, (account)(credit_token_balance) )
   };

   typedef eosio::multi_index< N(credittoken), credit_token_balance_info > credit_token_table_idx;

   struct credit_token_stat {
      asset total_offered_credit_limit;
      asset total_credit_token_supply;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( credit_token_stat, (total_offered_credit_limit)(total_credit_token_supply) )
   };

   typedef eosio::singleton< N(creditstat), credit_token_stat > credit_token_stat_singleton;

   // Gift tokens and fiat tokens backing all of the issued Yosemite Tokens
   // The sum of yosemite_token_issue_info.available must equal to the total supply of Yosemite Token (YTOKEN)
   // If yosemite_token_issue_info.account is merchant account, the issued yosemite_token_issue_info.available balance is
   // the sum of nominal values of the issued gift tokens redeemable for the real goods and services.
   // If yosemite_token_issue_info.account = 'fiatbacked', the issued yosemite_token_issue_info.available balance is
   // the total fiat money token amount 1:1 backed by the real money, i.e. redeemable for the real money by the contract owner (yosemite card)
   struct yosemite_token_issue_info {
      account_name   account;
      int64_t        total_issued; // total issued amount of Yosemite Token by the account (merchant or yosemite card)
      int64_t        total_consumed; // total amount of Yosemite Tokens consumed for the real goods and services on a merchant shop
      int64_t        total_burned; // total burned amount of Yosemite Tokens by an account
      int64_t        last_issued; // the last issued amount of Yosemite Token by an account
      int64_t        available; // currently consumable(merchant) or burnable(merchant, fiat-backed) amount of Yosemite Token for an account

      uint64_t primary_key() const { return account; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( yosemite_token_issue_info, (account)(total_issued)(total_consumed)(total_burned)(last_issued)(available) )
   };

   typedef eosio::multi_index< N(ytokenissue), yosemite_token_issue_info > yosemite_token_issue_table_idx;

   struct yosemite_token_issue_stat {
      asset total_available;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( yosemite_token_issue_stat, (total_available) )
   };

   typedef eosio::singleton< N(ytokenstat), yosemite_token_issue_stat > yosemite_token_issue_stat_singleton;

   // YUSD-token can be created by converting fiat(USD)-backed Yosemite Token owned by the contract owner(yosemite card)
   // and can be redeemed to real fiat money through contract owner.
   struct yusd_token_balance_info {
      account_name    account;
      int64_t         yusd_token_balance;

      uint64_t primary_key() const { return account; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( yusd_token_balance_info, (account)(yusd_token_balance) )
   };

   typedef eosio::multi_index< N(yusdtoken), yusd_token_balance_info > yusd_token_table_idx;

   struct yusd_token_stat {
      asset total_yusd_token_supply;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( yusd_token_stat, (total_yusd_token_supply) )
   };

   typedef eosio::singleton< N(yusdstat), yusd_token_stat > yusd_token_stat_singleton;

} } /// namespace yosemite::contract
