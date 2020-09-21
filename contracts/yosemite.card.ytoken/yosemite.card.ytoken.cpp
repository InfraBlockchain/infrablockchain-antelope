/**
 *  @file contracts/yosemite.card.ytoken/yosemite.card.ytoken.cpp
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#include "yosemite.card.ytoken.hpp"

#include <infrablockchainlib/identity.hpp>
#include <infrablockchainlib/token_api.h>

#include <eosiolib/symbol.hpp>
#include <eosiolib/action.hpp>

#define YOSEMITE_YTOKEN_KYC_REQUIRED_AUTH (INFRABLOCKCHAIN_ID_KYC_EMAIL_AUTH | INFRABLOCKCHAIN_ID_KYC_REAL_NAME_AUTH | INFRABLOCKCHAIN_ID_KYC_BANK_ACCOUNT_AUTH)
#define YOSEMITE_CREDIT_TOKEN_KYC_REQUIRED_AUTH (INFRABLOCKCHAIN_ID_KYC_EMAIL_AUTH | INFRABLOCKCHAIN_ID_KYC_PHONE_AUTH | INFRABLOCKCHAIN_ID_KYC_REAL_NAME_AUTH | INFRABLOCKCHAIN_ID_KYC_BANK_ACCOUNT_AUTH)

#define YOSEMITE_TOKEN_USD_FIAT_BACKED N(usdbacked)
#define YOSEMITE_TOKEN_YCARD_USERS_POOL_ACCOUNT N(ycard.users)

namespace yosemitex { namespace contract {

   using namespace eosio;
   using namespace infrablockchain::identity;
   using std::string;

   /// @action ytokenissue
   void yosemite_card_ytoken::ytokenissue( const account_name merchant, const asset& qty /* YTOKEN */, const asset& paid /* USD or YUSD */, const string& tag ) {

      int64_t ytoken_amount = qty.amount;
      symbol_type ytoken_symbol = qty.symbol;
      int64_t fiat_paid_amount = paid.amount;
      symbol_type fiat_paid_symbol = paid.symbol;

      bool is_USD_paid = fiat_paid_symbol.value == S(4,USD);
      bool is_YUSD_paid = fiat_paid_symbol.value == S(4,YUSD);

      eosio_assert( merchant != YOSEMITE_TOKEN_USD_FIAT_BACKED, "fiatbacked is not allowed merchant account name" );
      eosio_assert( ytoken_amount > 0, "yosemite token amount must be positive value" );
      eosio_assert( ytoken_symbol.value == S(4,YTOKEN), "yosemite token symbol must be YTOKEN" );
      eosio_assert( fiat_paid_amount >= 0, "fiat token amount paid for the newly issued yosemite token must be zero or positive value" );
      eosio_assert( is_USD_paid || is_YUSD_paid, "paid fiat token symbol must be USD or YUSD" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );
      eosio_assert( ytoken_amount >= fiat_paid_amount, "paid fiat amount must be equal or less than yosemite token amount" );

      // needs signature of the merchant issuing yosemite tokens which are redeemable to goods and services in the merchant's store
      require_auth( merchant );
      // needs signature of the contract owner (yosemite card service)
      require_auth( _self );

      // issue Yosemite Token(YTOKEN) and update yosemite token issue table for merchant account
      issue_yosemite_token( merchant, ytoken_amount, true /*write_last_issued*/ );
      stat_add_ytoken_issue_total_available( ytoken_amount );

      if ( fiat_paid_amount > 0 ) {
         if ( is_USD_paid ) {
            // paid : USD
            SEND_INLINE_ACTION( *this, usdredeemto, {_self, N(active)}, {merchant, paid, tag} )
         }
         if ( is_YUSD_paid ) {
            // paid : YUSD
            SEND_INLINE_ACTION( *this, yusdtransfer, {_self, N(active)}, {_self, merchant, paid, tag} )
         }
      }

      // notify 'ytokenissue' action to merchant account
      require_recipient( merchant );
   }

   /// @action ytokenburn
   void yosemite_card_ytoken::ytokenburn( const account_name merchant, const asset& qty /* YTOKEN */, const string& tag ) {

      int64_t ytoken_amount = qty.amount;
      symbol_type ytoken_symbol = qty.symbol;

      eosio_assert( merchant != YOSEMITE_TOKEN_USD_FIAT_BACKED, "fiatbacked is not allowed merchant account name" );
      eosio_assert( ytoken_amount > 0, "yosemite token amount must be positive value" );
      eosio_assert( ytoken_symbol.value == S(4,YTOKEN), "yosemite token symbol must be YTOKEN" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );

      // needs signature of the merchant who issued gift-token backed Yosemite Tokens
      require_auth( merchant );
      // needs signature of the contract owner (yosemite card service)
      require_auth( _self );

      burn_yosemite_token( merchant, ytoken_amount, false /*is_consumed_by_customer*/ );
      stat_subtract_ytoken_issue_total_available( ytoken_amount );

      // notify 'ytokenissue' action to merchant account
      require_recipient( merchant );
   }

   /// @action ytokenredeem
   void yosemite_card_ytoken::ytokenredeem( const account_name account, const asset& qty /* YTOKEN */, const asset& redeemed /* USD or YUSD */, const string& tag ) {

      int64_t ytoken_amount = qty.amount;
      symbol_type ytoken_symbol = qty.symbol;
      int64_t fiat_token_redeemed_amount = redeemed.amount;
      symbol_type fiat_token_redeemed_symbol = redeemed.symbol;

      bool is_USD_redeemed = fiat_token_redeemed_symbol.value == S(4,USD);
      bool is_YUSD_redeemed = fiat_token_redeemed_symbol.value == S(4,YUSD);

      eosio_assert( account != _self, "contract owner is not allowed to use ytokenredeem action" );
      eosio_assert( ytoken_amount > 0, "yosemite token amount must be positive value" );
      eosio_assert( ytoken_symbol.value == S(4,YTOKEN), "yosemite token symbol must be YTOKEN" );
      eosio_assert( fiat_token_redeemed_amount >= 0, "fiat token amount must be zero or positive value" );
      eosio_assert( is_USD_redeemed || is_YUSD_redeemed, "fiat token symbol must be USD or YUSD" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );
      eosio_assert( ytoken_amount >= fiat_token_redeemed_amount, "redeemed fiat token amount must be equal or less than yosemite token amount" );

      // Both Yosemite-Token holder and contract owner (yosemite card) need to sign ytokenredeem action
      require_auth( account );
      require_auth( _self );

      // transfer Yosemite Token (YTOKEN) to contract owner account
      // (through YOSEMITE Standard Token API)
      transfer_token( account, _self, ytoken_amount );

      if ( fiat_token_redeemed_amount > 0 ) {
         if ( is_USD_redeemed ) {
            SEND_INLINE_ACTION( *this, usdredeemto, {_self, N(active)}, {account, redeemed, tag} )
         }
         if ( is_YUSD_redeemed ) {
            // convert Yosemite Token to YUSD-token owned by redeem requester
            SEND_INLINE_ACTION( *this, yusdtransfer, {_self, N(active)}, {_self, account, redeemed, tag} )
         }
      }
   }

   /// @action ytokenpay
   void yosemite_card_ytoken::ytokenpay( const account_name merchant, const asset& price /* YTOKEN */, const asset& credit /* CREDIT */, const asset& ytoken /* YTOKEN */, const asset& reward /* YTOKEN */, const string& tag ) {

      int64_t price_ytoken_amount = price.amount;
      symbol_type price_ytoken_symbol = price.symbol;
      int64_t customer_used_credit_token_amount = credit.amount;
      symbol_type customer_used_credit_token_symbol = credit.symbol;
      int64_t customer_used_ytoken_amount = ytoken.amount;
      symbol_type customer_used_ytoken_symbol = ytoken.symbol;
      int64_t customer_reward_ytoken_amount = reward.amount;
      symbol_type customer_reward_ytoken_symbol = reward.symbol;

      // Both merchant and contract owner (yosemite card) need to sign ytokenpay action
      require_auth( merchant );
      require_auth( _self );

      // price_ytoken_amount > 0 : payment action
      // price_ytoken_amount < 0 : cancel payment action

      eosio_assert( price_ytoken_amount != 0, "price yosemite token amount must be not zero" );

      eosio_assert( price_ytoken_symbol.value == S(4,YTOKEN), "price yosemite token symbol must be YTOKEN" );
      eosio_assert( customer_used_credit_token_symbol.value == S(4,CREDIT), "credit token symbol must be CREDIT" );
      eosio_assert( customer_used_ytoken_symbol.value == S(4,YTOKEN), "customer used yosemite token symbol must be YTOKEN" );
      eosio_assert( customer_reward_ytoken_symbol.value == S(4,YTOKEN), "customer reward yosemite token symbol must be YTOKEN" );
      eosio_assert( price_ytoken_amount == customer_used_credit_token_amount + customer_used_ytoken_amount, "price != credit + ytoken" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );

      if ( price_ytoken_amount > 0 ) {
         // normal payment transaction processing
         eosio_assert( customer_used_credit_token_amount >= 0, "credit token amount must be zero or positive value" );
         eosio_assert( customer_used_ytoken_amount >= 0, "customer used yosemite token amount must be zero or positive value" );
         eosio_assert( customer_reward_ytoken_amount >= 0, "customer reward yosemite token amount must be zero positive value" );

         // if customer uses his own Yosemite Tokens in this payment,
         // transfer Yosemite Token from ycard.users pool account to yosemite card service account
         // (through YOSEMITE Standard Token API)
         if ( customer_used_ytoken_amount > 0 ) {
            transfer_token( YOSEMITE_TOKEN_YCARD_USERS_POOL_ACCOUNT, _self, customer_used_ytoken_amount );
         }

         int64_t consumed_merchant_issued_ytoken_amount = 0;

         yosemite_token_issue_table_idx ytoken_issue_table( _self, _self );
         auto yosemite_token_issue_info_it = ytoken_issue_table.find( merchant );
         if ( yosemite_token_issue_info_it != ytoken_issue_table.end() ) {

            int64_t merchant_issued_ytoken_available = (*yosemite_token_issue_info_it).available;
            if ( merchant_issued_ytoken_available >= price_ytoken_amount ) {
               // consume merchant-issued Yosemite Tokens ( ytokenissue.merchant.availble (-), ytoken.contract_owner.balance (-) )
               burn_yosemite_token( merchant, price_ytoken_amount, true /*is_consumed_by_customer*/ );
               stat_subtract_ytoken_issue_total_available( price_ytoken_amount );
               consumed_merchant_issued_ytoken_amount = price_ytoken_amount;
            } else {
               // consume merchant-issued Yosemite Tokens
               burn_yosemite_token( merchant, merchant_issued_ytoken_available, true /*is_consumed_by_customer*/ );
               stat_subtract_ytoken_issue_total_available( merchant_issued_ytoken_available );
               consumed_merchant_issued_ytoken_amount = merchant_issued_ytoken_available;
            }
         }

         // if there is no merchant-issued Yosemite Token, or merchant-issued Yosemite Tokens are insufficient to be consumed,
         // transfer contract owner(yosemite card)'s Yosemite Token(fiat-backed, redeemable to fiat money) to merchant account.
         // (through YOSEMITE Standard Token API)
         if ( consumed_merchant_issued_ytoken_amount < price_ytoken_amount ) {
            transfer_token( _self, merchant, price_ytoken_amount - consumed_merchant_issued_ytoken_amount );
         }

         // transfer customer reward Yosemite Token amount to ycard.users pool account
         if ( customer_reward_ytoken_amount > 0 ) {
            transfer_token( _self, YOSEMITE_TOKEN_YCARD_USERS_POOL_ACCOUNT, customer_reward_ytoken_amount );
         }

         // burn used CREDIT token amount
         if ( customer_used_credit_token_amount > 0 ) {
            SEND_INLINE_ACTION( *this, creditburn, {_self, N(active)}, {credit, tag} )
         }

      } else {
         ////////////////////////////////////////////////////
         /// cancel-payment transaction processing
         /// price_ytoken_amount < 0

         eosio_assert( customer_used_credit_token_amount <= 0, "credit token amount for cancel-payment must be zero or negative value" );
         eosio_assert( customer_used_ytoken_amount <= 0, "customer used yosemite token amount for cancel-payment must be zero or negative value" );
         eosio_assert( customer_reward_ytoken_amount <= 0, "customer reward yosemite token amount for cancel-payment must be zero negative value" );

         // rollback burned CREDIT token amount
         if ( customer_used_credit_token_amount < 0 ) {
            asset credit_to_rollback(-customer_used_credit_token_amount,S(4,CREDIT));
            SEND_INLINE_ACTION( *this, creditissue, {_self, N(active)}, {_self, _self, credit_to_rollback, tag} )
         }

         // rollback customer reward Yosemite Token transfer
         if (customer_reward_ytoken_amount < 0 ) {
            transfer_token( YOSEMITE_TOKEN_YCARD_USERS_POOL_ACCOUNT, _self, -customer_reward_ytoken_amount );
         }

         int64_t rollbacked_from_merchant_ytoken_balance = 0;
         int64_t merchant_ytoken_balance = get_token_balance( _self, merchant );
         if ( merchant_ytoken_balance > 0 ) {
            if ( merchant_ytoken_balance <= -price_ytoken_amount ) {
               rollbacked_from_merchant_ytoken_balance = merchant_ytoken_balance;
            } else {
               rollbacked_from_merchant_ytoken_balance = -price_ytoken_amount;
            }
            transfer_token( merchant, _self, rollbacked_from_merchant_ytoken_balance );
         }

         if ( rollbacked_from_merchant_ytoken_balance < -price_ytoken_amount ) {
            yosemite_token_issue_table_idx ytoken_issue_table( _self, _self );
            auto yosemite_token_issue_info_it = ytoken_issue_table.find( merchant );
            eosio_assert( yosemite_token_issue_info_it != ytoken_issue_table.end(), "cancel payment failed. insufficient yosemite tokens to be roll-backed" );

            // rollback consumed merchant-issued ytoken 'available' balance
            int64_t ytoken_issue_available_amount_to_recover = -price_ytoken_amount - rollbacked_from_merchant_ytoken_balance;
            ytoken_issue_table.modify( yosemite_token_issue_info_it, 0, [&](yosemite_token_issue_info& info) {
               info.total_consumed -= ytoken_issue_available_amount_to_recover;
               info.available += ytoken_issue_available_amount_to_recover;
            });

            // rollback burned(consumed) Yosemite Token balance
            issue_token( _self, ytoken_issue_available_amount_to_recover );

            stat_add_ytoken_issue_total_available( ytoken_issue_available_amount_to_recover );
         }

         // rollback customer-used ytoken amount
         if ( customer_used_ytoken_amount < 0 ) {
            transfer_token( _self, YOSEMITE_TOKEN_YCARD_USERS_POOL_ACCOUNT, -customer_used_ytoken_amount );
         }
      }

      // notify 'ytokenpay' action to merchant account
      require_recipient( merchant );
   }

   /// @action usdytissue
   void yosemite_card_ytoken::usdytissue( const asset& qty /* USD */, const string& tag ) {

      int64_t token_amount = qty.amount;
      symbol_type fiat_token_symbol = qty.symbol;

      eosio_assert( fiat_token_symbol.value == S(4,USD), "symbol of quantity must be USD" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );
      eosio_assert( qty.is_valid(), "invalid fiat token quantity" );
      eosio_assert( token_amount > 0, "must issue positive yosemite token quantity" );

      // only the contract owner account can issue fiat-backed Yosemite Token
      require_auth( _self );

      // issue USD fiat-backed Yosemite Token
      issue_yosemite_token( YOSEMITE_TOKEN_USD_FIAT_BACKED, token_amount, false /*write_last_issued*/ );
      stat_add_ytoken_issue_total_available( token_amount );
   }

   /// @action usdredeemto
   void yosemite_card_ytoken::usdredeemto( const account_name to, const asset& qty /* USD or YUSD */, const string& tag ) {

      int64_t fiat_token_amount = qty.amount;
      symbol_type fiat_token_symbol = qty.symbol;

      bool is_USD_redeem = fiat_token_symbol.value == S(4,USD);
      bool is_YUSD_redeem = fiat_token_symbol.value == S(4,YUSD);

      eosio_assert( is_USD_redeem || is_YUSD_redeem, "fiat token symbol must be USD or YUSD" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );
      eosio_assert( qty.is_valid(), "invalid fiat token quantity" );
      eosio_assert( fiat_token_amount > 0, "fiat token quantity must be positive" );

      if ( is_YUSD_redeem ) {
         eosio_assert( to != _self, "YUSD redeem is not allowed for contract owner" );
      }

      // only the contract owner (yosemite card) can redeem YUSD-token or fiat-backed Yosemite Token to real fiat money
      require_auth( _self );

      if ( is_USD_redeem ) {
         // redeem fiat(USD)-backed Yosemite Tokens to real fiat money
         burn_yosemite_token( YOSEMITE_TOKEN_USD_FIAT_BACKED, fiat_token_amount, false /*is_consumed_by_customer*/ );
         stat_subtract_ytoken_issue_total_available( fiat_token_amount );
      }

      if ( is_YUSD_redeem ) {
         // burn the redeem-requested YUSD-tokens held by contract owner
         subtract_yusd_token_balance( _self, fiat_token_amount );
         stat_subtract_yusd_token_total_supply( fiat_token_amount );
      }

      // notify 'usdredeemto' action to account who received redeemed fiat money
      require_recipient( to );
   }

   /// @action yusdtransfer
   void yosemite_card_ytoken::yusdtransfer( const account_name from, const account_name to, const asset& qty /* YUSD */, const string& tag ) {

      int64_t yusd_token_amount = qty.amount;
      symbol_type yusd_token_symbol = qty.symbol;

      eosio_assert( from != to, "cannot transfer to self" );
      eosio_assert( yusd_token_symbol.value == S(4,YUSD), "fiat token symbol must be YUSD" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );
      eosio_assert( qty.is_valid(), "invalid fiat token quantity" );
      eosio_assert( yusd_token_amount > 0, "must transfer positive fiat token quantity" );

      eosio_assert( has_all_kyc_status( from, YOSEMITE_YTOKEN_KYC_REQUIRED_AUTH ), "transferfiat.from account failed to satisfy KYC constraints" );
      eosio_assert( has_all_kyc_status( to, YOSEMITE_YTOKEN_KYC_REQUIRED_AUTH ), "transferfiat.to account failed to satisfy KYC constraints" );

      // 'from' account needs to sign
      require_auth( from );

      yusd_token_table_idx yusd_token_table( _self, _self );

      // if YUSD-tokens are transferred from/to the contract owner account (yosemite card),
      // YUSD-tokens are automatically converted to the fiat-backed Yosemite Tokens (YTOKEN)

      if ( from == _self ) {
         // convert contract owner(yosemite card)'s fiat-backed Yosemite Token to YUSD-token,
         // and transfer the converted YUSD-token
         burn_yosemite_token( YOSEMITE_TOKEN_USD_FIAT_BACKED, yusd_token_amount, false /*is_consumed_by_customer*/ );
         stat_subtract_ytoken_issue_total_available( yusd_token_amount );
         stat_add_yusd_token_total_supply( yusd_token_amount );
      } else {
         // fiat token ledger table update for 'from' account
         subtract_yusd_token_balance( from, yusd_token_amount );
      }

      if ( to == _self ) {
         // convert the received YUSD-token to contract owner(yosemite card)'s fiat-backed Yosemite Token
         issue_yosemite_token( YOSEMITE_TOKEN_USD_FIAT_BACKED, yusd_token_amount, false /*write_last_issued*/ );
         stat_add_ytoken_issue_total_available( yusd_token_amount );
         // the converted YUSD-tokens are burned
         stat_subtract_yusd_token_total_supply( yusd_token_amount );
      } else {
         // fiat token ledger table update for 'to' account
         add_yusd_token_balance( to, yusd_token_amount );
      }

      // notify 'transferfiat' action to 'from' and 'to' accounts
      require_recipient( from );
      require_recipient( to );
   }

   /// @action yusdredeemrq
   void yosemite_card_ytoken::yusdredeemrq( const account_name account, const asset& qty /* YUSD */, const string& tag ) {

      int64_t yusd_token_amount = qty.amount;
      symbol_type yusd_token_symbol = qty.symbol;

      eosio_assert( account != _self, "contract owner is not allowed to send redeemfiatrq action" );
      eosio_assert( yusd_token_symbol.value == S(4,YUSD), "fiat token symbol must be YUSD" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );
      eosio_assert( qty.is_valid(), "invalid fiat token quantity" );
      eosio_assert( yusd_token_amount > 0, "must redeem-request positive fiat token quantity" );

      eosio_assert( has_all_kyc_status( account, YOSEMITE_YTOKEN_KYC_REQUIRED_AUTH ), "redeem-YUSD-token requester account failed to satisfy KYC constraints" );

      // account who requests fiat token redeem needs to sign transaction
      require_auth( account );

      // move fiat token balance from the requested account to contract owner (yosemite card) account
      // fiat token balance of contract owner is only burnable by contract owner
      // when the redeemed real fiat money is deposited to the YUSD-token redeem requester
      subtract_yusd_token_balance( account, yusd_token_amount );
      add_yusd_token_balance( _self, yusd_token_amount );

      // notify 'redeemfiatrq' action to requester account
      require_recipient( account );
   }

   /// @action yusdcnclrdrq
   void yosemite_card_ytoken::cnclyusdrdrq( const account_name account, const asset& qty /* YUSD */, const string& tag ) {

      int64_t yusd_token_amount = qty.amount;
      symbol_type yusd_token_symbol = qty.symbol;

      eosio_assert( account != _self, "contract owner is not allowed to send redeemfiatrq and cnclrdfiatrq action" );
      eosio_assert( yusd_token_symbol.value == S(4,YUSD), "fiat token symbol must be YUSD" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );
      eosio_assert( qty.is_valid(), "invalid fiat token quantity" );
      eosio_assert( yusd_token_amount > 0, "must cancel-redeem-request positive fiat token quantity" );

      // only the contract owner (yosemite card) can cancel redeem-fiat request
      require_auth( _self );

      // return the requested YUSD-token amount to the redeem request
      subtract_yusd_token_balance( _self, yusd_token_amount );
      add_yusd_token_balance( account, yusd_token_amount );

      // notify 'cnclrdfiatrq' action to requester account
      require_recipient( account );
   }

   /// @action creditlimit
   void yosemite_card_ytoken::creditlimit( const account_name account, const asset& credit_limit, const string& tag ) {

      int64_t credit_limit_amount = credit_limit.amount;
      symbol_type credit_limit_symbol = credit_limit.symbol;

      eosio_assert( credit_limit_symbol.value == S(4,CREDIT), "credit token symbol must be CREDIT" );
      eosio_assert( credit_limit_amount >= 0, "credit limit amount must be zero or positive value" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );

      eosio_assert( has_all_kyc_status( account, YOSEMITE_CREDIT_TOKEN_KYC_REQUIRED_AUTH ), "credit-limit target account failed to satisfy KYC constraints" );

      // only the contract owner (yosemite card) can set user account's credit limit
      require_auth( _self );

      credit_offering_table_idx credit_offering_table( _self, _self );

      auto credit_offering_info_it = credit_offering_table.find( account );
      if ( credit_offering_info_it == credit_offering_table.end() ) {
         eosio_assert( credit_limit_amount > 0, "credit limit must be positive value" );
         credit_offering_table.emplace( _self, [&](credit_offering_info& info) {
            info.account = account;
            info.credit_limit = credit_limit_amount;
            info.credit_issued = 0;
         });
         stat_add_credit_token_total_offered_credit_limit( credit_limit_amount );
      } else {
         auto current_info = *credit_offering_info_it;
         eosio_assert( credit_limit_amount >= current_info.credit_issued, "credit-limit less than credit-issued not allowed" );
         if ( credit_limit_amount  == 0 ) {
            credit_offering_table.erase( credit_offering_info_it );
         } else {
            credit_offering_table.modify( credit_offering_info_it, 0, [&](credit_offering_info& info) {
               info.credit_limit = credit_limit_amount;
            });
         }
         int64_t current_credit_limit = current_info.credit_limit;
         if ( current_credit_limit < credit_limit_amount ) {
            stat_add_credit_token_total_offered_credit_limit( credit_limit_amount - current_credit_limit );
         } else if ( current_credit_limit > credit_limit_amount ) {
            stat_subtract_credit_token_total_offered_credit_limit( current_credit_limit - credit_limit_amount );
         }
      }

      // notify 'creditlimit' action to the target account
      require_recipient( account );
   }

   /// @action creditissue
   void yosemite_card_ytoken::creditissue( const account_name issuer, const account_name to, const asset& qty, const string& tag ) {

      int64_t credit_token_issue_amount = qty.amount;
      symbol_type credit_token_symbol = qty.symbol;

      eosio_assert( credit_token_symbol.value == S(4,CREDIT), "credit token symbol must be CREDIT" );
      eosio_assert( credit_token_issue_amount > 0, "credit issue amount must be positive value" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );

      eosio_assert( has_all_kyc_status( issuer, YOSEMITE_CREDIT_TOKEN_KYC_REQUIRED_AUTH ), "credit-issue issuer failed to satisfy KYC constraints" );

      // account who is trying to issue new token needs to sign
      require_auth( issuer );

      if ( issuer != _self ) {
         credit_offering_table_idx credit_offering_table( _self, _self );

         auto& credit_info = credit_offering_table.get( issuer );
         eosio_assert( credit_info.credit_limit - credit_info.credit_issued >= credit_token_issue_amount, "newly issued credit token amount exceeds the issuer's credit limit" );

         credit_offering_table.modify( credit_info, 0, [&](credit_offering_info& info) {
            info.credit_issued += credit_token_issue_amount;
         });
      }

      // issue new credit tokens to the issuer
      add_credit_token_balance( issuer, credit_token_issue_amount );

      // update credit token stat
      stat_add_credit_token_total_supply( credit_token_issue_amount );

      // notify 'creditissue' action to the issuer account
      require_recipient( issuer );

      if ( issuer != to ) {
         SEND_INLINE_ACTION( *this, credittxfer, {issuer, N(codecrdtxfer)}, {issuer, to, qty, tag} )
      }
   }

   /// @action credittxfer
   void yosemite_card_ytoken::credittxfer( const account_name from, const account_name to, const asset& qty /* CREDIT */, const string& tag ) {

      int64_t credit_token_amount = qty.amount;
      symbol_type credit_token_symbol = qty.symbol;

      eosio_assert( from != to, "cannot transfer credit token to self" );
      eosio_assert( credit_token_symbol.value == S(4,CREDIT), "credit token symbol must be CREDIT" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );
      eosio_assert( qty.is_valid(), "invalid credit token quantity" );
      eosio_assert( credit_token_amount > 0, "must transfer positive credit token quantity" );

      eosio_assert( has_all_kyc_status( from, YOSEMITE_CREDIT_TOKEN_KYC_REQUIRED_AUTH ), "credittxfer.from account failed to satisfy KYC constraints" );
      eosio_assert( has_all_kyc_status( to, YOSEMITE_CREDIT_TOKEN_KYC_REQUIRED_AUTH ), "credittxfer.to account failed to satisfy KYC constraints" );

      // 'from' account needs to sign
      require_auth( from );

      subtract_credit_token_balance( from, credit_token_amount );
      add_credit_token_balance( to, credit_token_amount );

      // notify 'credittxfer' action to 'from' and 'to' accounts
      require_recipient( from );
      require_recipient( to );
   }

   /// @action creditsettle
   void yosemite_card_ytoken::creditsettle( const account_name account, const asset& qty, const string& tag ) {

      int64_t credit_settle_amount = qty.amount;
      symbol_type credit_token_symbol = qty.symbol;

      eosio_assert( credit_token_symbol.value == S(4,CREDIT), "credit token symbol must be CREDIT" );
      eosio_assert( credit_settle_amount > 0, "credit settle amount must be positive value" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );

      // only the contract owner (yosemite card) can change user account's credit-issued amount
      require_auth( _self );

      credit_offering_table_idx credit_offering_table( _self, _self );

      auto& credit_info = credit_offering_table.get( account );
      eosio_assert( credit_settle_amount <= credit_info.credit_issued, "credit-settle amount must be less than or equal to current credit-issued amount" );

      credit_offering_table.modify( credit_info, 0, [&](credit_offering_info& info) {
         info.credit_issued -= credit_settle_amount;
      });

      // notify 'creditsettle' action to the target account
      require_recipient( account );
   }

   /// @action creditburn
   void yosemite_card_ytoken::creditburn( const asset& qty /* CREDIT */, const string& tag ) {

      int64_t credit_token_burn_amount = qty.amount;
      symbol_type credit_token_symbol = qty.symbol;

      eosio_assert( credit_token_symbol.value == S(4,CREDIT), "credit token symbol must be CREDIT" );
      eosio_assert( credit_token_burn_amount > 0, "credit token burn amount must be positive value" );
      eosio_assert( tag.size() <= 256, "too long tag string, max tag string size is 256 bytes" );

      // only the contract owner (yosemite card) can burn credit tokens owned by itself
      require_auth( _self );

      // burn credit tokens
      subtract_credit_token_balance( _self, credit_token_burn_amount );

      // update credit token stat
      stat_subtract_credit_token_total_supply( credit_token_burn_amount );
   }

   ///////////////////////////////////////////////////////
   /// YOSEMITE Standard Token Actions

   /// @action issue - inherited YOSEMITE Standard Token Action
   void yosemite_card_ytoken::issue( const account_name /*t*/, const account_name /*to*/, const asset& /*qty*/, const string& /*tag*/ ) {

      /// built-in action handler for 'issue' standard token action has already processed basic 'issue' operations

      eosio_assert( false, "use ytokenissue, issuefiatyt actions" );
      //eosio_assert( has_all_kyc_status( to, YOSEMITE_YTOKEN_KYC_REQUIRED_AUTH ), "issue.to account failed to satisfy KYC constraints" );
   }

   /// @action transfer - inherited YOSEMITE Standard Token Action
   void yosemite_card_ytoken::transfer( const account_name /*t*/, const account_name from, const account_name to, const asset& /*qty*/, const string& /*tag*/ ) {

      /// built-in action handler for 'transfer' standard token action has already processed basic 'transfer' operations

      eosio_assert( has_all_kyc_status( from, YOSEMITE_YTOKEN_KYC_REQUIRED_AUTH ), "transfer.from account failed to satisfy KYC constraints" );
      eosio_assert( has_all_kyc_status( to, YOSEMITE_YTOKEN_KYC_REQUIRED_AUTH ), "transfer.to account failed to satisfy KYC constraints" );
   }

   /// @action txfee - inherited YOSEMITE Standard Token Action
   void yosemite_card_ytoken::txfee( const account_name /*t*/, const account_name payer, const asset& /*fee*/ ) {

      /// built-in action handler for 'txfee' standard token action has already processed basic 'txfee' operations

      // Yosemite Token (YTOKEN) can be used as a transaction fee token if selected by block producers
      eosio_assert( has_all_kyc_status( payer, YOSEMITE_YTOKEN_KYC_REQUIRED_AUTH ), "transfer.to account failed to satisfy KYC constraints" );
   }

   void yosemite_card_ytoken::redeem( const asset& /*qty*/, const string& /*tag*/ ) {
      eosio_assert( false, "use ytokenredeem, redeemfiatrq, cnclrdfiatrq, redeemedfiat actions" );
   }


   //////////////////////////////////////////////////////
   /// private utility function implementations

   void yosemite_card_ytoken::issue_yosemite_token( const account_name ytoken_issuer, const int64_t ytoken_amount, bool write_last_issued ) {

      // issue Yosemite Token (YTOKEN) to contract owner (yosemite card) (through YOSEMITE Standard Token API)
      issue_token( _self, ytoken_amount );

      yosemite_token_issue_table_idx ytoken_issue_table( _self, _self );

      // yosemite token issue table update
      auto yosemite_token_issue_info_it = ytoken_issue_table.find( ytoken_issuer );
      if ( yosemite_token_issue_info_it == ytoken_issue_table.end() ) {
         ytoken_issue_table.emplace( _self, [&](yosemite_token_issue_info& info) {
            info.account = ytoken_issuer;
            info.total_issued = ytoken_amount;
            info.total_consumed = 0;
            info.total_burned = 0;
            if ( write_last_issued ) {
               info.last_issued = ytoken_amount;
            } else {
               info.last_issued = 0;
            }
            info.available = ytoken_amount;
         });
      } else {
         ytoken_issue_table.modify( yosemite_token_issue_info_it, 0, [&](yosemite_token_issue_info& info) {
            info.total_issued += ytoken_amount;
            if ( write_last_issued ) {
               info.last_issued = ytoken_amount;
            } else {
               info.last_issued = 0;
            }
            info.available += ytoken_amount;
         });
      }
   }

   void yosemite_card_ytoken::burn_yosemite_token( const account_name ytoken_issuer, const int64_t ytoken_amount, bool is_consumed_by_customer ) {

      // burn Yosemite Token (YTOKEN) balance owned by contract owner (yosemite card) (through YOSEMITE Standard Token API)
      redeem_token( ytoken_amount );

      yosemite_token_issue_table_idx ytoken_issue_table( _self, _self );
      auto yosemite_token_issue_info_it = ytoken_issue_table.find( ytoken_issuer );
      eosio_assert ( yosemite_token_issue_info_it != ytoken_issue_table.end(), "insufficient available yosemite token amount to burn, no yosemite token issue info" );
      auto current_fiat_backed_ytoken_issue_info = *yosemite_token_issue_info_it;
      eosio_assert( current_fiat_backed_ytoken_issue_info.available >= ytoken_amount, "insufficient available yosemite token amount to burn" );
      ytoken_issue_table.modify( yosemite_token_issue_info_it, 0, [&](yosemite_token_issue_info& info) {
         if ( is_consumed_by_customer ) {
            info.total_consumed += ytoken_amount;
         } else {
            info.total_burned += ytoken_amount;
         }
         info.available -= ytoken_amount;
      });
   }

   void yosemite_card_ytoken::stat_add_ytoken_issue_total_available( const int64_t ytoken_amount ) {

      yosemite_token_issue_stat_singleton ytoken_issue_stat_singleton( _self, _self );
      yosemite_token_issue_stat ytoken_issue_stat = ytoken_issue_stat_singleton.get_or_default({asset(0,S(4,YTOKEN))});
      ytoken_issue_stat.total_available.amount += ytoken_amount;
      ytoken_issue_stat_singleton.set( ytoken_issue_stat, _self );
   }

   void yosemite_card_ytoken::stat_subtract_ytoken_issue_total_available( const int64_t ytoken_amount ) {

      yosemite_token_issue_stat_singleton ytoken_issue_stat_singleton( _self, _self );
      eosio_assert( ytoken_issue_stat_singleton.exists(), "crashed ytoken_issue_stat state, no ytoken_issue_stat_singleton" );
      yosemite_token_issue_stat ytoken_issue_stat = ytoken_issue_stat_singleton.get();
      eosio_assert( ytoken_issue_stat.total_available.amount >= ytoken_amount, "crashed ytoken_issue_stat state" );
      ytoken_issue_stat.total_available.amount -= ytoken_amount;
      ytoken_issue_stat_singleton.set( ytoken_issue_stat, _self );
   }

   void yosemite_card_ytoken::add_yusd_token_balance( const account_name account, const int64_t yusd_token_amount ) {

      yusd_token_table_idx yusd_token_table( _self, _self );
      auto yusd_token_info_it = yusd_token_table.find( account );
      if ( yusd_token_info_it == yusd_token_table.end() ) {
         yusd_token_table.emplace( _self, [&](yusd_token_balance_info& info) {
            info.account = account;
            info.yusd_token_balance = yusd_token_amount;
         });
      } else {
         yusd_token_table.modify( yusd_token_info_it, 0, [&](yusd_token_balance_info& info) {
            info.yusd_token_balance += yusd_token_amount;
         });
      }
   }

   void yosemite_card_ytoken::subtract_yusd_token_balance( const account_name account, const int64_t yusd_token_amount ) {

      yusd_token_table_idx yusd_token_table( _self, _self );
      auto yusd_token_info_it = yusd_token_table.find( account );
      eosio_assert ( yusd_token_info_it != yusd_token_table.end(), "account has insufficient YUSD-token, no fiat token info" );
      auto yusd_token_info = *yusd_token_info_it;
      eosio_assert( yusd_token_info.yusd_token_balance >= yusd_token_amount, "account has insufficient YUSD-token" );
      yusd_token_table.modify( yusd_token_info_it, 0, [&](yusd_token_balance_info& info) {
         info.yusd_token_balance -= yusd_token_amount;
      });
   }

   void yosemite_card_ytoken::stat_add_yusd_token_total_supply( const int64_t yusd_token_amount ) {

      yusd_token_stat_singleton yusd_stat_singleton( _self, _self );
      yusd_token_stat yusd_stat = yusd_stat_singleton.get_or_default({asset(0,S(4,YUSD))});
      yusd_stat.total_yusd_token_supply.amount += yusd_token_amount;
      yusd_stat_singleton.set( yusd_stat, _self );
   }

   void yosemite_card_ytoken::stat_subtract_yusd_token_total_supply( const int64_t yusd_token_amount ) {

      yusd_token_stat_singleton yusd_stat_singleton( _self, _self );
      eosio_assert( yusd_stat_singleton.exists(), "crashed yusd_token_stat state, no yusd_token_stat_singleton" );
      yusd_token_stat yusd_stat = yusd_stat_singleton.get();
      eosio_assert( yusd_stat.total_yusd_token_supply.amount >= yusd_token_amount, "crashed yusd_token_stat state" );
      yusd_stat.total_yusd_token_supply.amount -= yusd_token_amount;
      yusd_stat_singleton.set( yusd_stat, _self );
   }

   void yosemite_card_ytoken::add_credit_token_balance( const account_name account, const int64_t credit_token_amount ) {

      credit_token_table_idx credit_token_table( _self, _self );
      auto credit_token_info_it = credit_token_table.find( account );
      if ( credit_token_info_it == credit_token_table.end() ) {
         credit_token_table.emplace( _self, [&](credit_token_balance_info& info) {
            info.account = account;
            info.credit_token_balance = credit_token_amount;
         });
      } else {
         credit_token_table.modify( credit_token_info_it, 0, [&](credit_token_balance_info& info) {
            info.credit_token_balance += credit_token_amount;
         });
      }
   }

   void yosemite_card_ytoken::subtract_credit_token_balance( const account_name account, const int64_t credit_token_amount ) {

      credit_token_table_idx credit_token_table( _self, _self );
      auto credit_token_info_it = credit_token_table.find( account );
      eosio_assert ( credit_token_info_it != credit_token_table.end(), "account has insufficient credit-token, no credit token info" );
      auto credit_token_info = *credit_token_info_it;
      eosio_assert( credit_token_info.credit_token_balance >= credit_token_amount, "account has insufficient credit-token" );
      credit_token_table.modify( credit_token_info_it, 0, [&](credit_token_balance_info& info) {
         info.credit_token_balance -= credit_token_amount;
      });
   }

   void yosemite_card_ytoken::stat_add_credit_token_total_offered_credit_limit( const int64_t credit_token_amount ) {

      credit_token_stat_singleton credit_stat_singleton( _self, _self );
      credit_token_stat credit_stat = credit_stat_singleton.get_or_default({asset(0,S(4,CREDIT)),asset(0,S(4,CREDIT))});
      credit_stat.total_offered_credit_limit.amount += credit_token_amount;
      credit_stat_singleton.set( credit_stat, _self );
   }

   void yosemite_card_ytoken::stat_subtract_credit_token_total_offered_credit_limit( const int64_t credit_token_amount ) {

      credit_token_stat_singleton credit_stat_singleton( _self, _self );
      eosio_assert( credit_stat_singleton.exists(), "crashed credit_token_stat state, no credit_token_stat_singleton" );
      credit_token_stat credit_stat = credit_stat_singleton.get();
      eosio_assert( credit_stat.total_offered_credit_limit.amount >= credit_token_amount, "crashed credit_token_stat state, total_offered_credit_limit" );
      credit_stat.total_offered_credit_limit.amount -= credit_token_amount;
      credit_stat_singleton.set( credit_stat, _self );
   }

   void yosemite_card_ytoken::stat_add_credit_token_total_supply( const int64_t credit_token_amount ) {

      credit_token_stat_singleton credit_stat_singleton( _self, _self );
      credit_token_stat credit_stat = credit_stat_singleton.get_or_default({asset(0,S(4,CREDIT)),asset(0,S(4,CREDIT))});
      credit_stat.total_credit_token_supply.amount += credit_token_amount;
      credit_stat_singleton.set( credit_stat, _self );
   }

   void yosemite_card_ytoken::stat_subtract_credit_token_total_supply( const int64_t credit_token_amount ) {

      credit_token_stat_singleton credit_stat_singleton( _self, _self );
      eosio_assert( credit_stat_singleton.exists(), "crashed credit_token_stat state, no credit_token_stat_singleton" );
      credit_token_stat credit_stat = credit_stat_singleton.get();
      eosio_assert( credit_stat.total_credit_token_supply.amount >= credit_token_amount, "crashed credit_token_stat state, total_credit_token_supply" );
      credit_stat.total_credit_token_supply.amount -= credit_token_amount;
      credit_stat_singleton.set( credit_stat, _self );
   }

} } /// namespace yosemitex::contract

EOSIO_ABI( yosemitex::contract::yosemite_card_ytoken,
           (ytokenissue)(ytokenburn)(ytokenredeem)(ytokenpay)
           (usdytissue)(usdredeemto)
           (yusdtransfer)(yusdredeemrq)(cnclyusdrdrq)
           (creditlimit)(creditissue)(credittxfer)(creditsettle)(creditburn)
           (issue)(transfer)(txfee)(redeem) )
