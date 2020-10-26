#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/symbol.hpp>
#include <eosiolib/time.hpp>

#include <string>

#define ALLCAMPER_AKRW_TOKEN_CONTRACT ::eosio::string_to_name("akrw.token")
#define ALLCAMPER_AKRW_TOKEN_SYMBOL ::eosio::string_to_symbol(0,"AKRW")

namespace allcamper {

   using namespace eosio;
   using std::string;

   /**
    * AllCamper camping car trading and renting smart contract
    */
   class allcamper_contract : public eosio::contract {
   public:
      explicit allcamper_contract( account_name self ) : contract(self) {}

      /**
       * token receiving handler called when AllCamper stable token (AKRW) is transferred to this contract account
       *
       * @param t - token id (token account name)
       * @param from - account name sending tokens
       * @param to - account name receiving tokens
       * @param qty - token quantity (amount, symbol) to transfer
       * @param tag - user tag string to identify a specific transfer action (application-specific purpose)
       */
      void receive_token( const account_name t, const account_name from, const account_name to, const asset& qty, const string& tag );


      /**
       * [[action]] Register AllCamper User Account
       *
       * @param account - AllCamper user's blockchain account (signature required)
       * @param user_info_multihash - ipfs multihash(base58) of user info json file
       */
      void reguser( const account_name account, const string& user_info_multihash );

      /**
       * [[action]] Update AllCamper User Info
       *
       * @param account - AllCamper user's blockchain account (signature required)
       * @param user_info_multihash - ipfs multihash(base58) of user info json file
       */
      void updateuser( const account_name account, const string& user_info_multihash );

      /**
       * [[action]] Register Camping Car
       *
       * @param car_id - camping car id
       * @param owner - campping owner's blockchain account (signature required)
       * @param car_info_multihash - ipfs multihash(base58) of car info json file
       */
      void regcampcar( const uint64_t car_id, const account_name owner, const string& car_info_multihash );

      /**
       * [[action]] Update Camping Car Info
       *
       * @param car_id - camping car id
       * @param owner - campping owner's blockchain account (signature required)
       * @param car_info_multihash - ipfs multihash(base58) of car info json file
       */
      void updatecar( const uint64_t car_id, const account_name owner, const string& car_info_multihash );

      /**
       * [[action]] Reserve Camping Car Trade
       *
       * @param deal_id - deal id of camping car trade
       * @param car_id - car id to trade
       * @param buyer - camping car buyer blockchain account (signature required)
       * @param price - camping car trade price (AKRW token)
       */
      void rsvcartrade( const uint64_t deal_id, const uint64_t car_id, const account_name buyer, const asset& price );


      /**
       * [[action]] Confirm Camping Car Trade
       *
       * @param deal_id - deal id of camping car trade
       * @param seller - camping car seller blockchain account (signature required)
       * @param buyer - camping car buyer blockchain account (signature required)
       */
      void cnfmcartrade( const uint64_t deal_id, const account_name seller, const account_name buyer );


      /**
       * [[action]] Cancel Camping Car Trade
       *
       * @param deal_id - deal id of camping car trade
       * @param canceler - deal canceler(seller or buyer) blockchain account (signature required)
       */
      void cnclcartrade( const uint64_t deal_id, const account_name canceler );


      /**
       * [[action]] Reserve Camping Car Rental
       *
       * @param rent_id - camping car rental id
       * @param car_id - camping car id
       * @param renter - camping car renter blockchain account (signature required)
       * @param price - rental price (AKRW token)
       */
      void rsvrentcar( const uint64_t rent_id, const uint64_t car_id, const account_name renter, const asset& price );


      /**
       * [[action]] Confirm Camping Car Rental
       *
       * @param rent_id - camping car rent id
       * @param owner - camping car owner blockchain account (signature required)
       * @param renter - camping car renter blockchain account (signature required)
       */
      void cnfmrentcar( const uint64_t rent_id, const account_name owner, const account_name renter );


      /**
       * [[action]] Cancel Camping Car Rental
       *
       * @param rent_id - camping car rent id
       * @param canceler - car rental canceler(owner or renter) blockchain account (signature required)
       */
      void cnclrentcar( const uint64_t rent_id, const account_name canceler );


      /**
       * [[action]] Review Camping Car Rental
       *
       * @param rent_id - camping car rent id
       * @param renter - camping car renter blockchain account (signature required)
       * @param score - review score
       * @param review_data_multihash - ipfs multihash(base58) of review data json file
       */
      void reviewrental( const uint64_t rent_id, const account_name renter, const double score, const string& review_data_multihash );


   private:

      void add_on_contract_akrw_token_balance( const account_name account, const asset& value );
      void subtract_on_contract_akrw_token_balance( const account_name account, const asset& value );


   };

   struct allcamper_account {
      account_name    account;
      asset           deposited_akrw_bal; // deposited AKRW token balance
      string          user_info_multihash; // ipfs multihash(base58) of user info json file

      uint64_t primary_key() const { return account; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( allcamper_account, (account)(deposited_akrw_bal)(user_info_multihash) )
   };

   typedef eosio::multi_index< N(accounts), allcamper_account > allcamper_account_table_idx;

