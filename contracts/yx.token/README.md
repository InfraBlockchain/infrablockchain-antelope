# Abtract
* This Yosemite contract is for non-native tokens.
* The Yosemite contract for the native token is [`yx.ntoken`](../../contracts/yx.ntoken/README.md), which the specification of the Yosemite token is explained.

# What You Can Do With This Contract
* An entity who owns the Yosemite account can create a token with `create` operation. YosemiteChain calls the entity the token issuer or the token depository.
   * When creating the token, the issuer can enable the setting of some options like account freezing or token freezing.
* The token depository issues and redeems the token.
* The token depository can freeze the token transfer.
* Any Yosemite accounts who own the token can transfer it each other.

# Decision Points Before Creating Token
You must consider the decision points carefully because it cannot be changed once created.
1. About precision
   * The minimum precision is 2. It's enough to use in most cases.
   * For backing crypto-currencies like BTC or ETH, you might need 8.
1. About options
   * Once the token is created, you cannot change the ways you handle the token. Generally the options of the token are for token management.
   * The reason why you cannot change them is the users of Yosemite must know management rules of the token before they own it.
   * Please read carefully about [`can_set_options`](#parameters-of-create).

# Management Actions

## setting fee for operations
* Transaction fee for operations is set by [yx.txfee](../../contracts/yx.txfee/)::settxfee operation.
```
clyos push action yx.txfee settxfee '{"operation":"tf.tcreate", "fee":"10000.00 DKRW"}}' -p yosemite
clyos push action yx.txfee settxfee '{"operation":"tf.tissue", "fee":"100.00 DKRW"}}' -p yosemite
clyos push action yx.txfee settxfee '{"operation":"tf.tredeem", "fee":"100.00 DKRW"}}' -p yosemite
clyos push action yx.txfee settxfee '{"operation":"tf.ttransfer", "fee":"10.00 DKRW"}}' -p yosemite
clyos push action yx.txfee settxfee '{"operation":"tf.tsetkyc", "fee":"5.00 DKRW"}}' -p yosemite
clyos push action yx.txfee settxfee '{"operation":"tf.tsetopts", "fee":"5.00 DKRW"}}' -p yosemite
clyos push action yx.txfee settxfee '{"operation":"tf.tfreezeac", "fee":"5.00 DKRW"}}' -p yosemite
```

# Actions

## create
Create a (non-native) token with its symbol and precision

* The token creator naturally becomes the token depository.
* Enabling the setting of options cannot be done any more after creation, because the owners of the token should know about what options can be set and it must not be changed after they own.
```
clyos push action yx.token create '{"ysymbol":{"tsymbol":"4,BTC","issuer":"d2"},"can_set_options":0}' -p d2
```

### parameters of create
1. ysymbol : token symbol and its issuer
1. can_set_options : 16-bit flags of options
   * NONE : 0b0000000000000000 (0)
   * FREEZE_TOKEN_TRANSFER : 0b0000000000000001 (1)
      * The issuer can freeze or unfreeze the transfer of the token.
   * FREEZE_ACCOUNT : 0b0000000000000010 (2)
      * The issuer can freeze or unfreeze the token owners. They cannot transfer the token if they are frozen.
   * SET_KYC_RULE : 0b0000000000000100 (4)
      * The issuer can set KYC rules for the transfer of the token.
      * Only the account which passes the required KYC authentication can send or receive the token.
   * SET_ACCOUNT_TYPE_RULE : 0b0000000000001000 (8)
      * The issuer can set the account type for the transfer of the token.
      * For example, the account whose type is 'A' can only send to or receive from the account whose type is 'B'. The 'B' account can send to or receive from 'A' and the token issuer. That means only the 'B' can redeem the token.
      * Note that this feature will be implemented in the future version.

## issue
Issue the token to an account by the token depository

```
clyos push action yx.token issue '{"to":"user2","token":{"amount":"100000.0000 BTC","issuer":"d2"},"memo":"my memo"}' -p d2
```

### parameters of issue
1. to : the account who is transferred the token
1. token : the amount of token with the issuer(=token depository)
   * amount
   * issuer
1. memo : the additional data set by the action caller

### inline actions and notifications of issue
* Case 1. If the to account is different from the issuer, issue does the inline action for `transfer`.
```
#      yx.token <= yx.token::issue              {"to":"user2","token":{"amount":"100000.0000 BTC","issuer":"d2"},"memo":"my memo"}
#      yx.token <= yx.token::transfer           {"from":"d2","to":"user2","token":{"amount":"100000.0000 BTC","issuer":"d2"},"memo":"my memo"}
#            d2 <= yx.token::transfer           {"from":"d2","to":"user2","token":{"amount":"100000.0000 BTC","issuer":"d2"},"memo":"my memo"}
#         user2 <= yx.token::transfer           {"from":"d2","to":"user2","token":{"amount":"100000.0000 BTC","issuer":"d2"},"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"d2","token":{"amount":"100.00 DKRW","issuer":"d1"}}
#            d2 <= yx.ntoken::payfee            {"payer":"d2","token":{"amount":"100.00 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"d2","token":{"amount":"100.00 DKRW","issuer":"d1"}}
```

* Case 2. If the to account is the token depository itself, there is no `transfer` inline action.
```
#      yx.token <= yx.token::issue              {"to":"d2","token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"d2","token":{"amount":"100.00 DKRW","issuer":"d1"}}
#            d2 <= yx.ntoken::payfee            {"payer":"d2","token":{"amount":"100.00 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"d2","token":{"amount":"100.00 DKRW","issuer":"d1"}}
```

## redeem
Redeem(burn) token from an account by the token depository

* At, first the account transfers the token to its depository.
```
clyos push action yx.token transfer '{"from":"user2","to":"d2","token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"redeem by user2"}' -p user2
```
1. Then the token depository checks the transfer action is irreversible and calls redeem action.
```
clyos push action yx.token redeem '{"token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"redeem for user2"}' -p d2
```

### parameters of redeem
1. token : the amount of token with the issuer(=token depository)
   * amount
   * issuer
1. memo : the additional data set by the action caller


## transfer
Transfer token

```
clyos push action yx.token transfer '{"from":"user2","to":"user3","token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"my memo"}' -p user2
```

### parameters of transfer
1. from : account name to transfer from
1. to : account name to transfer to
1. token : the amount of token with the issuer(=token depository)
   * amount
   * issuer
1. memo : string less than or equal to 256 bytes

## wptransfer
Transfer token with the designated fee payer

```
clyos push action yx.token wptransfer '{"from":"user2","to":"user3","token":{"amount":"10000.0000 BTC","issuer":"d2"},"payer":"servprovider","memo":"my memo"}' -p user2
```

### parameters of wptransfer
1. from : account name to transfer from
1. to : account name to transfer to
1. token : the amount of token with the issuer(=token depository)
   * amount
   * issuer
1. payer : account name which pays transaction fee
1. memo : string less than or equal to 256 bytes


## setkycrule
Set the KYC vector for send or receive

* SET_KYC_RULE flag of can_set_options must be set at token creation time.
```
clyos push action yx.token setkycrule '{"ysymbol":{"tsymbol":"4,BTC","issuer":"d2"}, "type":0, "kyc":4}' -p d2
clyos push action yx.token setkycrule '{"ysymbol":{"tsymbol":"4,BTC","issuer":"d2"}, "type":1, "kyc":4}' -p d2
```

### parameters of setkycrule
1. ysymbol : token symbol and its issuer
1. type
   * 0 : transfer send
   * 1 : transfer receive
1. kyc : 16-bit flag from [yx.identity](../../contracts/yx.identity/)

## setoptions
set or add token options

* The token depository can freeze token transfer or unfreeze it with TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER flag.
   * FREEZE_TOKEN_TRANSFER flag of can_set_options must be set at token creation time.

```
clyos push action yx.token setoptions '{"ysymbol":{"tsymbol":"4,BTC","issuer":"d2"}, "options":1, "reset":1}' -p d2
```

### parameters of setoptions
1. ysymbol : token symbol and its issuer
1. options : 16-bit flags
   * 0b0000000000000000 : TOKEN_OPTIONS_NONE
   * 0b0000000000000001 : TOKEN_OPTIONS_FREEZE_TOKEN_TRANSFER
1. reset
   * 1 : reset
   * 0 : add (don't reset)

## freezeacc
Freeze an account which has the token by its issuer

* FREEZE_ACCOUNT flag of can_set_options must be set at token creation time.
```
clyos push action yx.token freezeacc '{"ysymbol":{"tsymbol":"4,BTC","issuer":"d2"}, "accs":["user1","user2"], "freeze":1}' -p d2
clyos push action yx.token freezeacc '{"ysymbol":{"tsymbol":"4,BTC","issuer":"d2"}, "accs":["user2"], "freeze":0}' -p d2
```

### parameters of freezeacc
1. ysymbol : token symbol and its issuer
1. accs : this list of account names
1. freeze : freeze or unfreeze


# Tables

## tstats
Get the token statistics
```
clyos get table yx.token 4,BTC tstats
```

### results of tstats
```
{
  "rows": [{
      "supply": {
        "amount": "800000.0000 BTC",
        "issuer": "d2"
      },
      "options": 0,
      "kyc_rules": [],
      "kyc_rule_flags": []
    }
  ],
  "more": false
}
```

## taccounts
Get all the token balances of the user
```
clyos get table yx.token user2 taccounts
```

### results of taccounts
1. id : just for internal managing purpose 
1. token : yx_asset type which are token amount and issuer
1. amount : balance of the account
```
{
  "rows": [{
      "id": 0,
      "token": {
        "amount": "10000.0000 BTC",
        "issuer": "d2"
      },
      "options": 0
    }
  ],
  "more": false
}
```

### get the specific token balance of the user
* The value of -L is `yx_symbol_serialized`; 128-bit integers which is combined with the symbol and the issuer.
   * | the symbol(64-bit integer) | issuer (64-bit integer) |
   * Web Assembly VM follows little endian.
```
clyos get table yx.token user2 taccounts --index 2 --key-type i128 -L 0x00000000000080480654455354000000 -l 1
```