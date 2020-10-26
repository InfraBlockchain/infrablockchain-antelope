#include "allcamper.contract.hpp"

#include <eosiolib/symbol.hpp>
#include <eosiolib/action.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/system.h>
#include <eosiolib/print.h>

#include "../allcamper.akrw.token/allcamper.akrw.token.hpp"

namespace allcamper {

   class akrw_token_interface {
   public:
      void transfer( const account_name /*t*/, const account_name /*from*/, const account_name /*to*/, const asset /*qty*/, const string /*tag*/ ) {
      }
   };

   using namespace eosio;
   using std::string;

   /**
    * token receiving handler called when AllCamper stable token (AKRW) is transferred to this contract account
    *
    * @param t - token id (token account name)
    * @param from - account name sending tokens
    * @param to - account name receiving tokens
    * @param qty - token quantity (amount, symbol) to transfer
    * @param tag - user tag string to identify a specific transfer action (application-specific purpose)
    */
   void allcamper_contract::receive_token( const account_name t, const account_name from, const account_name to, const asset& qty, const string& tag ) {

      if ( from != _self && to == _self && t == ALLCAMPER_AKRW_TOKEN_CONTRACT
           && qty.symbol.value == ALLCAMPER_AKRW_TOKEN_SYMBOL && qty.amount > 0) {
         add_on_contract_akrw_token_balance( from, qty );
      }
   }

   //////////////////////////////////////////////////////
   /// Contract Action Implementations

   /**
    * [[action]] Register AllCamper User Account
    *
    * @param account - AllCamper user's blockchain account (signature required)
    * @param user_info_multihash - ipfs multihash(base58) of user info json file
    */
   void allcamper_contract::reguser( const account_name account, const string& user_info_multihash ) {

      allcamper_account_table_idx account_table( _self, _self );
      eosio_assert( account_table.find( account ) == account_table.end(), "account already exists" );

      // needs signature of allcamper user
      require_auth( account );

      account_table.emplace( _self, [&](allcamper_account& a) {
         a.account = account;
         a.deposited_akrw_bal = asset(0, ALLCAMPER_AKRW_TOKEN_SYMBOL);
         a.user_info_multihash = user_info_multihash;
      });
   }


   /**
    * [[action]] Update AllCamper User Info
    *
    * @param account - AllCamper user's blockchain account (signature required)
    * @param user_info_multihash - ipfs multihash(base58) of user info json file
    */
   void allcamper_contract::updateuser( const account_name account, const string& user_info_multihash ) {

      allcamper_account_table_idx account_table( _self, _self );
      auto account_info_it = account_table.find( account );
      eosio_assert( account_info_it != account_table.end(), "no existing account" );

      // needs signature of allcamper user
      require_auth( account );

      account_table.modify( account_info_it, 0, [&](allcamper_account& a) {
         a.user_info_multihash = user_info_multihash;
      });
   }


   /**
    * [[action]] Register Camping Car
    *
    * @param car_id - camping car id
    * @param owner - campping owner's blockchain account (signature required)
    * @param car_info_multihash - ipfs multihash(base58) of car info json file
    */
   void allcamper_contract::regcampcar( const uint64_t car_id, const account_name owner, const string& car_info_multihash ) {

      camping_car_table_idx camping_car_table( _self, _self );
      eosio_assert( camping_car_table.find( car_id ) == camping_car_table.end(), "camping car id already exists" );

      // needs signature of camping car owner
      require_auth( owner );

      camping_car_table.emplace( _self, [&](camping_car& c) {
         c.car_id = car_id;
         c.owner = owner;
         c.status = CAMPING_CAR_STATUS_AVAILABLE;
         c.review_score = 0;
         c.review_count = 0;
         c.last_review_at = 0;
         c.car_info_multihash = car_info_multihash;
      });
   }


   /**
    * [[action]] Update Camping Car Info
    *
    * @param car_id - camping car id
    * @param owner - campping owner's blockchain account (signature required)
    * @param car_info_multihash - ipfs multihash(base58) of car info json file
    */
   void allcamper_contract::updatecar( const uint64_t car_id, const account_name owner, const string& car_info_multihash ) {

      camping_car_table_idx camping_car_table( _self, _self );
      auto camping_car_info_it = camping_car_table.find( car_id );
      eosio_assert( camping_car_info_it != camping_car_table.end(), "no existing camping car item matching car_id" );

      // needs signature of camping car owner
      require_auth( owner );

      camping_car_table.modify( camping_car_info_it, 0, [&](camping_car& c) {
         c.car_info_multihash = car_info_multihash;
      });
   }


