# Abtract
* This Yosemite contract is for the peer-to-peer market of non-fungible tokens.
* The Yosemite contract for the non-fungible token(NFT) is [`yx.nft`](../../contracts/yx.nft/README.md).

# What You Can Do With This Contract
* An entity who owns the NFT can sell it with the specific token. The seller can cancel the sell order.
* An entity who owns the token can buy it.

# Management Actions

## setting fee for operations
* Transaction fee for operations is set by [yx.txfee](../../contracts/yx.txfee/)::settxfee operation.
```
clyos push action yx.txfee settxfee '{"operation":"tf.nftsell", "fee":"500.00 DKRW"}}' -p yosemite
clyos push action yx.txfee settxfee '{"operation":"tf.nftbuy", "fee":"500.00 DKRW"}}' -p yosemite
clyos push action yx.txfee settxfee '{"operation":"tf.nftsellc", "fee":"10.00 DKRW"}}' -p yosemite
```


# Actions

## sell
Sell NFT with the specified token

* NFT is transferred from the seller to `yx.nftex` account.
* Price token must be created if it is not the native token. See [`yx.token`](../../contracts/yx.token/README.md).
```
clyos push action yx.nftex sell '{"seller":"user1","nft":{"ysymbol":{"tsymbol":"0,GIT","issuer":"gameprovider"},"id":"0"},"price":{"amount":"1000.00 GMT","issuer":"gameprovider"},"expiration":"2018-10-24T02:49:57","memo":"my memo"}' -p user1
```

### parameters of sell
1. seller : seller who owns the NFT
1. nft
   * ysymbol : NFT symbol and its issuer; precision is always 0
   * id : NFT id managed by [`yx.nft`](../../contracts/yx.nft/README.md)
1. price : the amount of token with the issuer
   * native token managed by yx.ntoken
   * non-native token managed by yx.token
1. expiration : expiration time in the ISO8601 format, combined date and time to seconds in UTC e.g. 2018-10-24T02:49:57
   * YosemiteChain assumes that the time-zone of the expiration time is UTC. Other time-zones are not considered.
   * The minimum expiration is 1 minute from now.
1. memo : string less than or equal to 256 bytes

### inline actions and notifications of sell
```
#      yx.nftex <= yx.nftex::sell               {"seller":"user1","nft":{"ysymbol":{"tsymbol":"0,GIT","issuer":"gameprovider"},"id":0},"price":{"amo...
#        yx.nft <= yx.nft::transferid           {"from":"user1","to":"yx.nftex","issuer":"gameprovider","ids":[0],"memo":"my memo"}
#         user1 <= yx.nft::transferid           {"from":"user1","to":"yx.nftex","issuer":"gameprovider","ids":[0],"memo":"my memo"}
#      yx.nftex <= yx.nft::transferid           {"from":"user1","to":"yx.nftex","issuer":"gameprovider","ids":[0],"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"500.00 DKRW","issuer":"d1"}}
#         user1 <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"500.00 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"500.00 DKRW","issuer":"d1"}}
```

## cancelsell
Cancel the sell order of NFT

* NFT is transferred from `yx.nftex` account to the seller.
```
clyos push action yx.nftex cancelsell '{"nft":{"ysymbol":{"tsymbol":"0,GIT","issuer":"gameprovider"},"id":"0"}}' -p user1
```

### parameters of cancelsell
1. nft
   * ysymbol : NFT symbol and its issuer; precision is always 0
   * id : NFT id managed by [`yx.nft`](../../contracts/yx.nft/README.md)

### inline actions and notifications of cancelsell
```
#      yx.nftex <= yx.nftex::cancelsell         {"nft":{"ysymbol":{"tsymbol":"0,GIT","issuer":"gameprovider"},"id":0}}
#        yx.nft <= yx.nft::transferid           {"from":"yx.nftex","to":"user1","issuer":"gameprovider","ids":[0],"memo":""}
#      yx.nftex <= yx.nft::transferid           {"from":"yx.nftex","to":"user1","issuer":"gameprovider","ids":[0],"memo":""}
#         user1 <= yx.nft::transferid           {"from":"yx.nftex","to":"user1","issuer":"gameprovider","ids":[0],"memo":""}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"10.00 DKRW","issuer":"d1"}}
#         user1 <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"10.00 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"user1","token":{"amount":"10.00 DKRW","issuer":"d1"}}
```

## buy
Buy NFT on the sell order book

```
clyos push action yx.nftex buy '{"buyer":"user2","nft":{"ysymbol":{"tsymbol":"0,GIT","issuer":"gameprovider"},"id":"0"},"price":{"amount":"1000.00 GMT","issuer":"gameprovider"},"memo":""}' -p user2
```

### parameters of buy
1. buyer
1. nft
   * ysymbol : NFT symbol and its issuer; precision is always 0
   * id : NFT id managed by [`yx.nft`](../../contracts/yx.nft/README.md)
1. price : the amount of token with the issuer
   * native token managed by yx.ntoken
   * non-native token managed by yx.token
1. memo : string less than or equal to 256 bytes

### inline actions and notifications of buy
```
#      yx.nftex <= yx.nftex::buy                {"buyer":"user2","nft":{"ysymbol":{"tsymbol":"0,GIT","issuer":"gameprovider"},"id":0},"price":{"amou...
#        yx.nft <= yx.nft::transferid           {"from":"yx.nftex","to":"user2","issuer":"gameprovider","ids":[0],"memo":""}
#      yx.nftex <= yx.nft::transferid           {"from":"yx.nftex","to":"user2","issuer":"gameprovider","ids":[0],"memo":""}
#         user2 <= yx.nft::transferid           {"from":"yx.nftex","to":"user2","issuer":"gameprovider","ids":[0],"memo":""}
#      yx.token <= yx.token::transfer           {"from":"user2","to":"user1","token":{"amount":"1000.00 GMT","issuer":"gameprovider"},"memo":""}
#         user2 <= yx.token::transfer           {"from":"user2","to":"user1","token":{"amount":"1000.00 GMT","issuer":"gameprovider"},"memo":""}
#         user1 <= yx.token::transfer           {"from":"user2","to":"user1","token":{"amount":"1000.00 GMT","issuer":"gameprovider"},"memo":""}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"user2","token":{"amount":"500.00 DKRW","issuer":"d1"}}
#         user2 <= yx.ntoken::payfee            {"payer":"user2","token":{"amount":"500.00 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"user2","token":{"amount":"500.00 DKRW","issuer":"d1"}}
```

# Tables

## sellbook
Get all sell orders for each NFT symbol

The value 518171862 is [FNV-1a hash](https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash) result of `0,GIT@gameprovider`.
```
clyos get table yx.nftex 518171862 sellbook
```

### results of nftokens
```
{
  "rows": [{
      "id": 0,
      "ysymbol": {
        "tsymbol": "0,GIT",
        "issuer": "gameprovider"
      },
      "seller": "user1",
      "price": {
        "amount": "1000.00 GMT",
        "issuer": "gameprovider"
      },
      "expiration": "2018-10-24T02:49:57"
    }
  ],
  "more": false
}
```
