# Abtract
* This contract is for non-native tokens.
* The contract for native token is [`yx.ntoken`](../../contracts/yx.ntoken/README.md). You must read it first.

# What You Can Do With This Contract
* An account which wants to register as the depository must call `create` operation with its token symbol and precision.
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

## setting the contract as privileged
```
cleos push action eosio setpriv '["yx.token", 1]' -p eosio
```

## setting fee for operations
* Fee for operations is set by `setfee` operation.
* The majority of the current depositories must agree using multisig feature.
* [Temporary] For test convenience, this operation is not yet multisig.
```
cleos push action yx.txfee settxfee '{"operation":"tf.tcreate", "fee":"10000.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.tissue", "fee":"100.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.tredeem", "fee":"100.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.ttransfer", "fee":"10.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.tsetkyc", "fee":"5.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.tsetopts", "fee":"5.0000 DKRW"}}' -p eosio
```

# Operations

## register d2 as non-native token depository
* d2 wants to be the depository of BTC token with precision 4.
* KYC authentication for native token transfer of d2 must be set first because of fee. That also means d2 must have sufficient DKRW.
* d2 will set its BTC token with no authentication. d2 considers KYC unnecessary.
```
cleos push action yx.token create '[ {"symbol":"4,BTC","issuer":"d2"}, 0 ]' -p d2
```

## issue non-native token to user2 by d2
```
cleos push action yx.token issue '[ "user2", {"asset":"100000.0000 BTC","issuer":"d2"}, "memo" ]' -p d2
```

## redeem non-native token from user2 by d2
1. user2 transfers the native token to d2.
```
cleos push action yx.token transfer '[ "user2", "d2", {"asset":"10000.0000 BTC","issuer":"d2"}, "memo" ]' -p user2
```
1. d2 checks the transfer operation is done and calls redeem operation.
```
cleos push action yx.token redeem '[ {"asset":"10000.0000 BTC","issuer":"d2"}, "redeem for user2" ]' -p d2
```

## transfer non-native token from user1 to user2
```
cleos push action yx.token transfer '[ "user1", "user2", {"asset":"10000.0000 BTC","issuer":"d2"}, "memo" ]' -p user1
```
### parameters of transfer operation
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `asset` : token amount and symbol with issuer account; in case of native token, issuer is the empty string
   1. `fee payer` : account name which pays fee; usually the same as `from` account name
   1. `memo` : string less than or equal to 256 bytes

### if the fee payer account is different from 'from' account
* It requires the signature of the fee payer account.
```
cleos push action yx.token wptransfer '[ "user1", "user2", {"asset":"10000.0000 BTC","issuer":"d2"}, "servprovider", "memo" ]' -p user1 servprovider
```

## setting KYC vector
* Rule type
   * 0 : transfer send (from account)
   * 1 : transfer receive (to account)
```
cleos push action yx.token setkycrule '{"token":{"symbol":"4,ETH","issuer":"d2"}, "type":0, "kyc":4}' -p d2
cleos push action yx.token setkycrule '{"token":{"symbol":"4,ETH","issuer":"d2"}, "type":1, "kyc":4}' -p d2
```

## setting token options
1. freeze token transfer
```
cleos push action yx.token setoptions '{"token":{"symbol":"4,BTC","issuer":"d2"}, "options":1, "overwrite":1}' -p d2
```

## get the token statistics
```
cleos get table yx.token 4,BTC tstats
```

## get all the token balances of the user
```
cleos get table yx.token user1 taccounts
```

# Example
* At first, you must execute yx.ntoken example and must set fee for all operations.
* d2 wants to create and issue BTC with precision 4 token to user1.

## 1. Without KYC
```
cleos push action yx.token create '[ {"symbol":"4,BTC","issuer":"d2"} ]' -p d2
cleos get table yx.token 4,BTC tstats
cleos push action yx.token issue '[ "user2", {"asset":"100000.0000 BTC","issuer":"d2"}, "memo" ]' -p d2
cleos get table yx.token d2 taccounts
cleos get table yx.token user2 taccounts
cleos push action yx.token transfer '[ "user2", "user1", {"asset":"10000.0000 BTC","issuer":"d2"}, "memo" ]' -p user2
cleos get table yx.token user1 taccounts
cleos get table yx.token user2 taccounts
```

## 1. With KYC
```
cleos push action yx.token create '[ {"symbol":"8,ETH","issuer":"d2"} ]' -p d2
cleos get table yx.token 8,ETH tstats
cleos push action yx.token setkycrule '{"token":{"symbol":"8,ETH","issuer":"d2"}, "type":0, "kyc":4}' -p d2
cleos push action yx.token setkycrule '{"token":{"symbol":"8,ETH","issuer":"d2"}, "type":1, "kyc":4}' -p d2
cleos get table yx.token 8,ETH tstats
```