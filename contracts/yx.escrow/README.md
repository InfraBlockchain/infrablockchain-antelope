# Abtract
* This Yosemite contract is for token escrow service.

# What You Can Do With This Contract
* Any Yosemite tokens issued via yx.ntoken and yx.token can be escrowed by the escrow service provider aka third party.
* The entity who owns the third party account can provide escrow service as DApp to its users.
   * The users request their token transfer to the DApp.
   * The third party account can decide to proceed or cancel the user's token transfer.

# Management Actions

## setting fee for operations
* Transaction fee for operations is set by [yx.txfee](../../contracts/yx.txfee/)::settxfee operation.
* It shares with transaction fee of yx.token.
```
clyos push action yx.txfee settxfee '{"operation":"tf.esescrow", "fee":"100.00 DKRW"}}' -p yosemite
clyos push action yx.txfee settxfee '{"operation":"tf.esproceed", "fee":"10.00 DKRW"}}' -p yosemite
clyos push action yx.txfee settxfee '{"operation":"tf.escancel", "fee":"10.00 DKRW"}}' -p yosemite
```

# Actions

## escrow
Request escrow to the third party to transfer the YOSEMITE token

* The amount of token is transferred to the `thirdparty` account. Only when the third party calls 'proceed' action with the specified id, it is transferred to the 'to' account.
* The native token with the specific system depository can be in escrow with this action.
```
clyos push action yx.escrow escrow '{"thirdparty":"servprovider","id":1,"from":"user2","to":"user1","token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"my memo"}' -p servprovider user2
```
### parameters of escrow
1. thirdparty : account name of the third party providing escrow service
1. id : identifier of escrow request
1. from : account name to transfer from
1. to : account name to transfer to
1. token : the amount of token with the issuer
   * amount
   * issuer
1. memo : string less than or equal to 256 bytes to be used for inlined transfer action

### inline actions and notifications of escrow
```
#     yx.escrow <= yx.escrow::escrow            {"thirdparty":"servprovider","id":1,"from":"user2","to":"user1","token":{"amount":"10000.0000 BTC","...
#      yx.token <= yx.token::transfer           {"from":"user2","to":"servprovider","token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"my memo"}
#         user2 <= yx.token::transfer           {"from":"user2","to":"servprovider","token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"my memo"}
#  servprovider <= yx.token::transfer           {"from":"user2","to":"servprovider","token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"my memo"}
```

## escrownt
Request escrow to the third party to transfer the native token regardless of the system depository

* The amount of native token is transferred to the `thirdparty` account. Only when the third party calls 'proceed' action with the specified id, it is transferred to the 'to' account.
```
clyos push action yx.escrow escrownt '{"thirdparty":"servprovider","id":0,"from":"user1","to":"user2","amount":"10000.00 DKRW","memo":"my memo"}' -p servprovider user1
```
### parameters of escrownt
1. thirdparty : account name of the third party providing escrow service
1. id : identifier of escrow request
1. from : account name to transfer from
1. to : account name to transfer to
1. amount : token amount and symbol
1. memo : string less than or equal to 256 bytes to be used for inlined transfer action

### inline actions and notifications of escrownt
```
#     yx.escrow <= yx.escrow::escrownt          {"thirdparty":"servprovider","id":0,"from":"user1","to":"user2","amount":"10000.00 DKRW","memo":"my mem...
#     yx.ntoken <= yx.ntoken::transfer          {"from":"user1","to":"servprovider","amount":"10000.00 DKRW","memo":"memo"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"user1","to":"servprovider","token":{"amount":"10000.00 DKRW","issuer":"d1"},"memo":"my memo"}
#         user1 <= yx.ntoken::ntransfer         {"from":"user1","to":"servprovider","token":{"amount":"10000.00 DKRW","issuer":"d1"},"memo":"my memo"}
#  servprovider <= yx.ntoken::ntransfer         {"from":"user1","to":"servprovider","token":{"amount":"10000.00 DKRW","issuer":"d1"},"memo":"my memo"}
```

## proceed
Confirm transfer of token to 'to' account by the third party

* The escrow request pointed by 'id' is removed.
```
clyos push action yx.escrow proceed '{"thirdparty":"servprovider","id":0}' -p servprovider
```

### inline actions and notifications of proceed
```
#     yx.escrow <= yx.escrow::proceed           {"thirdparty":"servprovider","id":0}
#     yx.ntoken <= yx.ntoken::transfer          {"from":"servprovider","to":"user2","amount":"10000.00 DKRW","memo":"my memo"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"servprovider","to":"user2","token":{"amount":"10000.00 DKRW","issuer":"d1"},"memo":"my memo"}
#  servprovider <= yx.ntoken::ntransfer         {"from":"servprovider","to":"user2","token":{"amount":"10000.00 DKRW","issuer":"d1"},"memo":"my memo"}
#         user2 <= yx.ntoken::ntransfer         {"from":"servprovider","to":"user2","token":{"amount":"10000.00 DKRW","issuer":"d1"},"memo":"my memo"}
```

## cancel
Cancel transfer of token by the third party

* Token is returned back to 'from' account.
* The escrow request pointed by 'id' is removed.
```
clyos push action yx.escrow cancel '{"thirdparty":"servprovider","id":1}' -p servprovider
```
### inline actions and notifications of cancel
```
#     yx.escrow <= yx.escrow::cancel            {"thirdparty":"servprovider","id":1}
#      yx.token <= yx.token::transfer           {"from":"servprovider","to":"user2","token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"my memo"}
#  servprovider <= yx.token::transfer           {"from":"servprovider","to":"user2","token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"my memo"}
#         user2 <= yx.token::transfer           {"from":"servprovider","to":"user2","token":{"amount":"10000.0000 BTC","issuer":"d2"},"memo":"my memo"}
```

# Tables

## escrows
Get the list of escrow requests of the third party

```
clyos get table yx.escrow servprovider escrows
```
### result of escrows
```
{
  "rows": [{
      "id": 1,
      "token": {
        "amount": "10000.0000 BTC",
        "issuer": "d2"
      },
      "from": "user2",
      "to": "user1",
      "memo": "my memo"
    }
  ],
  "more": false
}
```
* Note that native token for any system depositories has empty issuer.
```
"token": {
  "amount": "10000.00 DKRW",
  "issuer": ""
}
```
