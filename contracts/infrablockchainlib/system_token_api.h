/**
 *  @file infrablockchainlib/system_token_api.h
 *  @author bezalel@infrablockchain.com
 *  @copyright defined in infrablockchain/LICENSE.txt
 */
#pragma once

#include <eosiolib/types.h>

extern "C" {

/**
 *  @defgroup InfraBlockchain System Token API
 *  @ingroup contractdev
 *  @brief Defines API for InfraBlockchain System Token Operations
 */

/**
 *  @defgroup InfraBlockchain System Token C API
 *  @ingroup systokenapi
 *  @brief Defines API for InfraBlockchain System Token Operations
 *  @{
 */


/**
 *  Get System Token Count
 *  @brief get the number of active system tokens authorized by block producers and used as transaction fee payment token
 *
 *  @return the number of system tokens
 */
size_t get_system_token_count();

/**
 *  Get System Token List
 *  @brief get current total supply of a token
 *
 *  @param token_id - token account name
 *  @return current total supply amount of the token
 */
//int64_t get_system_token_list( account_name token_id );

/**
 * Get System Token List
 * @brief Retrieve the system token list
 *
 * @param data - output buffer of the system token list, only retrieved if sufficent size to hold packed data.
 * @param datalen - size of the data buffer, 0 to report required size.
 * @return size of the packed system token list data
 * @pre `data` is a valid pointer to a range of memory at least `datalen` bytes long
 * @post `data` is filled with packed system token list (vector<infrablockchain_system_token>)
 */
uint32_t get_system_token_list(char* data, uint32_t datalen);

/**
 * Set System Token List
 * @brief set the list of system tokens (vector<infrablockchain_system_token>) used as transaction fee payment token.
 *        2/3+ block producers have to sign any modification of system token list.
 *
 * @param system_token_list_data - packed data of system_token vector in the appropriate system token order
 * @param data_size - size of the data buffer
 *
 * @return -1 if setting new system token list was unsuccessful, otherwise returns the version of the new system token list
 */
int64_t set_system_token_list( char* system_token_list_data, uint32_t data_size );


///@ } systokenapi
}

struct infrablockchain_system_token {
   uint64_t token_id; // token account name selected as system token
   uint32_t token_weight; // token value weight as transaction fee payment, 10000 = 1.0x, 5000 = 0.5x (tx fee is charged 2x)
};
