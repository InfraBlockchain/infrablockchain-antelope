# Abtract
* This contract is for native token.
* The contract for non-native tokens is [`yx.token`](../../contracts/yx.token/). It's recommended to read about the native token first.

# Types of Tokens
* There are two types of tokens in Yosemite Public Blockchain.
  1. native token
  1. non-native tokens
* The native token supports only one type of FIAT token. First of all, it is used as the transaction fee.
* The non-native tokens are created by anyone who owns his or her account. It is made of the symbol and the issuer account. e.g. 4,BTC@d1
  * They are all different for each token issuer. For example, 4,BTC/d1 and 4,BTC/d2 are both different.

# Types of Token Operations
* There are two types of operations.
  1. for depositories
  1. for any users

# What You Can Do With This Contract
* Only system depositories can issue and redeem the native token.
  * For example, if the native token is DKRW, the system depository account d1(depository 1) and d2 can issue and redeem DKRW separately.
  * If a new depository wants to issue the native token, it must call [yx.system](../../contracts/yx.system/)::regsysdepo operation first to become the system depository.
  * The active block producers must authrorize it as the system depository.
* Blockchain users can transfer the native token regardless of depositories. There would be two general cases.
  1. When user1 has 1000 DKRW/d1 and 1000 DKRW/d2, total 2000 DKRW, and transfers 2000 DKRW to user2, user2 will have 1000 DKRW/d1 and 1000 DKRW/d2.
  1. When user1 has 1000 DKRW/d1 and 1000 DKRW/d2, total 2000 DKRW, and transfers 1500 DKRW to user2, user2 will have 1000 DKRW/d1 and 500 DKRW/d2 or 500 DKRW/d1 and 1000 DKRW/d2 randomly but 1500 DKRW in total.
* Blockchain users can transfer the native token designating the depository.

# Before Using This Contract

## setting fee for operations
* Transaction fee for operations is set by [yx.txfee](../../contracts/yx.txfee/)::settxfee operation.
```
cleos push action yx.txfee settxfee '{"operation":"tf.nissue", "fee":"0.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.nredeem", "fee":"1000.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.ntransfer", "fee":"10.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.transfer", "fee":"20.0000 DKRW"}}' -p eosio
```

## setkycrule : setting KYC vector
* Active block producers can set KYC vector to determine who can send, receive, or do both the native token with `setkycrule` operation.
```
cleos push action yx.ntoken setkycrule '{"type":0, "kyc":15}' -p eosio
cleos push action yx.ntoken setkycrule '{"type":1, "kyc":15}' -p eosio
```

### parameters of setkycrule
1. type
   * 0 : transfer send
   * 1 : transfer receive
1. kyc
   * flags from [yx.identity](../../contracts/yx.identity/)

# Actions

## nissue : issue native token to an account by the system depository
* d1 must be the system depository registered and authroized by [yx.system](../../contracts/yx.system/).
* In this example, 4,DKRW is the native token.
```
$ cleos push action yx.ntoken nissue '{"to":"user1", "token":{"amount":"100000.0000 DKRW","issuer":"d1"}, "memo":"my memo"}' -p d1
```

### parameters of nissue
1. to : the account who is transferred the token
1. token : the amount of native token with the issuer(=system depository)
   * The `amount` must be formatted to the number with the exact precision plus the symbol name.
1. memo : the additional data set by the action caller

### notification of nissue
* If the to account is different from the issuer, nissue does the inline action for `ntransfer`.
```
#     yx.ntoken <= yx.ntoken::nissue            {"to":"user1","token":{"amount":"100000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"d1","to":"user1","token":{"amount":"100000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#            d1 <= yx.ntoken::ntransfer         {"from":"d1","to":"user1","token":{"amount":"100000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#         user1 <= yx.ntoken::ntransfer         {"from":"d1","to":"user1","token":{"amount":"100000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
```

## nredeem : redeem native token from an account by the system depository
* At first, the account transfers the native token to the system depository.
```
cleos push action yx.ntoken transfer '{"from":"user1", "to":"d1", "amount":"10000.0000 DKRW", "memo":"my memo"}' -p user1
```
* Then the system depository checks the transfer action is done and calls nredeem action.
```
cleos push action yx.ntoken nredeem '{"token":{"amount":"10000.0000 DKRW","issuer":"d1"}, "memo":"my memo"}' -p d1
```

### parameters of nredeem
1. token : the amount of native token with the issuer(=system depository)
1. memo : the additional data set by the action caller

### notification of nredeem

## transfer, wptransfer : transfer the native token regardless of the system depository
### transfer action
* DKRW tokens issued by any system depositories can be transferred.
```
cleos push action yx.ntoken transfer '{"from":"user1", "to":"user2", "amount":"10000.0000 DKRW", "memo":"my memo"}' -p user1
```
### parameters of transfer
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `amount` : token amount and symbol
   1. `memo` : string less than or equal to 256 bytes

