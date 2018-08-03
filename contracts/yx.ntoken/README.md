# Abtract
* This Yosemite contract is for native token.
* The Yosemite contract for non-native tokens is [`yx.token`](../../contracts/yx.token/). It's recommended to read about the native token first.

# Yosemite Tokens
## Types of Token
* There are two types of tokens in Yosemite Public Blockchain(YosemiteChain or Yosemite).
  1. native token
  1. non-native tokens
* The native token must be a FIAT-token, pegged to 1:1 exchange rate. This is designed to be used as the network's transaction fee and the incentive for the block producers.
   * Only the system depositories issue and redeem the native token. For more information, refer to [yx.system](../../contracts/yx.system/).
   * Only the blockchain users who have successfully verified their identity by completing the KYC process can transfer the native token. For more information, refer to [yx.identity](../../contracts/yx.identity/).
* The non-native tokens are created by anyone who owns his or her blockchain account. For more information, refer to [`yx.token`](../../contracts/yx.token/).

## Format of Token
* The structure of token includes precision, symbol, and the issuer.
* 4,DKRW@d1   
   * 4 : precision (the number of bits used to hold the fractional part in the concept of floating-point numbers, .0000) 
   * DKRW : symbol
   * d1 : account name of the issuer
* Different issuer results in different tokens. For example, 4,DKRW@d1 and 4,DKRW@d2 are different.
* Different precision results in different tokens. For example, 4,BTC@d3 and 8,BTC@d3 are different.
## Amount Format
* 1234.5678 DKRW
   * If token's symbol and precision is 4,DKRW, then the amount should be written to reflect token's precision. e.g. 1.0000 DKRW
* In the Yosemite software, the amount 1234.5678 is saved in 64-bit integer as 12345678.
* The maximum amount defined by the software is 2^62-1(=4611686018427387903). In such case, up to 461168601842738.7903 DKRW is usuable.

# Yosemite Native Token
* From the usage perspective, DKRW is DKRW regardless of whether it's 4,DKRW@d1 or 4,DKRW@d2.
* For the native token, YosemiteChain defines its symbol and precision at the Yosemite software release to conform to such perspective.
   * In the specific YosemiteChain network for the Korea market, 4,DKRW is the native token. There can't be 8,DKRW.
   * It also means there would be the Yosemite software with 8,DKRW and the specific network with it. But it's the totally different network.
   * There would be the Yosemite software with 4,DUSD or 4,EUR. These softwares can be made with CMake build options, YOSEMITE_NATIVE_TOKEN_SYMBOL_NAME and YOSEMITE_NATIVE_TOKEN_PRECISION. Refer to [root CMakeLists.txt](../CMakeLists.txt) file.
   * **Note that only this Yosemite contract, `yx.ntoken`, manages the native token.**
      * YosemiteChain has a plan to provide the general smart contract platform in the near future. At that time, one of the Yostemite accounts would make the smart contract which is named to `xx.ntoken` and it would manage 4,DKRW. But 4,DKRW managed by `xx.token` is not the native token but the non-native token.
* In spite of importance of alignment with the usage perspective, it would be undesirable if there were only one issuer of the native token. Above all, it might not be what blockchain users want that the native token is controlled by one entity, which it's definitely centralized.
* YosemiteChain allows multiple issuers for the native token. Native token issuer, which is named to the system depository, is registered by [yx.system](../../contracts/yx.system/).
* This Yosemite contract manages the native token issued by multiple issuers to be viewed as the same token, which are actually different each other at the software level.
   * For example, even though the account user1 has 1000.0000 DKRW@d1 and 1000.0000 DKRW@d2 at the software level, this Yosemite contract shows the person or the DApp provider who owns the account user1 2000.0000 DRKW.

# What You Can Do With This Yosemite Contract
* Only system depositories can issue and redeem the native token.
  * For example, if the native token is DKRW, the system depository account d1(depository 1) and d2 can issue and redeem DKRW separately.
  * If a new depository wants to issue the native token, it must call [yx.system](../../contracts/yx.system/)::regsysdepo action first to become the system depository.
     * The active block producers must authrorize it as the system depository.