   /**
    * [[action]] Reserve Camping Car Trade
    *
    * @param deal_id - deal id of camping car trade
    * @param car_id - car id to trade
    * @param buyer - camping car buyer blockchain account (signature required)
    * @param price - camping car trade price (AKRW token)
    */
   void allcamper_contract::rsvcartrade( const uint64_t deal_id, const uint64_t car_id, const account_name buyer, const asset& price ) {

      camping_car_trade_table_idx camping_car_trade_table( _self, _self );
      eosio_assert( camping_car_trade_table.find( deal_id ) == camping_car_trade_table.end(), "duplicated deal_id" );

      camping_car_table_idx camping_car_table( _self, _self );
      auto camping_car_info_it = camping_car_table.find( car_id );
      eosio_assert( camping_car_info_it != camping_car_table.end(), "no existing camping car item matching car_id" );

      auto camping_car_info = *camping_car_info_it;
      eosio_assert( camping_car_info.status == CAMPING_CAR_STATUS_AVAILABLE, "camping car is not available for trading" );

      // needs signature of buyer
      require_auth( buyer );

      // lock buyer's token balance
      subtract_on_contract_akrw_token_balance( buyer, price );

      camping_car_trade_table.emplace( _self, [&](camping_car_trade& t) {
         t.deal_id = deal_id;
         t.car_id = car_id;
         t.seller_account = camping_car_info.owner;
         t.buyer_account = buyer;
         t.price = price;
         t.status = CAMPING_CAR_TRADE_STATUS_RESERVED;
         t.reserved_at = current_time();
         t.confirmed_at = 0;
      });

      camping_car_table.modify( camping_car_info_it, 0, [&](camping_car& c) {
         c.status = CAMPING_CAR_STATUS_TRADING;
      });
   }


   /**
    * [[action]] Confirm Camping Car Trade
    *
    * @param deal_id - deal id of camping car trade
    * @param seller - camping car seller blockchain account (signature required)
    * @param buyer - camping car buyer blockchain account (signature required)
    */
   void allcamper_contract::cnfmcartrade( const uint64_t deal_id, const account_name seller, const account_name buyer ) {

      camping_car_trade_table_idx camping_car_trade_table( _self, _self );
      auto camping_car_trade_it = camping_car_trade_table.find( deal_id );
      eosio_assert( camping_car_trade_it != camping_car_trade_table.end(), "no matched camping car trade record" );

      auto camping_car_trade_obj = *camping_car_trade_it;
      eosio_assert( camping_car_trade_obj.status == CAMPING_CAR_TRADE_STATUS_RESERVED, "invalid status, not *reserved* trade status" );
      eosio_assert( camping_car_trade_obj.seller_account == seller, "not matched seller account" );
      eosio_assert( camping_car_trade_obj.buyer_account == buyer, "not matched buyer account" );

      camping_car_table_idx camping_car_table( _self, _self );
      auto camping_car_info_it = camping_car_table.find( camping_car_trade_obj.car_id );
      eosio_assert( camping_car_info_it != camping_car_table.end(), "no existing camping car item matching car_id" );

      // needs signature of seller and buyer
      require_auth( seller );
      require_auth( buyer );

      camping_car_trade_table.modify( camping_car_trade_it, 0, [&](camping_car_trade& t) {
         t.status = CAMPING_CAR_TRADE_STATUS_CONFIRMED;
         t.confirmed_at = current_time();
      });

      camping_car_table.modify( camping_car_info_it, 0, [&](camping_car& c) {
         c.owner = buyer;
         c.status = CAMPING_CAR_STATUS_AVAILABLE;
      });

      // transfer AKRW token balance in escrow to seller account
      INLINE_ACTION_SENDER(akrw_token_interface, transfer)(
         ALLCAMPER_AKRW_TOKEN_CONTRACT, // code
         { _self, N(active) }, // permission
         { ALLCAMPER_AKRW_TOKEN_CONTRACT, _self, seller,
           camping_car_trade_obj.price, std::string("camping car trade") } // action parameters
      );
   }


