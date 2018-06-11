# Abstract
* Through yx.token contract, several depositories can issue, redeem token with the same symbol. For example, d1(depository 1) and d2 can issue and redeem DKRW seperately. 
  * For redeem operation, only depositories can do it and burn DKRW of the specific account issued by each one.
* Through the contract, blockchain users can transfer tokens regardless of issuers. There would be two general cases.
  1. When user1 has 1000 DKRW/d1 and 1000 DKRW/d2, total 2000 DKRW, and transfers 2000 DKRW to user2, user2 will have 1000 DKRW/d1 and 1000 DKRW/d2.
  1. When user1 has 1000 DKRW/d1 and 1000 DKRW/d2, total 2000 DKRW, and transfers 1500 DKRW to user2, user2 will have 1000 DKRW/d1 and 500 DKRW/d2 or 500 DKRW/d1 and 1000 DKRW/d2 randomly but 1500 DKRW in total.
* yx.exchange (our orderbook-based exchange contract) uses the yx.token contract in the deposit & withdraw operations.
   * The deposit operation tranfers from the user account to the yx.exchange account on the yx.token contract.
   * The withdraw operaion does the opposite.

# To build yx.token contract
1. locate yx.token directory under EOS_HOME/contracts
1. add `add_subdirectory(yx.token)` to EOS_HOME/contracts/CMakeLists.txt
1. run eosio_build.sh

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

## DKRW token creation by d1 & d2
```
cleos push action yx.token create '[ "d1", "1000000000 DKRW", 0, 0, 0]' -p yx.token
cleos push action yx.token create '[ "d2", "1000000000 DKRW", 0, 0, 0]' -p yx.token
```

## Issue DKRW/d1 & DKRW/d2 token to user1
```
cleos push action yx.token issue '[ "user1", {"quantity":"100 DKRW","contract":"d1"}, "memo" ]' -p d1
cleos push action yx.token issue '[ "user1", {"quantity":"100 DKRW","contract":"d2"}, "memo" ]' -p d2
```

## Check balance (console log)
`cleos push action yx.token printbalance '{"owner":"user1", "symbol":{"symbol":"0,DKRW","contract":"d1"}}' -p yx.token`

## Trasfer DKRW from user1 to user2
```
cleos push action yx.token transfer '[ "user1", "user2", "150 DKRW", "m" ]' -p user1
cleos push action yx.token transfer '[ "user1", "user2", "50 DKRW", "m" ]' -p user1
```

## Redeem DKRW
```
cleos push action yx.token redeem '[ "user2", {"quantity":"100 DKRW","contract":"d1"}, "redeem for user2" ]' -p d1
cleos push action yx.token redeem '[ "user2", {"quantity":"100 DKRW","contract":"d2"}, "redeem for user2" ]' -p d2
```