* One system depository can allow redemption of other system depositories. Fore more information, refer to `nredeem` action.
* Blockchain users can transfer the native token regardless of depositories. There would be two general cases.
  1. When user1 has 1000.0000 DKRW@d1 and 1000.0000 DKRW@d2, total 2000.0000 DKRW, and transfers 2000.0000 DKRW to user2, user2 will have 1000.0000 DKRW@d1 and 1000.0000 DKRW@d2.
  1. When user1 has 1000.0000 DKRW@d1 and 1000.0000 DKRW@d2, total 2000.0000 DKRW, and transfers 1500.0000 DKRW to user2, user2 will have 1000.0000 DKRW@d1 and 500.0000 DKRW@d2 or 500.0000 DKRW@d1 and 1000.0000 DKRW@d2 randomly but 1500.0000 DKRW in total.
* Blockchain users can transfer the native token designating the specific depository.

# Management Actions

## setting fee for operations
* Transaction fee for operations is set by [yx.txfee](../../contracts/yx.txfee/)::settxfee operation.
```
cleos push action yx.txfee settxfee '{"operation":"tf.nissue", "fee":"0.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.nredeem", "fee":"1000.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.ntransfer", "fee":"10.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.transfer", "fee":"20.0000 DKRW"}}' -p eosio
```

## setkycrule : setting KYC vector
* Active block producers sets KYC vector to determine who can send, receive, or do both the native token with `setkycrule` operation.
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

## nissue
Issue native token to an account by the system depository

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

### inline actions and notifications of nissue
* Case 1. If the to account is different from the issuer, nissue does the inline action for `ntransfer`.
   * Note that in this example the transaction fee of nissue is set to 0.0000 DKRW, so the inline action for `payfee` is now shown.
   * In the example below, the `yx.ntoken <= yx.ntoken::ntransfer` part is the `ntrasfer` inline action. `d1 <= yx.ntoken::ntransfer` and `user1 <= yx.ntoken::ntransfer` parts are notifications of the inline action.
```
#     yx.ntoken <= yx.ntoken::nissue            {"to":"user1","token":{"amount":"100000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"d1","to":"user1","token":{"amount":"100000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#            d1 <= yx.ntoken::ntransfer         {"from":"d1","to":"user1","token":{"amount":"100000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#         user1 <= yx.ntoken::ntransfer         {"from":"d1","to":"user1","token":{"amount":"100000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
```
* Case 2. If the to account is the system depository itself, there is no `ntransfer` action.
```
#     yx.ntoken <= yx.ntoken::nissue            {"to":"d1","token":{"amount":"100000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
```

## nredeem
Redeem native token from an account by the system depository

* At first, the account transfers the native token to the system depository.
```
cleos push action yx.ntoken transfer '{"from":"user1","to":"d1","amount":"10000.0000 DKRW","memo":"my memo"}' -p user1
```
   * The account user1 can send the native token issued by other system depositories like d2 to the system depository d1. In such case, d1 will call the transfer action to d2 to redeem the native token from d2 at any time. It's natural to request redemption between system depositories.
* Then the system depository checks the transfer action is irreversible and calls nredeem action.
```
cleos push action yx.ntoken nredeem '{"token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"my memo"}' -p d1
```

### parameters of nredeem
1. token : the amount of native token with the issuer(=system depository)
1. memo : the additional data set by the action caller

### inline actions and notifications of nredeem
* For all actions which requires the fee bigger than 0.0000 DKRW, the inline action for `payfee` is executed.
* The `payfee` action is notified to the payer acount and yx.txfee account.
```
#     yx.ntoken <= yx.ntoken::nredeem           {"token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"d1","token":{"amount":"1000.0000 DKRW","issuer":"d1"}}
#            d1 <= yx.ntoken::payfee            {"payer":"d1","token":{"amount":"1000.0000 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"d1","token":{"amount":"1000.0000 DKRW","issuer":"d1"}}
```

## transfer
Transfer the native token regardless of the system depository

* The native token issued by any system depositories can be transferred.
```
cleos push action yx.ntoken transfer '{"from":"user1","to":"user2","amount":"10000.0000 DKRW","memo":"my memo"}' -p user1
```
### parameters of transfer
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `amount` : token amount and symbol
   1. `memo` : string less than or equal to 256 bytes

