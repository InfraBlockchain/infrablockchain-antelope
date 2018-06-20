# Abstract
* Yosemite blockchain supports only one type of FIAT token as its native token. It is used as the fee of operations. 
* Through yx.token contract, several depositories can issue, redeem the native token.
  * For example, if the native token is DKRW, d1(depository 1) and d2 can issue and redeem DKRW separately.
  * If a new depository wants to issue the native token, it calls the create operation with multisig to propose that it wants to join in. The 2/3 of the current depositories must agree. 
* Through the contract, blockchain users can transfer the native token regardless of depositories. There would be two general cases.
  1. When user1 has 1000 DKRW/d1 and 1000 DKRW/d2, total 2000 DKRW, and transfers 2000 DKRW to user2, user2 will have 1000 DKRW/d1 and 1000 DKRW/d2.
  1. When user1 has 1000 DKRW/d1 and 1000 DKRW/d2, total 2000 DKRW, and transfers 1500 DKRW to user2, user2 will have 1000 DKRW/d1 and 500 DKRW/d2 or 500 DKRW/d1 and 1000 DKRW/d2 randomly but 1500 DKRW in total.
* Other than the native token, depositories or other parties can create/issue/redeem various tokens and blockchain users can transfer them. Those operations just require the fee.
  * Non-native tokens are all different from each token issuer.

# To test yx.token contract
* Assume that the eosio.bios contract and the eosio.token contract are prepared.
   * https://github.com/EOSIO/eos/wiki/Tutorial-Getting-Started-With-Contracts

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

# Native Token

## create native token by d1
* d1 wants to join as the depository.
```
cleos push action yx.token createn '[ "d1" ]' -p d1
```

## issue native token to user1 by d1
* In this example, DKRW/system is the native token.
```
cleos push action yx.token issuen '[ "user1", {"quantity":"100000.0000 DKRW","contract":"system"}, "d1", "memo" ]' -p d1
```

## redeem native token from user1 by d1
1. user1 transfers the native token to d1.
```
cleos push action yx.token transfer '[ "user1", "d1", {"quantity":"10000.0000 DKRW","contract":"system"}, "m" ]' -p user1
```
1. d1 checks the transfer operation is done and calls redeemn operation.
```
cleos push action yx.token redeemn '[ {"quantity":"10000.0000 DKRW","contract":"system"}, "d1", "memo" ]' -p d1
```

## transfer native token from user1 to user2
1. use normal transfer operation : DKRW tokens issued by many depositories can be tranferred.
```
cleos push action yx.token transfer '[ "user1", "user2", {"quantity":"10000.0000 DKRW","contract":"system"}, "m" ]' -p user1
```
1. use transfern operation
```
cleos push action yx.token transfern '[ "user1", "user2", {"quantity":"100000.0000 DKRW","contract":"system"}, "d1", "memo" ]' -p user1
```


# Non-native Token

## create non-native token by d2
* d2 wants to create BTC token with precision 4. 
```
cleos push action yx.token create '[ {"symbol":"4,BTC","contract":"d2"}]' -p d2
```

## issue non-native token to user2 by d2
```
cleos push action yx.token issue '[ "user2", {"quantity":"100000.0000 BTC","contract":"d2"}, "memo" ]' -p d2
```

## redeem non-native token from user2 by d2
1. user2 transfers the native token to d2.
```
cleos push action yx.token transfer '[ "user2", "d2", {"quantity":"10000.0000 BTC","contract":"d2"}, "m" ]' -p user2
```
1. d2 checks the transfer operation is done and calls redeem operation.
```
cleos push action yx.token redeem '[ {"quantity":"10000.0000 BTC","contract":"d2"}, "redeem for user2" ]' -p d2
```

## transfer non-native token from user1 to user2
```
cleos push action yx.token transfer '[ "user1", "user2", {"quantity":"10000.0000 BTC","contract":"d2"}, "m" ]' -p user1
```