/**
 *  @file infrablockchainlib/token_api.h
 *  @author bezalel@yosemitex.com
 *  @copyright defined in yosemite/LICENSE.txt
 */
#pragma once

#include <eosiolib/types.h>

extern "C" {

/**
 *  @defgroup YOSEMITE Standard Token API
 *  @ingroup contractdev
 *  @brief Defines API for YOSEMITE Standard Token Operations
 */

/**
 *  @defgroup YOSEMITE Standard Token C API
 *  @ingroup tokenapi
 *  @brief Defines API for YOSEMITE Standard Token Operations
 *  @{
 */


/**
 *  Get Token Symbol
 *  @brief get token symbol of a token
 *
 *  @param token_id - token account name
 *  @return token symbol name (eosio::symbol_name)
 */
uint64_t get_token_symbol( account_name token_id );

/**
 *  Get Token Total Supply
 *  @brief get current total supply of a token
 *
 *  @param token_id - token account name
 *  @return current total supply amount of the token
 */
int64_t get_token_total_supply( account_name token_id );

/**
 *  Get Token Balance
 *  @brief get token balance of an account for an token
 *
 *  @param token_id - token account name
 *  @param account - account name to retrieve token balance
 *  @return token balance of the account
 */
int64_t get_token_balance( account_name token_id, account_name account );

/**
 *  Issue Token
 *  @brief issue new token to a specific account,
 *  token_id is implicitly the action receiver (token owner) account,
 *  the contract code of token owner account can issue its own token only.
 *
 *  @param to - account name to which new token is issued
 *  @param amount - amount of token to issue
 */
void issue_token( account_name to, int64_t amount );

/**
 *  Transfer Token
 *  @brief transfer token from 'from' account to 'to' account,
 *  token_id is implicitly the action receiver (token owner) account,
 *  the contract code of token owner account can transfer its own token only.
 *
 *  @param from - account from which token amount is transferred (subtracted)
 *  @param to - account to which token amount is transferred (added)
 *  @param amount - amount of token to transfer
 */
void transfer_token( account_name from, account_name to, int64_t amount );

/**
 *  Redeem(Burn) Token
 *  @brief redeem(burn) token from token owner account,
 *  token_id is implicitly the action receiver (token owner) account,
 *  the contract code of token owner account can burn its own token only.
 *
 *  @param amount - amount of token to redeem(burn)
 */
void redeem_token( int64_t amount );

///@ } tokenapi
}
