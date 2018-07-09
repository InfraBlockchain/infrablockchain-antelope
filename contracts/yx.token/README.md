# Abtract
* This contract is for non-native tokens.
* The contract for native token is [`yx.ntoken`](../../contracts/yx.ntoken/README.md). You must read it first.

# What You Can Do With This Contract
* An account which wants to register as the depository must call `regdepo` operation with its token symbol and precision.
   * Example symbol : BTC
   * Example precision : 8
   * Full Example : 1.00000000 BTC
* Registered depositories can issue/redeem tokens.
* Any users who own tokens can transfer them each other.

# Before Testing The Contract
* Assume that the eosio.bios contract is prepared.
   * https://github.com/EOSIO/eos/wiki/Tutorial-Getting-Started-With-Contracts
* Assume that the yx.ntoken contract is prepared.
* Assume that yx.feedist, d1, d2, user1 and user2 accounts are created.

## Unlock wallet
cleos wallet unlock --password \<your-wallet-password\>

## Create the contract account
```
cleos wallet import 5JBt64r7caPQTkFmLReckxLS28n9TEmGhs43hUqPh7DXpomjhfc
cleos wallet import 5KSmCCSkBz84feSzzg5Bk9A1dsyTg6N72bwcoS48sg638Q8g12s
cleos create account eosio yx.token EOS5NQoKnxzmkrTRHydYyicYYQQ59hGFLe1qj6vWnJk291tghgbhx EOS7fXnHFBpHLpQevKzdrfKffnAyK8vydPbmA6grXb6gFrezsELKa
```

## deploy the contract
`cleos set contract yx.token build/contracts/yx.token -p yx.token`

# Setting Fee for Operations
* Fee for operations is set by `setfee` operation.
* The majority of the current depositories must agree using multisig feature.
* [Temporary] For test convenience, this operation is not yet multisig.
```
cleos push action yx.token setfee '[ "regdepo", "10000.0000 DKRW" ]' -p yx.token
cleos push action yx.token setfee '[ "issue", "0.0000 DKRW" ]' -p yx.token
cleos push action yx.token setfee '[ "redeem", "1000.0000 DKRW" ]' -p yx.token
cleos push action yx.token setfee '[ "transfer", "5.0000 DKRW" ]' -p yx.token
```

# Setting user's permission
```
cleos set account permission user1 active '{"threshold": 1, "keys":[{"key":"EOS7NaaAkmNGEXa2UkgUmxdkwxA2tf8Tn7y9YmMykLEcSpTSc6tgk", "weight":1}], "accounts":[{"permission":{"actor":"yx.token","permission":"eosio.code"},"weight":1}], "waits":[] }' owner -p user1
```
* EOS7NaaAkmNGEXa2UkgUmxdkwxA2tf8Tn7y9YmMykLEcSpTSc6tgk is the user1's public key for active permission.
* This job is required for fee transfer with yx.ntoken.

# Operations

## register d2 as non-native token depository
* d2 wants to be the depository of BTC token with precision 4.
* KYC authentication for native token transfer of d2 must be set first because of fee. That also means d2 must have sufficient DKRW.
* d2 will set its BTC token with no authentication. d2 considers KYC unnecessary.
```
cleos push action yx.token regdepo '[ {"symbol":"4,BTC","issuer":"d2"}, 0 ]' -p d2
```

## issue non-native token to user2 by d2
```
cleos push action yx.token issue '[ "user2", {"quantity":"100000.0000 BTC","issuer":"d2"}, "memo" ]' -p d2
```

## redeem non-native token from user2 by d2
1. user2 transfers the native token to d2.
```
cleos push action yx.token transfer '[ "user2", "d2", {"quantity":"10000.0000 BTC","issuer":"d2"}, "memo" ]' -p user2
```
1. d2 checks the transfer operation is done and calls redeem operation.
```
cleos push action yx.token redeem '[ {"quantity":"10000.0000 BTC","issuer":"d2"}, "redeem for user2" ]' -p d2
```

## transfer non-native token from user1 to user2
```
cleos push action yx.token transfer '[ "user1", "user2", {"quantity":"10000.0000 BTC","issuer":"d2"}, "user1", "memo" ]' -p user1
```
### parameters of transfer operation
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `quantity` : token amount and symbol with issuer account; in case of native token, issuer is the empty string
   1. `fee payer` : account name which pays fee; usually the same as `from` account name
   1. `memo` : string less than or equal to 256 bytes

### if the fee payer account is different from 'from' account
* It requires the signature of the fee payer account.
```
cleos push action yx.token transfer '[ "user1", "user2", {"quantity":"10000.0000 BTC","issuer":"d2"}, "servprovider", "memo" ]' -p user1 servprovider
```

# Example
* At first, you must execute yx.ntoken example and must set fee for all operations.
* d2 wants to create and issue BTC with precision 4 token to user1.

## 1. With KYC
```
cleos push action yx.token regdepo '[ {"symbol":"4,BTC","issuer":"d2"}, 0 ]' -p d2
cleos push action yx.token printbalance '{"owner":"d2", "symbol":{"symbol":"4,DKRW","issuer":"system"}}' -p yx.token
cleos push action yx.token printbalance '{"owner":"yx.feedist", "symbol":{"symbol":"4,DKRW","issuer":"system"}}' -p yx.token
cleos push action yx.token issue '[ "user2", {"quantity":"100000.0000 BTC","issuer":"d2"}, "memo" ]' -p d2
cleos push action yx.token printbalance '{"owner":"user2", "symbol":{"symbol":"4,BTC","issuer":"d2"}}' -p yx.token
cleos push action yx.token transfer '[ "user2", "user1", {"quantity":"10000.0000 BTC","issuer":"d2"}, "user2", "memo" ]' -p user2
```

## 1. Without KYC like KYC_AUTHVECTOR_REAL_NAME_AUTH
* Creates user3 and don't set any user3's KYC information.
```
cleos push action yx.token regdepo '[ {"symbol":"8,ETH","issuer":"d2"}, 4 ]' -p d2

# KYC error!
cleos push action yx.token issue '[ "user3", {"quantity":"100.00000000 ETH","issuer":"d2"}, "memo" ]' -p d2
```