/**
 *  @file
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <yosemitelib/system_accounts.hpp>

#define YOSEMITE_ID_ACC_TYPE_NORMAL        0b0000000000000000
#define YOSEMITE_ID_ACC_TYPE_SYSTEM        0b1000000000000000

#define YOSEMITE_ID_KYC_NO_AUTH            0b0000000000000000
#define YOSEMITE_ID_KYC_EMAIL_AUTH         0b0000000000000001
#define YOSEMITE_ID_KYC_PHONE_AUTH         0b0000000000000010
#define YOSEMITE_ID_KYC_REAL_NAME_AUTH     0b0000000000000100
#define YOSEMITE_ID_KYC_BANK_ACCOUNT_AUTH  0b0000000000001000
#define YOSEMITE_ID_KYC_MAX_AUTH           0b0000000000001111

#define YOSEMITE_ID_ACC_STATE_CLEAR                       0x00000000
#define YOSEMITE_ID_ACC_STATE_BLACKLISTED                 0x00000001
#define YOSEMITE_ID_ACC_STATE_BLACKLISTED_NTOKEN_SEND     0x00000002
#define YOSEMITE_ID_ACC_STATE_BLACKLISTED_NTOKEN_RECEIVE  0x00000004
#define YOSEMITE_ID_ACC_STATE_MAX                         0x00000007

namespace yosemite { namespace identity {

    using identity_type_t = uint16_t;
    using identity_kyc_t = uint16_t;
    using identity_state_t = uint32_t;

    struct identity_info {
        account_name account;
        account_name identity_authority; // identity authority account managing the identity info. of this 'account'
        identity_type_t type; // account type (e.g. normal, system, ...)
        identity_kyc_t kyc; // KYC status (e.g. email, phone, real name, bank account, ...)
        identity_state_t state; // account state (e.g. flag for blacklisted account, Identity-Authority specific flags, ...)
        std::string data; // Identity-Authority specific data

        uint64_t primary_key() const { return account; }

        // explicit serialization macro is not necessary, used here only to improve compilation time
        EOSLIB_SERIALIZE( identity_info, (account)(identity_authority)(type)(kyc)(state)(data) )
    };

    typedef eosio::multi_index<N(identity), identity_info> identity_idx;

    const identity_info get_identity_info(const account_name account) {
        if (is_system_account(account)) {
            return identity_info{
                account, 0,
                YOSEMITE_ID_ACC_TYPE_SYSTEM, YOSEMITE_ID_KYC_MAX_AUTH, YOSEMITE_ID_ACC_STATE_CLEAR, {}
            };
        }
        identity_idx identity_table(YOSEMITE_IDENTITY_ACCOUNT, YOSEMITE_IDENTITY_ACCOUNT);
        auto id_it = identity_table.find(account);
        if (id_it == identity_table.end()) {
            return identity_info{
                    account, 0,
                    YOSEMITE_ID_ACC_TYPE_NORMAL, YOSEMITE_ID_KYC_NO_AUTH, YOSEMITE_ID_ACC_STATE_CLEAR, {}
            };
        } else {
            return *id_it;
        }
    }

    identity_type_t get_identity_account_type(const account_name account) {
        if (is_system_account(account)) {
            return YOSEMITE_ID_ACC_TYPE_SYSTEM;
        }
        identity_idx identity_table(YOSEMITE_IDENTITY_ACCOUNT, YOSEMITE_IDENTITY_ACCOUNT);
        auto id_it = identity_table.find(account);
        if (id_it == identity_table.end()) {
            return YOSEMITE_ID_ACC_TYPE_NORMAL;
        } else {
            return id_it->type;
        }
    }

//    bool is_valid_account_type(uint16_t type) {
//        return type == YOSEMITE_ID_ACC_TYPE_NORMAL || type == YOSEMITE_ID_ACC_TYPE_SYSTEM;
//    }

    bool is_account_type(const account_name account, identity_type_t type_code) {
        return get_identity_account_type(account) == type_code;
    }

    identity_kyc_t get_identity_kyc_status(const account_name account) {
        if (is_system_account(account)) {
            return YOSEMITE_ID_KYC_MAX_AUTH;
        }
        identity_idx identity_table(YOSEMITE_IDENTITY_ACCOUNT, YOSEMITE_IDENTITY_ACCOUNT);
        auto id_it = identity_table.find(account);
        if (id_it == identity_table.end()) {
            return YOSEMITE_ID_KYC_NO_AUTH;
        } else {
            return id_it->kyc;
        }
    }

//    bool is_valid_kyc_status(uint16_t kyc_flags) {
//        return kyc_flags <= YOSEMITE_ID_KYC_MAX_AUTH;
//    }

    bool has_kyc_status(const account_name account, identity_kyc_t kyc_flags) {
        if (kyc_flags == 0) return true;
        return (get_identity_kyc_status(account) & kyc_flags) != 0;
    }

    bool has_all_kyc_status(const account_name account, identity_kyc_t kyc_flags) {
        return (get_identity_kyc_status(account) & kyc_flags) == kyc_flags;
    }

    identity_state_t get_identity_account_state(const account_name account) {
        if (is_system_account(account)) {
            return YOSEMITE_ID_ACC_STATE_CLEAR;
        }
        identity_idx identity_table(YOSEMITE_IDENTITY_ACCOUNT, YOSEMITE_IDENTITY_ACCOUNT);
        auto id_it = identity_table.find(account);
        if (id_it == identity_table.end()) {
            return YOSEMITE_ID_ACC_STATE_CLEAR;
        } else {
            return id_it->state;
        }
    }

//    bool is_valid_account_state(uint32_t state_flags) {
//        return state_flags <= YOSEMITE_ID_ACC_STATE_MAX;
//    }

    bool has_account_state(const account_name account, identity_state_t state_flag) {
        if (state_flag == 0) return true;
        return (get_identity_account_state(account) & state_flag) != 0;
    }

    bool has_all_account_state(const account_name account, identity_state_t state_flags) {
        return (get_identity_account_state(account) & state_flags) == state_flags;
    }

    std::string get_identity_data(const account_name account) {
        if (is_system_account(account)) {
            return std::string();
        }
        identity_idx identity_table(YOSEMITE_IDENTITY_ACCOUNT, YOSEMITE_IDENTITY_ACCOUNT);
        auto id_it = identity_table.find(account);
        if (id_it == identity_table.end()) {
            return std::string();
        } else {
            return id_it->data;
        }
    }

}}