   /**
    * [[action]] Cancel Camping Car Trade
    *
    * @param deal_id - deal id of camping car trade
    * @param canceler - deal canceler(seller or buyer) blockchain account (signature required)
    */
   void allcamper_contract::cnclcartrade( const uint64_t deal_id, const account_name canceler ) {

      camping_car_trade_table_idx camping_car_trade_table( _self, _self );
      auto camping_car_trade_it = camping_car_trade_table.find( deal_id );
      eosio_assert( camping_car_trade_it != camping_car_trade_table.end(), "no matched camping car trade record" );

      auto camping_car_trade_obj = *camping_car_trade_it;
      eosio_assert( camping_car_trade_obj.status == CAMPING_CAR_TRADE_STATUS_RESERVED, "invalid status, not *reserved* trade status" );
      eosio_assert( camping_car_trade_obj.seller_account == canceler || camping_car_trade_obj.buyer_account == canceler, "canceler account must be seller or buyer account" );

      camping_car_table_idx camping_car_table( _self, _self );
      auto camping_car_info_it = camping_car_table.find( camping_car_trade_obj.car_id );
      eosio_assert( camping_car_info_it != camping_car_table.end(), "no existing camping car item matching car_id" );

      // needs signature of canceler (seller or buyer)
      require_auth( canceler );

      camping_car_trade_table.modify( camping_car_trade_it, 0, [&](camping_car_trade& t) {
         t.status = CAMPING_CAR_TRADE_STATUS_CANCELLED;
      });

      camping_car_table.modify( camping_car_info_it, 0, [&](camping_car& c) {
         c.status = CAMPING_CAR_STATUS_AVAILABLE;
      });

      // refund AKRW token balance in escrow to buyer account
      INLINE_ACTION_SENDER(akrw_token_interface, transfer)(
         ALLCAMPER_AKRW_TOKEN_CONTRACT, // code
         { _self, N(active) }, // permission
         { ALLCAMPER_AKRW_TOKEN_CONTRACT, _self, camping_car_trade_obj.buyer_account,
           camping_car_trade_obj.price, std::string("camping car trade cancelled") } // action parameters
      );
   }


   /**
    * [[action]] Reserve Camping Car Rental
    *
    * @param rent_id - camping car rental id
    * @param car_id - camping car id
    * @param renter - camping car renter blockchain account (signature required)
    * @param price - rental price (AKRW token)
    */
   void allcamper_contract::rsvrentcar( const uint64_t rent_id, const uint64_t car_id, const account_name renter, const asset& price ) {

      camping_car_rental_table_idx camping_car_rental_table( _self, _self );
      eosio_assert( camping_car_rental_table.find( rent_id ) == camping_car_rental_table.end(), "duplicated rent_id" );

      camping_car_table_idx camping_car_table( _self, _self );
      auto camping_car_info_it = camping_car_table.find( car_id );
      eosio_assert( camping_car_info_it != camping_car_table.end(), "no existing camping car item matching car_id" );

      auto camping_car_info = *camping_car_info_it;
      eosio_assert( camping_car_info.status == CAMPING_CAR_STATUS_AVAILABLE, "camping car is not available for rental" );

      // needs signature of renter account
      require_auth( renter );

      // lock renter's token balance
      subtract_on_contract_akrw_token_balance( renter, price );

      camping_car_rental_table.emplace( _self, [&](camping_car_rental& r) {
         r.rent_id = rent_id;
         r.car_id = car_id;
         r.owner_account = camping_car_info.owner;
         r.renter_account = renter;
         r.price = price;
         r.status = CAMPING_CAR_RENTAL_STATUS_RESERVED;
         r.reserved_at = current_time();
         r.confirmed_at = 0;
      });

      camping_car_table.modify( camping_car_info_it, 0, [&](camping_car& c) {
         c.status = CAMPING_CAR_STATUS_RENT;
      });
   }