### notification of transfer
```
#     yx.ntoken <= yx.ntoken::transfer          {"from":"user1","to":"user2","amount":"10000.0000 DKRW","memo":"memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"user1","token":"20.0000 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"user1","token":{"amount":"20.0000 DKRW","issuer":"d1"}}
#         user1 <= yx.ntoken::feetransfer       {"payer":"user1","token":{"amount":"20.0000 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::feetransfer       {"payer":"user1","token":{"amount":"20.0000 DKRW","issuer":"d1"}}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"memo"}
#         user1 <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"memo"}
#         user2 <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"memo"}
```

### use ntransfer action
* would be used by depository only
```
cleos push action yx.ntoken ntransfer '[ "user1", "user2", {"amount":"10000.0000 DKRW","issuer":"d1"}, "memo" ]' -p user1
```

### parameters of ntransfer
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `token` : token amount and symbol with issuer account
   1. `memo` : string less than or equal to 256 bytes

### use wptransfer action if the fee payer account is different from 'from' account for `transfer` action
* It requires the signature of the fee payer account.
* 'wp' means 'with payer'.
```
cleos push action yx.ntoken wptransfer '[ "user1", "user2", "10000.0000 DKRW", "servprovider", "memo" ]' -p user1 servprovider
```

### parameters of wptransfer
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `amount` : token amount and symbol
   1. `fee payer` : account name which pays transaction fee
   1. `memo` : string less than or equal to 256 bytes

### use wpntransfer action if the fee payer account is different from 'from' account for `ntransfer` action
```
cleos push action yx.ntoken wpntransfer '[ "user1", "user2", {"amount":"10000.0000 DKRW","issuer":"d1"}, "servprovider", "memo" ]' -p user1 servprovider
```

### parameters of wpntransfer
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `token` : token amount and symbol with issuer account
   1. `fee payer` : account name which pays transaction fee
   1. `memo` : string less than or equal to 256 bytes

## get the stats of native token per depository
* d1 is the name of system depository account.
```
cleos get table yx.ntoken d1 ntstats
```

## get the native token balance of the user for each depository
* For example, user1 is the name of user account.
```
cleos get table yx.ntoken user1 ntaccounts
```

## get the sum of native token balance for all system depositories of the user
* For example, user1 is the name of user account.
```
cleos get table yx.ntoken user1 ntaccountstt
```

# Example
* Let's say d1 is the depository for DKRW with precision 4, the native token of the blockchain.
* First of all, you must set fee for all actions and d1, d2, user1 and user2 accounts are KYC'ed.

```
#register d1 as system depository
cleos push action eosio regsysdepo '["d1","http://sysdepo.org",1]' -p d1 eosio
cleos push action eosio authsysdepo '["d1"]' -p eosio

cleos push action yx.ntoken nissue '[ "d1", {"amount":"1000000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
cleos push action yx.ntoken nissue '[ "d2", {"amount":"100000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
cleos get table yx.ntoken d1 ntstats
cleos get table yx.ntoken d1 ntaccounts
cleos get table yx.ntoken d2 ntaccounts
cleos push action yx.ntoken transfer '[ "d1", "user1", "10000.0000 DKRW", "memo"]' -p d1
cleos push action yx.ntoken ntransfer '[ "d1", "user1", {"amount":"10000.0000 DKRW","issuer":"d1"}, "d1", "memo" ]' -p d1
cleos get table yx.ntoken user1 ntaccounts
cleos get table yx.ntoken user1 ntaccountstt
cleos push action yx.ntoken wptransfer '[ "user1", "user2", "10000.0000 DKRW", "d2", "memo" ]' -p user1 d2
cleos get table yx.ntoken user1 ntaccounts
cleos get table yx.ntoken d2 ntaccounts
cleos get table yx.ntoken user2 ntaccounts
cleos push action yx.ntoken wpntransfer '[ "user1", "user2", {"amount":"10000.0000 DKRW","issuer":"d1"}, "d2", "memo" ]' -p user1 d2
cleos get table yx.ntoken user1 ntaccounts
cleos get table yx.ntoken d2 ntaccounts
cleos get table yx.ntoken user2 ntaccounts

# error transactions
cleos push action yx.ntoken ntransfer '[ "d1", "user1", {"amount":"100000000.0000 DKRW","issuer":"d1"}, "d1", m"]' -p d1
cleos push action yx.ntoken ntransfer '[ "d1", "user1", {"amount":"10000.0000 DKRW","issuer":"d2"}, "d1", "m" ]' -p d1
cleos push action yx.ntoken transfer '[ "user1", "user2", "10000.0000 DKRW", "d2", "m" ]' -p user1
```