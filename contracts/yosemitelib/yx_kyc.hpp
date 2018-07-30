/**
 *  @copyright defined in LICENSE.txt
 */
#ifndef YX_KYC_HPP
#define YX_KYC_HPP

#include <eosiolib/eosio.hpp>

namespace yosemite {
    const uint32_t KYC_VECTOR_NO_AUTH           = 0x00000000;
    const uint32_t KYC_VECTOR_EMAIL_AUTH        = 0x00000001;
    const uint32_t KYC_VECTOR_PHONE_AUTH        = 0x00000002;
    const uint32_t KYC_VECTOR_REAL_NAME_AUTH    = 0x00000004;
    const uint32_t KYC_VECTOR_BANK_ACCOUNT_AUTH = 0x00000008;

    /* When a new bit constant is added, KYC_VECTOR_MAX_VALUE must be updated. */
    const uint32_t KYC_VECTOR_MAX_VALUE =
            KYC_VECTOR_EMAIL_AUTH | KYC_VECTOR_PHONE_AUTH | KYC_VECTOR_REAL_NAME_AUTH |
            KYC_VECTOR_BANK_ACCOUNT_AUTH
    ;
}

#endif //YX_KYC_HPP
