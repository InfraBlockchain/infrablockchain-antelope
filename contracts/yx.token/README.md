# Types of Tokens
* There are two types of tokens in Yosemite Public Blockchain.
  1. native token
  1. non-native tokens
* The native token supports only one type of dFIAT token. First of all, it is used as the fee of operations.
* The non-native tokens are created by any depositories and individuals. It is made of the symbol and the issuer. e.g. BTC/d1
  * They are all different for each token issuer. For example, BTC/d1 and BTC/d2 are both different.

# Types of Token Operations
* There are three types of operations.
  1. for native token
  1. for non-native token
  1. for management purpose 

# What You Can Do With yx.token
* Depositories can issue and redeem the native token.
  * For example, if the native token is DKRW, the depository account d1(depository 1) and d2 can issue and redeem DKRW separately.
  * If a new depository wants to issue the native token, it must call the 'regdepon' operation with multisig to propose that it wants to join in.
     * The majority of the current depositories must agree using multisig feature.
* Through the contract, blockchain users can transfer the native token regardless of depositories. There would be two general cases.
  1. When user1 has 1000 DKRW/d1 and 1000 DKRW/d2, total 2000 DKRW, and transfers 2000 DKRW to user2, user2 will have 1000 DKRW/d1 and 1000 DKRW/d2.
  1. When user1 has 1000 DKRW/d1 and 1000 DKRW/d2, total 2000 DKRW, and transfers 1500 DKRW to user2, user2 will have 1000 DKRW/d1 and 500 DKRW/d2 or 500 DKRW/d1 and 1000 DKRW/d2 randomly but 1500 DKRW in total.
* Other than the native token, depositories or individuals can create/issue/redeem tokens and users can transfer them each other.

# To test yx.token contract
* Assume that the eosio.bios contract is prepared.
   * https://github.com/EOSIO/eos/wiki/Tutorial-Getting-Started-With-Contracts
* Assume that yx.feedist, d1, d2, user1 and user2 accounts are created.

## Unlock wallet
cleos wallet unlock --password \<your-wallet-password\>

## Create yx.token contract account
```
cleos wallet import 5JBt64r7caPQTkFmLReckxLS28n9TEmGhs43hUqPh7DXpomjhfc
cleos wallet import 5KSmCCSkBz84feSzzg5Bk9A1dsyTg6N72bwcoS48sg638Q8g12s
cleos create account eosio yx.token EOS5NQoKnxzmkrTRHydYyicYYQQ59hGFLe1qj6vWnJk291tghgbhx EOS7fXnHFBpHLpQevKzdrfKffnAyK8vydPbmA6grXb6gFrezsELKa
```

## deploy yx.token contract
`cleos set contract yx.token build/contracts/yx.token -p yx.token`

# Setting Fee for Operations
* Fee for operations is set by `setfee` operation.
* The majority of the current depositories must agree using multisig feature.
* [Temporary] For test convenience, this operation is not yet multisig.
```
cleos push action yx.token setfee '[ "issuen", "0.0000 DKRW" ]' -p yx.token
cleos push action yx.token setfee '[ "redeemn", "1000.0000 DKRW" ]' -p yx.token
cleos push action yx.token setfee '[ "transfern", "10.0000 DKRW" ]' -p yx.token
cleos push action yx.token setfee '[ "regdepo", "10000.0000 DKRW" ]' -p yx.token
cleos push action yx.token setfee '[ "issue", "0.0000 DKRW" ]' -p yx.token
cleos push action yx.token setfee '[ "redeem", "1000.0000 DKRW" ]' -p yx.token
cleos push action yx.token setfee '[ "transfer", "5.0000 DKRW" ]' -p yx.token
```

# Native Token

## register d1 as native token depository
* d1 wants to join as the depository.
* [Temporary] For test convenience, this operation is not yet multisig.
```
cleos push action yx.token regdepon '[ "d1" ]' -p d1
```

## issue native token to user1 by d1
* In this example, DKRW/system is the native token.
```
cleos push action yx.token issuen '[ "user1", {"quantity":"100000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
```

## redeem native token from user1 by d1
1. user1 transfers the native token to d1.
```
cleos push action yx.token transfer '[ "user1", "d1", {"quantity":"10000.0000 DKRW","issuer":""}, "m", "user1" ]' -p user1
```
1. d1 checks the transfer operation is done and calls redeemn operation.
```
cleos push action yx.token redeemn '[ {"quantity":"10000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
```