   /**
    * [[action]] Confirm Camping Car Rental
    *
    * @param rent_id - camping car rent id
    * @param owner - camping car owner blockchain account (signature required)
    * @param renter - camping car renter blockchain account (signature required)
    */
   void allcamper_contract::cnfmrentcar( const uint64_t rent_id, const account_name owner, const account_name renter ) {

      camping_car_rental_table_idx camping_car_rental_table( _self, _self );
      auto camping_car_rental_it = camping_car_rental_table.find( rent_id );
      eosio_assert( camping_car_rental_it != camping_car_rental_table.end(), "no matched camping car rental record" );

      auto camping_car_rental_obj = *camping_car_rental_it;
      eosio_assert( camping_car_rental_obj.status == CAMPING_CAR_RENTAL_STATUS_RESERVED, "invalid status, not *reserved* rental status" );
      eosio_assert( camping_car_rental_obj.owner_account == owner, "not matched owner account" );
      eosio_assert( camping_car_rental_obj.renter_account == renter, "not matched renter account" );

      camping_car_table_idx camping_car_table( _self, _self );
      auto camping_car_info_it = camping_car_table.find( camping_car_rental_obj.car_id );
      eosio_assert( camping_car_info_it != camping_car_table.end(), "no existing camping car item matching car_id" );

      // needs signature of owner and renter
      require_auth( owner );
      require_auth( renter );

      camping_car_rental_table.modify( camping_car_rental_it, 0, [&](camping_car_rental& r) {
         r.status = CAMPING_CAR_RENTAL_STATUS_CONFIRMED;
         r.confirmed_at = current_time();
      });

      camping_car_table.modify( camping_car_info_it, 0, [&](camping_car& c) {
         c.status = CAMPING_CAR_STATUS_AVAILABLE;
      });

      // transfer AKRW token balance in escrow to camping car owner account
      INLINE_ACTION_SENDER(akrw_token_interface, transfer)(
         ALLCAMPER_AKRW_TOKEN_CONTRACT, // code
         { _self, N(active) }, // permission
         { ALLCAMPER_AKRW_TOKEN_CONTRACT, _self, owner,
           camping_car_rental_obj.price, std::string("camping car rental") } // action parameters
      );
   }


   /**
    * [[action]] Cancel Camping Car Rental
    *
    * @param rent_id - camping car rent id
    * @param canceler - car rental canceler(owner or renter) blockchain account (signature required)
    */
   void allcamper_contract::cnclrentcar( const uint64_t rent_id, const account_name canceler ) {

      camping_car_rental_table_idx camping_car_rental_table( _self, _self );
      auto camping_car_rental_it = camping_car_rental_table.find( rent_id );
      eosio_assert( camping_car_rental_it != camping_car_rental_table.end(), "no matched camping car rental record" );

      auto camping_car_rental_obj = *camping_car_rental_it;
      eosio_assert( camping_car_rental_obj.status == CAMPING_CAR_RENTAL_STATUS_RESERVED, "invalid status, not *reserved* rental status" );
      eosio_assert( camping_car_rental_obj.owner_account == canceler || camping_car_rental_obj.renter_account == canceler, "canceler account must be owner or renter account" );

      camping_car_table_idx camping_car_table( _self, _self );
      auto camping_car_info_it = camping_car_table.find( camping_car_rental_obj.car_id );
      eosio_assert( camping_car_info_it != camping_car_table.end(), "no existing camping car item matching car_id" );

      // needs signature of canceler (owner or renter)
      require_auth( canceler );

      camping_car_rental_table.modify( camping_car_rental_it, 0, [&](camping_car_rental& r) {
         r.status = CAMPING_CAR_RENTAL_STATUS_CANCELLED;
      });

      camping_car_table.modify( camping_car_info_it, 0, [&](camping_car& c) {
         c.status = CAMPING_CAR_STATUS_AVAILABLE;
      });

      // refund AKRW token balance in escrow to renter account
      INLINE_ACTION_SENDER(akrw_token_interface, transfer)(
         ALLCAMPER_AKRW_TOKEN_CONTRACT, // code
         { _self, N(active) }, // permission
         { ALLCAMPER_AKRW_TOKEN_CONTRACT, _self, camping_car_rental_obj.renter_account,
           camping_car_rental_obj.price, std::string("camping car rental cancelled") } // action parameters
      );
   }


