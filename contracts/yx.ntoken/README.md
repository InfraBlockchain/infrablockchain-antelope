# Abtract
* This contract is for native token.
* The contract for non-native tokens is [`yx.token`](../../contracts/yx.token/README.md). It's recommended to read about native token first.

# Types of Tokens
* There are two types of tokens in Yosemite Public Blockchain.
  1. native token
  1. non-native tokens
* The native token supports only one type of dFIAT token. First of all, it is used as the fee of operations.
* The non-native tokens are created by any depositories and individuals. It is made of the symbol and the issuer. e.g. BTC/d1
  * They are all different for each token issuer. For example, BTC/d1 and BTC/d2 are both different.

# Types of Token Operations
* There are two types of operations.
  1. for depositories
  1. for any users

# What You Can Do With This Contract
* Depositories can issue and redeem the native token.
  * For example, if the native token is DKRW, the depository account d1(depository 1) and d2 can issue and redeem DKRW separately.
  * If a new depository wants to issue the native token, it must call the 'regdepon' operation with multisig to propose that it wants to join in.
     * The majority of the current depositories must agree using multisig feature.
* Through the contract, blockchain users can transfer the native token regardless of depositories. There would be two general cases.
  1. When user1 has 1000 DKRW/d1 and 1000 DKRW/d2, total 2000 DKRW, and transfers 2000 DKRW to user2, user2 will have 1000 DKRW/d1 and 1000 DKRW/d2.
  1. When user1 has 1000 DKRW/d1 and 1000 DKRW/d2, total 2000 DKRW, and transfers 1500 DKRW to user2, user2 will have 1000 DKRW/d1 and 500 DKRW/d2 or 500 DKRW/d1 and 1000 DKRW/d2 randomly but 1500 DKRW in total.

# Before testing the contract
* Assume that the eosio.bios contract is prepared.
   * https://github.com/EOSIO/eos/wiki/Tutorial-Getting-Started-With-Contracts
* Assume that yx.feedist, d1, d2, user1 and user2 accounts are created.

## Unlock wallet
cleos wallet unlock --password \<your-wallet-password\>

## Create the contract account
```
cleos wallet import 5JBt64r7caPQTkFmLReckxLS28n9TEmGhs43hUqPh7DXpomjhfc
cleos wallet import 5KSmCCSkBz84feSzzg5Bk9A1dsyTg6N72bwcoS48sg638Q8g12s
cleos create account eosio yx.ntoken EOS5NQoKnxzmkrTRHydYyicYYQQ59hGFLe1qj6vWnJk291tghgbhx EOS7fXnHFBpHLpQevKzdrfKffnAyK8vydPbmA6grXb6gFrezsELKa
```

## deploy the contract
`cleos set contract yx.ntoken build/contracts/yx.ntoken -p yx.ntoken`

# Setting Fee for Operations
* Fee for operations is set by `setfee` operation.
* The majority of the current depositories must agree using multisig feature.
* [Temporary] For test convenience, this operation is not yet multisig.
```
cleos push action yx.ntoken setfee '[ "issuen", "0.0000 DKRW" ]' -p yx.ntoken
cleos push action yx.ntoken setfee '[ "redeemn", "1000.0000 DKRW" ]' -p yx.ntoken
cleos push action yx.ntoken setfee '[ "transfern", "10.0000 DKRW" ]' -p yx.ntoken
cleos push action yx.ntoken setfee '[ "transfer", "5.0000 DKRW" ]' -p yx.ntoken
```

# Operations

## register d1 as native token depository
* d1 wants to join as the depository.
* [Temporary] For test convenience, this operation is not yet multisig.
```
cleos push action yx.ntoken regdepon '[ "d1" ]' -p d1
```

## issue native token to user1 by d1
* In this example, DKRW/system is the native token.
```
cleos push action yx.ntoken issuen '[ "user1", {"quantity":"100000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
```

## redeem native token from user1 by d1
1. user1 transfers the native token to d1.
```
cleos push action yx.ntoken transfer '[ "user1", "d1", {"quantity":"10000.0000 DKRW","issuer":""},  "user1", "memo" ]' -p user1
```
1. d1 checks the transfer operation is done and calls redeemn operation.
```
cleos push action yx.ntoken redeemn '[ {"quantity":"10000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
```

## transfer native token from user1 to user2
### use normal transfer operation
* DKRW tokens issued by any depositories can be transferred.
```
cleos push action yx.ntoken transfer '[ "user1", "user2", {"quantity":"10000.0000 DKRW","issuer":""}, "user1", "memo" ]' -p user1
```
### use transfern operation
* would be used by depository only
```
cleos push action yx.ntoken transfern '[ "user1", "user2", {"quantity":"10000.0000 DKRW","issuer":"d1"}, "user1", "memo" ]' -p user1
```
### parameters of transfer and transfern operations
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `quantity` : token amount and symbol with issuer account; in case of native token, issuer is the empty string
   1. `fee payer` : account name which pays fee; usually the same as `from` account name
   1. `memo` : string less than or equal to 256 bytes

### if the fee payer account is different from 'from' account
* It requires the signature of the fee payer account.
```
cleos push action yx.ntoken transfer '[ "user1", "user2", {"quantity":"10000.0000 DKRW","issuer":"d1"}, "servprovider", "memo" ]' -p user1 servprovider
```

# Example
* Let's say d1 is the depository for DKRW with precision 4, the native token of the blockchain.
* First of all, you must set fee for all operations.
* Note that printsupplyn, printbalance are not public operations.

```
cleos push action yx.ntoken regdepon '[ "d1" ]' -p d1
cleos push action yx.ntoken issuen '[ "d1", {"quantity":"1000000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
cleos push action yx.ntoken issuen '[ "d2", {"quantity":"100000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
cleos push action yx.ntoken printsupplyn '[ "d1" ]' -p yx.ntoken
cleos push action yx.ntoken printbalance '{"owner":"d1", "symbol":{"symbol":"4,DKRW","issuer":""}}' -p yx.ntoken
cleos push action yx.ntoken printbalance '{"owner":"d2", "symbol":{"symbol":"4,DKRW","issuer":""}}' -p yx.ntoken
cleos push action yx.ntoken transfer '[ "d1", "user1", {"quantity":"10000.0000 DKRW","issuer":""}, "d1", "memo"]' -p d1
cleos push action yx.ntoken transfern '[ "d1", "user1", {"quantity":"10000.0000 DKRW","issuer":"d1"}, "d1", "memo" ]' -p d1
cleos push action yx.ntoken printbalance '{"owner":"user1", "symbol":{"symbol":"4,DKRW","issuer":""}}' -p yx.ntoken
cleos push action yx.ntoken transfer '[ "user1", "user2", {"quantity":"10000.0000 DKRW","issuer":""}, "d2", "memo" ]' -p user1 d2
# error transactions
cleos push action yx.ntoken transfern '[ "d1", "user1", {"quantity":"100000000.0000 DKRW","issuer":"d1"}, "d1", m"]' -p d1
cleos push action yx.ntoken transfern '[ "d1", "user1", {"quantity":"10000.0000 DKRW","issuer":"d2"}, "d1", "m" ]' -p d1
cleos push action yx.ntoken transfer '[ "user1", "user2", {"quantity":"10000.0000 DKRW","issuer":""}, "d2", "m" ]' -p user1
cleos push action yx.token transfer '[ "user1", "user2", {"quantity":"10000.0000 BTC","issuer":"d2"}, "m", "user1" ]' -p user1
```