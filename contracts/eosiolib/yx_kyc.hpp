/**
 *  @copyright defined in LICENSE.txt
 */
#ifndef YX_KYC_HPP
#define YX_KYC_HPP

#include <eosiolib/eosio.hpp>

namespace yosemite {
    const uint32_t KYC_AUTHVECTOR_NO_AUTH           = 0x00000000;
    const uint32_t KYC_AUTHVECTOR_EMAIL_AUTH        = 0x00000001;
    const uint32_t KYC_AUTHVECTOR_PHONE_AUTH        = 0x00000002;
    const uint32_t KYC_AUTHVECTOR_REAL_NAME_AUTH    = 0x00000004;
    const uint32_t KYC_AUTHVECTOR_BANK_ACCOUNT_AUTH = 0x00000008;

    const uint32_t KYC_AUTHVECTOR_MAX_VALUE =
            KYC_AUTHVECTOR_EMAIL_AUTH | KYC_AUTHVECTOR_PHONE_AUTH | KYC_AUTHVECTOR_REAL_NAME_AUTH |
            KYC_AUTHVECTOR_BANK_ACCOUNT_AUTH
    ;
}

#endif //YX_KYC_HPP