### inline actions and notifications of transfer
* Case 1. If the `from` account has the enough amount for the specific depository, only one inline action for `ntransfer` is executed.
   * In this example, user1 has more than or equal to 10000.0000 DKRW@d1 in its account.
```
#     yx.ntoken <= yx.ntoken::transfer          {"from":"user1","to":"user2","amount":"10000.0000 DKRW","memo":"my memo"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#         user1 <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#         user2 <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"20.0000 DKRW","issuer":"d1"}}
#         user1 <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"20.0000 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"20.0000 DKRW","issuer":"d1"}}
```
* Case 2. There's two or more inline actions for `ntransfer`.
   * In this example, user1 has 7000.0000 DKRW@d1 and 4000.0000 DKRW@d2 in its account. d2 is another system depository.
   * There are two `ntransfer` inline actions and `payfee` inline action is executed with 20.0000 DKRW@d2.
```
#     yx.ntoken <= yx.ntoken::transfer          {"from":"user1","to":"user2","amount":"10000.0000 DKRW","memo":"my memo"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"7000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#         user1 <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"7000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#         user2 <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"7000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"3000.0000 DKRW","issuer":"d2"},"memo":"my memo"}
#         user1 <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"3000.0000 DKRW","issuer":"d2"},"memo":"my memo"}
#         user2 <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"3000.0000 DKRW","issuer":"d2"},"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"20.0000 DKRW","issuer":"d2"}}
#         user1 <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"20.0000 DKRW","issuer":"d2"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"20.0000 DKRW","issuer":"d2"}}
```

## wptransfer
Transfer the native token regardless of the system depository with the designated fee payer

* If the fee payer account is different from 'from' account, you should use this action.
* 'wp' means 'with payer'.
* It requires the signature of the fee payer account.
```
cleos push action yx.ntoken wptransfer '{"from":"user1","to":"user2","amount":"10000.0000 DKRW","payer":"servprovider","memo":"my memo"}' -p user1 servprovider
```

### parameters of wptransfer
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `amount` : token amount and symbol
   1. `payer` : account name which pays transaction fee
   1. `memo` : string less than or equal to 256 bytes

### inline actions and notifications of wptransfer
* You can see that `payer` of `payfee` inline action is the servprovider account.
```
#     yx.ntoken <= yx.ntoken::wptransfer        {"from":"user1","to":"user2","amount":"10000.0000 DKRW","payer":"servprovider","memo":"my memo"}
#     yx.ntoken <= yx.ntoken::wpntransfer       {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"payer":"servprovide...
#         user1 <= yx.ntoken::wpntransfer       {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"payer":"servprovide...
#         user2 <= yx.ntoken::wpntransfer       {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"payer":"servprovide...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"servprovider","token":{"amount":"20.0000 DKRW","issuer":"d1"}}
#  servprovider <= yx.ntoken::payfee            {"payer":"servprovider","token":{"amount":"20.0000 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"servprovider","token":{"amount":"20.0000 DKRW","issuer":"d1"}}
```

## ntransfer
Transfer the native token designating the system depository

```
cleos push action yx.ntoken ntransfer '{"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"my memo"}' -p user1
```

### parameters of ntransfer
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `token` : token amount and symbol with issuer account
   1. `memo` : string less than or equal to 256 bytes

### inline actions and notifications of ntransfer
* There would be no inline action other than `payfee` inline action when the fee is set to more than 0.0000 DKRW.
* In this example, the fee is set to 10.0000 DKRW. So the `payfee` inline action is shown.
```
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#         user1 <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#         user2 <= yx.ntoken::ntransfer         {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"10.0000 DKRW","issuer":"d1"}}
#         user1 <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"10.0000 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"10.0000 DKRW","issuer":"d1"}}
```

### wpntransfer
Transfer the native token designating the system depository with the designated fee payer

* The purpose and requirement of this action is the same with `wptransfer` action.
```
cleos push action yx.ntoken wpntransfer '{"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"payer":"servprovider","memo":"my memo"}' -p user1 servprovider
```