   /**
    * [[action]] Review Camping Car Rental
    *
    * @param rent_id - camping car rent id
    * @param renter - camping car renter blockchain account (signature required)
    * @param score - review score
    * @param review_data_multihash - ipfs multihash(base58) of review data json file
    */
   void allcamper_contract::reviewrental( const uint64_t rent_id, const account_name renter, const double score, const string& review_data_multihash ) {

      rental_review_table_idx rental_review_table( _self, _self );
      eosio_assert( rental_review_table.find( rent_id ) == rental_review_table.end(), "duplicated rent_id on review table" );

      camping_car_rental_table_idx camping_car_rental_table( _self, _self );
      auto camping_car_rental_it = camping_car_rental_table.find( rent_id );
      eosio_assert( camping_car_rental_it != camping_car_rental_table.end(), "no matched camping car rental record" );
      auto camping_car_rental_info = *camping_car_rental_it;

      eosio_assert( camping_car_rental_info.renter_account == renter, "not matched renter account" );
      eosio_assert( camping_car_rental_info.status == CAMPING_CAR_RENTAL_STATUS_CONFIRMED, "car retal is not *confirmed* status" );
      eosio_assert( score >= 0.0 && score <= 10.0, "invalid score value" );

      camping_car_table_idx camping_car_table( _self, _self );
      auto camping_car_info_it = camping_car_table.find( camping_car_rental_info.car_id );
      eosio_assert( camping_car_info_it != camping_car_table.end(), "no existing camping car item matching car_id" );
      auto camping_car_info = *camping_car_info_it;

      // needs signature of camping car renter
      require_auth( renter );

      uint64_t now = current_time();

      rental_review_table.emplace( _self, [&](rental_review& r) {
         r.rent_id = rent_id;
         r.car_id = camping_car_rental_info.car_id;
         r.renter = renter;
         r.created_at = now;
         r.score = score;
         r.review_data_multihash = review_data_multihash;
      });

      double weighted_review_score = score;
      if (camping_car_info.last_review_at != 0) {
         double year_ms = 365.0*24*60*60*1000;
         double time_weight = double((now - camping_car_info.last_review_at)) / year_ms;
         if ( time_weight > 1.0 ) {
            time_weight = 1.0;
         }
         double count_weight = 1.0 / double(camping_car_info.review_count + 1);
         double weight = time_weight*0.7 + count_weight;
         if ( weight > 1.0 ) {
            weight = 1.0;
         }
         weighted_review_score = camping_car_info.review_score * (1.0-weight) + weight * score;
      }

      camping_car_table.modify( camping_car_info_it, 0, [&](camping_car& c) {
         c.review_score = weighted_review_score;
         c.review_count += 1;
         c.last_review_at = now;
      });
   }


   //////////////////////////////////////////////////////
   /// private utility function implementations

   void allcamper_contract::add_on_contract_akrw_token_balance( const account_name account, const asset& value ) {
      eosio_assert( value.symbol.value == ALLCAMPER_AKRW_TOKEN_SYMBOL, "invalid token symbol" );
      eosio_assert( value.amount >= 0, "invalid token value" );

      allcamper_account_table_idx account_table( _self, _self );
      auto account_info_it = account_table.find( account );
      eosio_assert( account_info_it != account_table.end(), "not registered account" );

      account_table.modify( account_info_it, 0, [&](allcamper_account& info) {
         info.deposited_akrw_bal += value;
      });
   }

   void allcamper_contract::subtract_on_contract_akrw_token_balance( const account_name account, const asset& value ) {
      eosio_assert( value.symbol.value == ALLCAMPER_AKRW_TOKEN_SYMBOL, "invalid token symbol" );
      eosio_assert( value.amount >= 0, "invalid token value" );

      allcamper_account_table_idx account_table( _self, _self );
      auto account_info_it = account_table.find( account );
      eosio_assert( account_info_it != account_table.end(), "not registered account" );

      eosio_assert( (*account_info_it).deposited_akrw_bal.amount >= value.amount, "insufficient on-contract token balance" );

      account_table.modify( account_info_it, 0, [&](allcamper_account& info) {
         info.deposited_akrw_bal -= value;
      });
   }

} /// namespace allcamper

extern "C" {
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
//      prints("allcamper_contract::apply(");
//      printn(receiver);
//      prints(",");
//      printn(code);
//      prints(",");
//      printn(action);
//      prints(")");
      auto self = receiver;
      if ( code == ALLCAMPER_AKRW_TOKEN_CONTRACT && action == N(transfer) ) {
         allcamper::allcamper_contract contract( self );
         eosio::execute_action( &contract, &allcamper::allcamper_contract::receive_token );
      }
      if( action == N(onerror)) {
         /* onerror is only valid if it is for the "infrasys" code account and authorized by "infrasys"'s "active permission */
         eosio_assert(code == N(infrasys), "onerror action's are only valid from the system account");
      }
      if( code == self || action == N(onerror) ) {
         allcamper::allcamper_contract thiscontract( self );
         switch( action ) {
            EOSIO_API( allcamper::allcamper_contract,
                       (reguser)(updateuser)(regcampcar)(updatecar)
                       (rsvcartrade)(cnfmcartrade)(cnclcartrade)
                       (rsvrentcar)(cnfmrentcar)(cnclrentcar)(reviewrental) )
         }
         /* does not allow destructor of thiscontract to run: eosio_exit(0); */
      }
   }
}