   struct camping_car {
      uint64_t         car_id;
      account_name     owner;
      uint8_t          status; // 0: available, 1: rent, 2: trading
      double           review_score; // weighed average review score
      uint32_t         review_count;
      uint64_t         last_review_at;
      string           car_info_multihash; // ipfs multihash(base58) of car info json file

      uint64_t primary_key() const { return car_id; }
      uint64_t by_owner() const { return owner; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( camping_car, (car_id)(owner)(status)(review_score)(review_count)(last_review_at)(car_info_multihash) )
   };

   static constexpr uint8_t CAMPING_CAR_STATUS_AVAILABLE = 0;
   static constexpr uint8_t CAMPING_CAR_STATUS_RENT = 1;
   static constexpr uint8_t CAMPING_CAR_STATUS_TRADING = 2;

   typedef eosio::multi_index< N(campingcars), camping_car,
                               indexed_by<N(byowner), const_mem_fun<camping_car, uint64_t, &camping_car::by_owner> >
                               > camping_car_table_idx;

   struct camping_car_trade {
      uint64_t          deal_id;
      uint64_t          car_id;
      account_name      seller_account;
      account_name      buyer_account;
      asset             price;
      uint8_t           status; // 1: reserved, 2: confirmed, 3: cancelled
      uint64_t          reserved_at;
      uint64_t          confirmed_at;

      uint64_t primary_key() const { return deal_id; }
      uint64_t by_car() const { return car_id; }
      account_name by_seller() const { return seller_account; }
      account_name by_buyer() const { return buyer_account; }
      uint64_t by_status() const { return status; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( camping_car_trade, (deal_id)(car_id)(seller_account)(buyer_account)(price)(status)(reserved_at)(confirmed_at) )
   };

   static constexpr uint8_t CAMPING_CAR_TRADE_STATUS_RESERVED = 1;
   static constexpr uint8_t CAMPING_CAR_TRADE_STATUS_CONFIRMED = 2;
   static constexpr uint8_t CAMPING_CAR_TRADE_STATUS_CANCELLED = 3;

   typedef eosio::multi_index< N(trades), camping_car_trade,
                               indexed_by<N(bycar), const_mem_fun<camping_car_trade, uint64_t, &camping_car_trade::by_car> >,
                               indexed_by<N(byseller), const_mem_fun<camping_car_trade, account_name, &camping_car_trade::by_seller> >,
                               indexed_by<N(bybuyer), const_mem_fun<camping_car_trade, account_name, &camping_car_trade::by_buyer> >,
                               indexed_by<N(bystatus), const_mem_fun<camping_car_trade, uint64_t, &camping_car_trade::by_status> >
                               > camping_car_trade_table_idx;

   struct camping_car_rental {
      uint64_t          rent_id;
      uint64_t          car_id;
      account_name      owner_account;
      account_name      renter_account;
      asset             price;
      uint8_t           status; // 1: reserved, 2: confirmed, 3: cancelled
      uint64_t          reserved_at;
      uint64_t          confirmed_at;

      uint64_t primary_key() const { return rent_id; }
      uint64_t by_car() const { return car_id; }
      account_name by_owner() const { return owner_account; }
      account_name by_renter() const { return renter_account; }
      uint64_t by_status() const { return status; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( camping_car_rental, (rent_id)(car_id)(owner_account)(renter_account)(price)(status)(reserved_at)(confirmed_at) )
   };

   static constexpr uint8_t CAMPING_CAR_RENTAL_STATUS_RESERVED = 1;
   static constexpr uint8_t CAMPING_CAR_RENTAL_STATUS_CONFIRMED = 2;
   static constexpr uint8_t CAMPING_CAR_RENTAL_STATUS_CANCELLED = 3;

   typedef eosio::multi_index< N(rentals), camping_car_rental,
                               indexed_by<N(bycar), const_mem_fun<camping_car_rental, uint64_t, &camping_car_rental::by_car> >,
                               indexed_by<N(byowner), const_mem_fun<camping_car_rental, account_name, &camping_car_rental::by_owner> >,
                               indexed_by<N(byrenter), const_mem_fun<camping_car_rental, account_name, &camping_car_rental::by_renter> >,
                               indexed_by<N(bystatus), const_mem_fun<camping_car_rental, uint64_t, &camping_car_rental::by_status> >
                               > camping_car_rental_table_idx;

   struct rental_review {
      uint64_t          rent_id;
      uint64_t          car_id;
      account_name      renter;
      uint64_t          created_at;
      double            score;
      string            review_data_multihash; // ipfs multihash(base58) of review data json file

      uint64_t primary_key() const { return rent_id; }
      uint64_t by_car() const { return car_id; }
      account_name by_renter() const { return renter; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( rental_review, (rent_id)(car_id)(renter)(created_at)(score)(review_data_multihash) )
   };

   typedef eosio::multi_index< N(reviews), rental_review,
                               indexed_by<N(bycar), const_mem_fun<rental_review, uint64_t, &rental_review::by_car> >,
                               indexed_by<N(byrenter), const_mem_fun<rental_review, account_name, &rental_review::by_renter> >
                               > rental_review_table_idx;

} /// namespace allcamper
