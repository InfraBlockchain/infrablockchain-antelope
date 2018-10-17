# To build yex contract
1. locate yex directory under EOS_HOME/contracts
1. add `add_subdirectory(yex)` to EOS_HOME/contracts/CMakeLists.txt
1. run eosio_build.sh

# To test yex contract

* Assume that the eosio.bios contract and the eosio.token contract are prepared.
   * https://github.com/EOSIO/eos/wiki/Tutorial-Getting-Started-With-Contracts
   * https://github.com/EOSIO/eos/wiki/Tutorial-eosio-token-Contract

## unlock wallet
cleos wallet unlock --password \<your-wallet-password\>

## yex contract
```
cleos wallet import 5JaiqS7QMibLc8jf7jw75by7ZcFGu78ifBp8fPudHYNDRjq9NtC
cleos wallet import 5KNZguojmRLQzy5oUzs3zK3acQYNmzAk1YU1vJ9nrr7834YR84m
cleos create account eosio yex EOS7rW3MpWQg64p5NdNEw453Jh1EsuYndjxpfZ1cZpMfRvs734zwH EOS7rW3MpWQg64p5NdNEw453Jh1EsuYndjxpfZ1cZpMfRvs734zwH
```

## deploy yex contract
`cleos set contract yex build/contracts/yex -p yex`

## user1
```
cleos wallet import 5JUFNtseKzkjxaqA69GYX4e4eDuTR5p2P9bCkXWEjWVDdNMEAAV
cleos wallet import 5J8mZ6sqVXZzkPtzQ9pRrpTzXJm7amBUfXfja6KGNjSMWnMFPG4
cleos create account eosio user1 EOS5Mi1wGid545qYfUNJaMY1qmrHRF51z3StnCP5usDTFymgTqvD6 EOS7NaaAkmNGEXa2UkgUmxdkwxA2tf8Tn7y9YmMykLEcSpTSc6tgk
cleos set account permission user1 active '{"threshold": 1,"keys": [{"key": "EOS7rW3MpWQg64p5NdNEw453Jh1EsuYndjxpfZ1cZpMfRvs734zwH","weight": 1}],"accounts": [{"permission":{"actor":"yex","permission":"active"},"weight":1}]}' owner -p user1
```

## user2
```
cleos wallet import 5K5jUcrT827UjQE497GuVHDF5tjJv2V2knheFThKaDjW54dNNrM
cleos wallet import 5KjqyFRQrNzxfjd6zj5MREBedLe2cZdbk4a5eUnojxshXmYbsZ7
cleos create account eosio user2 EOS7sCHT9Fw4h6q8DM9PC4kGgozCQbW1Gs4QdXRY29wJiUHu3uciU EOS68XGZHQ8TiQYMFUvXXgNbbTGu6pQ3fTgqmH16FBDrLtFzDyLa2
cleos set account permission user2 active '{"threshold": 1,"keys": [{"key": "EOS7rW3MpWQg64p5NdNEw453Jh1EsuYndjxpfZ1cZpMfRvs734zwH","weight": 1}],"accounts": [{"permission":{"actor":"yex","permission":"active"},"weight":1}]}' owner -p user2
```

## d1 (depository 1)
```
cleos wallet import 5KFi61BKiAR5XNXQLYhkkzLWwdWbXUzmekVzn3MmpZjT9X1n78N
cleos wallet import 5JiJiBo2mQBfx9cHTTieEA8aVVktqr1gond7SogG8dF9in1iFBS
cleos create account eosio d1 EOS884qE7rLpnf3cejF6pq4w5S8wp9PMhHmSViuxrrQmGpkFtDvy2 EOS6LPAQCVK69srvKmPCTyD6QjG4Z8t3YThJVTGeZeQg8MokC4YTq
```

## EOS token issueing for user1 & user2
```
cleos push action eosio.token create '[ "eosio", "1000000000 EOS", 0, 0, 0]' -p eosio.token
cleos push action eosio.token issue '[ "user1", "10000 EOS", "memo" ]' -p eosio
cleos push action eosio.token issue '[ "user2", "10000 EOS", "memo" ]' -p eosio
```
* check balance : cleos get currency balance eosio.token user1

## BTC token issueing for user1 & user2
```
cleos push action eosio.token create '[ "d1", "1000000000 BTC", 0, 0, 0]' -p eosio.token
cleos push action eosio.token issue '[ "user1", "10000 BTC", "memo" ]' -p d1
cleos push action eosio.token issue '[ "user2", "10000 BTC", "memo" ]' -p d1
```