### parameters of wpntransfer
* Parameters
   1. `from` : account name to transfer from
   1. `to` : account name to transfer to
   1. `token` : token amount and symbol with issuer account
   1. `payer` : account name which pays transaction fee
   1. `memo` : string less than or equal to 256 bytes

### inline actions and notifications of wpntransfer
* There would be no inline action other than `payfee` inline action when the fee is set to more than 0.0000 DKRW.
* In this example, the fee is set to 10.0000 DKRW. So the `payfee` inline action is shown.
* You can see that `payer` of `payfee` inline action is the servprovider account.
```
#     yx.ntoken <= yx.ntoken::wpntransfer       {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"payer":"servprovide...
#         user1 <= yx.ntoken::wpntransfer       {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"payer":"servprovide...
#         user2 <= yx.ntoken::wpntransfer       {"from":"user1","to":"user2","token":{"amount":"10000.0000 DKRW","issuer":"d1"},"payer":"servprovide...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"servprovider","token":{"amount":"10.0000 DKRW","issuer":"d1"}}
#  servprovider <= yx.ntoken::payfee            {"payer":"servprovider","token":{"amount":"10.0000 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"servprovider","token":{"amount":"10.0000 DKRW","issuer":"d1"}}
```

# Tables

## ntstats 
Get the stats of native token for the system depository

* If d1 is the name of system depository, its current supply information is shown.
```
cleos get table yx.ntoken d1 ntstats
```
### Result of ntstats
```
{
  "rows": [{
      "key": "basicstats",
      "supply": "35169900000",
      "options": 0
    }
  ],
  "more": false
}
```

## ntaccounts
Get the native token balance of the user for all system depositories

```
cleos get table yx.ntoken user1 ntaccounts
```
### Result of ntaccounts
* The user1 account has 50000.0000 DKRW@d1 and 1000.0000 DKRW@d2.
```
{
  "rows": [{
      "depository": "d1",
      "amount": 500000000
    },{
      "depository": "d2",
      "amount": 10000000
    }
  ],
  "more": false
}
```

## ntaccountstt
Get the sum of native token balance for all system depositories of the user

```
cleos get table yx.ntoken user1 ntaccountstt
```
### Result of ntaccounts
```
{
  "rows": [{
      "amount": 510000000
    }
  ],
  "more": false
}
```

# Execution Example
* Let's say d1 is the system depository for DKRW with precision 4, the native token of the blockchain.
```
cleos push action eosio regsysdepo '["d1","http://sysdepo.org",1]' -p d1 eosio
cleos push action eosio authsysdepo '["d1"]' -p eosio
```
* First of all, you must set transaction fee for all actions and d1, d2, user1 and user2 accounts are KYC'ed.
* Here's the example:
```
cleos push action yx.ntoken nissue '[ "d1", {"amount":"1000000.0000 DKRW","issuer":"d1"}, "memo" ]' -p d1
cleos get table yx.ntoken d1 ntstats
cleos get table yx.ntoken d1 ntaccounts
cleos push action yx.ntoken transfer '[ "d1", "user1", "10000.0000 DKRW", "memo"]' -p d1
cleos push action yx.ntoken ntransfer '[ "d1", "user1", {"amount":"10000.0000 DKRW","issuer":"d1"}, "d1", "memo" ]' -p d1
cleos get table yx.ntoken user1 ntaccounts
cleos get table yx.ntoken user1 ntaccountstt
cleos push action yx.ntoken wptransfer '[ "user1", "user2", "10000.0000 DKRW", "d1", "memo" ]' -p user1 d1
cleos get table yx.ntoken user1 ntaccounts
cleos get table yx.ntoken d1 ntaccounts
cleos get table yx.ntoken user2 ntaccounts
cleos push action yx.ntoken wpntransfer '[ "user1", "user2", {"amount":"10000.0000 DKRW","issuer":"d1"}, "d1", "memo" ]' -p user1 d1
cleos get table yx.ntoken user1 ntaccounts
cleos get table yx.ntoken d1 ntaccounts
cleos get table yx.ntoken user2 ntaccounts
```
