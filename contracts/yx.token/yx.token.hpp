#pragma once

#include <eosiolib/eosio.hpp>
#include <yosemitelib/yx_asset.hpp>
#include <string>
#include <musl/upstream/include/bits/stdint.h>

namespace yosemite {

    using namespace eosio;
    using std::string;
    using boost::container::flat_map;

    class token : public contract {
    public:
        explicit token(account_name self) : contract(self) {
        }

        /* Option flags for can_set_options */
        #define TOKEN_CAN_SET_OPTIONS_NONE                      0b0000000000000000
        #define TOKEN_CAN_SET_OPTIONS_FREEZE_TOKEN_TRANSFER     0b0000000000000001
        #define TOKEN_CAN_SET_OPTIONS_FREEZE_ACCOUNT            0b0000000000000010
        #define TOKEN_CAN_SET_OPTIONS_SET_KYC_RULE              0b0000000000000100
        #define TOKEN_CAN_SET_OPTIONS_MAX                       0b0000000000000111

        void create(const yx_symbol &ysymbol, uint16_t can_set_options);
        void issue(const account_name &to, const yx_asset &token, const string &memo);
        void redeem(const yx_asset &token, const string &memo);
        void transfer(account_name from, account_name to, yx_asset asset, const string &memo);
        void wptransfer(account_name from, account_name to, yx_asset token, account_name payer, const string &memo);
        void setkycrule(const yx_symbol &ysymbol, uint8_t type, uint16_t kyc);
        void setoptions(const yx_symbol &ysymbol, uint16_t options, bool overwrite);
        void freezeacc(const yx_symbol &ysymbol, const vector<account_name> &accs, bool freeze);

    private:
        void charge_fee(const account_name &payer, uint64_t operation);

        /* Option flags for account management */
        #define TOKEN_ACCOUNT_OPTIONS_NONE                      0b0000000000000000
        #define TOKEN_ACCOUNT_OPTIONS_FREEZE_ACCOUNT            0b0000000000000001

        /* scope = owner */
        struct balance_holder {
            uint64_t id = 0;
            uint128_t yx_symbol_s{}; // yx_symbol which is serialized to 128 bit;
                                     // eosio::symbol_type::value is higher 64 bit and issuer account is lower 64 bit.
            int64_t amount = 0;
            uint16_t options = TOKEN_ACCOUNT_OPTIONS_NONE;

            uint64_t primary_key() const { return id; }
            uint128_t by_yx_symbol_s() const { return yx_symbol_s; }
        };

        /* KYC rule for token */
        enum token_kyc_rule_type {
            TOKEN_KYC_RULE_TYPE_TRANSFER_SEND    = 0,
            TOKEN_KYC_RULE_TYPE_TRANSFER_RECEIVE = 1,

            TOKEN_KYC_RULE_TYPE_MAX // MUST NOT EXCEED MORE THAN 255
        };

        /* Option flags for token management */
        #define TOKEN_OPTIONS_NONE                      0b0000000000000000
        #define TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER     0b0000000000000001
        #define TOKEN_OPTIONS_MAX                       0b0000000000000001

        /* scope = token symbol */
        struct token_stats {
            uint64_t issuer = 0;
            int64_t supply = 0;
            uint16_t can_set_options = TOKEN_CAN_SET_OPTIONS_NONE; // can set only at token creation time
            uint16_t options = TOKEN_OPTIONS_NONE;
            vector<uint8_t> kyc_rule_types; // == token_kyc_rule_type
            vector<uint16_t> kyc_rule_flags; // from yosemitelib/identity.hpp

            uint64_t primary_key() const { return issuer; }
        };

        typedef eosio::multi_index<N(tstats), token_stats> stats;
        typedef eosio::multi_index<N(taccounts), balance_holder,
                indexed_by<N(yxsymbol), const_mem_fun<balance_holder, uint128_t, &balance_holder::by_yx_symbol_s> >
        > accounts;

        void add_token_balance(const account_name &owner, const yx_asset &token);
        void sub_token_balance(const account_name &owner, const yx_asset &token);
        bool check_identity_auth_for_transfer(account_name account, const token_kyc_rule_type &kycrule_type,
                                              const token_stats &tstats);
    };
}