## transfer native token from user1 to user2
### use normal transfer operation
* DKRW tokens issued by any depositories can be transferred.
```
cleos push action yx.token transfer '[ "user1", "user2", {"quantity":"10000.0000 DKRW","issuer":""}, "m", "user1" ]' -p user1
```
### use transfern operation
* would be used by depository only
```
cleos push action yx.token transfern '[ "user1", "user2", {"quantity":"10000.0000 DKRW","issuer":"d1"}, "memo", "user1" ]' -p user1
```


# Non-native Token

## register d2 as non-native token depository
* d2 wants to be the depository of BTC token with precision 4.
```
cleos push action yx.token regdepo '[ {"symbol":"4,BTC","issuer":"d2"}]' -p d2
```

## issue non-native token to user2 by d2
```
cleos push action yx.token issue '[ "user2", {"quantity":"100000.0000 BTC","issuer":"d2"}, "memo" ]' -p d2
```

## redeem non-native token from user2 by d2
1. user2 transfers the native token to d2.
```
cleos push action yx.token transfer '[ "user2", "d2", {"quantity":"10000.0000 BTC","issuer":"d2"}, "m" ]' -p user2
```
1. d2 checks the transfer operation is done and calls redeem operation.
```
cleos push action yx.token redeem '[ {"quantity":"10000.0000 BTC","issuer":"d2"}, "redeem for user2" ]' -p d2
```

## transfer non-native token from user1 to user2
```
cleos push action yx.token transfer '[ "user1", "user2", {"quantity":"10000.0000 BTC","issuer":"d2"}, "m", "user1" ]' -p user1
```
* Parameters
   1. from account name
   1. to account name
   1. token amount and symbol with depository account
   1. memo : string less or equal than 256 bytes
   1. fee payer account name : usually the same as from account name

### if the fee payer account is different from 'from' account
* It requires the signature of the fee payer account.
```
cleos push action yx.token transfer '[ "user1", "user2", {"quantity":"10000.0000 BTC","issuer":"d2"}, "m", "servprovider" ]' -p user1 servprovider
```


# Example
* Let's say d1 is the depository for DKRW with precision 4, the native token of the blockchain.
* d2 wants to create and issue BTC with precision 4 token to user1.
* First of all, you must set fee for all operations.
* Note that printsupplyn, printbalance are not public operations.

```
cleos push action yx.token regdepon '[ "d1" ]' -p d1
cleos push action yx.token issuen '[ "d1", {"quantity":"1000000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
cleos push action yx.token issuen '[ "d2", {"quantity":"100000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
cleos push action yx.token printsupplyn '[ "d1" ]' -p yx.token
cleos push action yx.token printbalance '{"owner":"d1", "symbol":{"symbol":"4,DKRW","issuer":""}}' -p yx.token
cleos push action yx.token printbalance '{"owner":"d2", "symbol":{"symbol":"4,DKRW","issuer":""}}' -p yx.token
cleos push action yx.token transfer '[ "d1", "user1", {"quantity":"10000.0000 DKRW","issuer":""}, "m", "d1" ]' -p d1
cleos push action yx.token transfern '[ "d1", "user1", {"quantity":"10000.0000 DKRW","issuer":"d1"}, "m", "d1" ]' -p d1
cleos push action yx.token transfer '[ "user1", "user2", {"quantity":"10000.0000 DKRW","issuer":""}, "m", "d2" ]' -p user1 d2
# error transactions
cleos push action yx.token transfern '[ "d1", "user1", {"quantity":"100000000.0000 DKRW","issuer":"d1"}, "m", "d1" ]' -p d1
cleos push action yx.token transfern '[ "d1", "user1", {"quantity":"10000.0000 DKRW","issuer":"d2"}, "m", "d1" ]' -p d1
cleos push action yx.token transfer '[ "user1", "user2", {"quantity":"10000.0000 DKRW","issuer":""}, "m", "d2" ]' -p user1

cleos push action yx.token regdepo '[ {"symbol":"4,BTC","issuer":"d2"}]' -p d2
cleos push action yx.token printbalance '{"owner":"d2", "symbol":{"symbol":"4,DKRW","issuer":"system"}}' -p yx.token
cleos push action yx.token printbalance '{"owner":"yx.feedist", "symbol":{"symbol":"4,DKRW","issuer":"system"}}' -p yx.token
cleos push action yx.token issue '[ "user2", {"quantity":"100000.0000 BTC","issuer":"d2"}, "memo" ]' -p d2
cleos push action yx.token printbalance '{"owner":"user2", "symbol":{"symbol":"4,BTC","issuer":"d2"}}' -p yx.token
cleos push action yx.token transfer '[ "user2", "user1", {"quantity":"10000.0000 BTC","issuer":"d2"}, "m", "user2" ]' -p user2
```