## deposit for user1 & user2
* *Note that precision is not supported now.*
```
cleos push action yex deposit '{"from":"user1", "quantity":{"quantity":"10000 EOS","contract":"eosio"}}' -p user1
cleos push action yex deposit '{"from":"user1", "quantity":{"quantity":"10000 BTC","contract":"d1"}}' -p user1
cleos push action yex deposit '{"from":"user2", "quantity":{"quantity":"10000 EOS","contract":"eosio"}}' -p user2
cleos push action yex deposit '{"from":"user2", "quantity":{"quantity":"10000 BTC","contract":"d1"}}' -p user2
```
## buy orders by user1
* buy BTC using EOS as currency
```
cleos push action yex buy '{"buyer":"user1", "at_price":{"quantity":"1 EOS","contract":"eosio"}, "quantity":{"quantity":"1 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user1
cleos push action yex buy '{"buyer":"user1", "at_price":{"quantity":"2 EOS","contract":"eosio"}, "quantity":{"quantity":"1 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user1
cleos push action yex buy '{"buyer":"user1", "at_price":{"quantity":"3 EOS","contract":"eosio"}, "quantity":{"quantity":"1 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user1
cleos push action yex buy '{"buyer":"user1", "at_price":{"quantity":"4 EOS","contract":"eosio"}, "quantity":{"quantity":"1 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user1
cleos push action yex buy '{"buyer":"user1", "at_price":{"quantity":"5 EOS","contract":"eosio"}, "quantity":{"quantity":"1 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user1
cleos push action yex buy '{"buyer":"user1", "at_price":{"quantity":"3 EOS","contract":"eosio"}, "quantity":{"quantity":"1 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user1
cleos push action yex buy '{"buyer":"user1", "at_price":{"quantity":"4 EOS","contract":"eosio"}, "quantity":{"quantity":"1 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user1
cleos push action yex buy '{"buyer":"user1", "at_price":{"quantity":"5 EOS","contract":"eosio"}, "quantity":{"quantity":"1 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user1
cleos push action yex buy '{"buyer":"user1", "at_price":{"quantity":"3 EOS","contract":"eosio"}, "quantity":{"quantity":"1 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user1
```

## sell orders by user2
* sell BTC to earn EOS
```
cleos push action yex sell '{"seller":"user2", "at_price":{"quantity":"3 EOS","contract":"eosio"}, "quantity":{"quantity":"1 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user2
cleos push action yex sell '{"seller":"user2", "at_price":{"quantity":"3 EOS","contract":"eosio"}, "quantity":{"quantity":"2 BTC","contract":"d1"}, "expiration":"2018-05-31T02:49:57.000"}' -p user2
```

# To print order book
* Please check nodeos console log.

## Buy order book based on price 3 EOS (price index)
```
cleos push action yex printprindex '{"buy_or_sell":"1", "token":"0 BTC", "at_price":"3 EOS"}' -p yex
```
## Buy order book based on price 1 EOS (price index)
```
cleos push action yex printprindex '{"buy_or_sell":"1", "token":"0 BTC", "at_price":"1 EOS"}' -p yex
```
## Sell order book based on price 3 EOS (price index)
```
cleos push action yex printprindex '{"buy_or_sell":"0", "token":"0 BTC", "at_price":"3 EOS"}' -p yex
```

## Simple list-up
```
cleos get table yex 6071415247654309701 buybook
cleos get table yex 6071415247654309701 sellbook
```
* 6071415247654309701 is the scope and the uint64 representation of the string `BTCEOS`.

# To cancel all orders of buy/sell order book
```
cleos push action yex cancelall '{"token":"0 BTC", "currency":"0 EOS", "buy":"1", "sell":"0"}' -p yex
cleos push action yex cancelall '{"token":"0 BTC", "currency":"0 EOS", "buy":"0", "sell":"1"}' -p yex
cleos push action yex cancelall '{"token":"0 BTC", "currency":"0 EOS", "buy":"1", "sell":"1"}' -p yex
```

# ETC.
## setup eosio account
cleos wallet import 5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3

## set bios contract
```
cleos set contract eosio build/contracts/eosio.bios -p eosio
```

## eosio.token contract
```
cleos wallet import 5JUFNtseKzkjxaqA69GYX4e4eDuTR5p2P9bCkXWEjWVDdNMEAAV
cleos wallet import 5J8mZ6sqVXZzkPtzQ9pRrpTzXJm7amBUfXfja6KGNjSMWnMFPG4
cleos create account eosio eosio.token EOS5Mi1wGid545qYfUNJaMY1qmrHRF51z3StnCP5usDTFymgTqvD6 EOS7NaaAkmNGEXa2UkgUmxdkwxA2tf8Tn7y9YmMykLEcSpTSc6tgk
cleos set contract eosio.token build/contracts/eosio.token -p eosio.token
```

## account permission for deposit & withdrawal
```
cleos set account permission user1 active '{"threshold": 1,"keys": [{"key": "EOS7rW3MpWQg64p5NdNEw453Jh1EsuYndjxpfZ1cZpMfRvs734zwH","weight": 1}],"accounts": [{"permission":{"actor":"yex","permission":"active"},"weight":1}]}' owner -p user1
```
* Note that EOS7rW3MpWQg64p5NdNEw453Jh1EsuYndjxpfZ1cZpMfRvs734zwH is the public key of the 'yex' account.
