# Abtract
* This Yosemite contract is for non-fungible tokens.
* The Yosemite contract for the native token is [`yx.ntoken`](../../contracts/yx.ntoken/README.md), which the specification of the Yosemite token is explained.
* The Yosemite contract for the user token is [`yx.token`](../../contracts/yx.token/README.md).

# Non-Fungible Token (NFT) 
* As specified by [ERC721](https://github.com/ethereum/EIPs/issues/721), NFT can represent ownership over digital or physical assets.
* NFTs are distinguishable and you must track the ownership of each one separately.

# What You Can Do With This Contract
* An entity who owns the Yosemite account can create a NFT with `create` operation. YosemiteChain calls the entity the NFT issuer or the NFT depository.
   * When creating the NFT, the issuer can enable the setting of some options like account freezing or token freezing.
* The NFT depository issues and redeems the NFT.
* The NFT depository can freeze the NFT transfer.
* Any Yosemite accounts who own the NFT can transfer it each other.

## Format of NFT
* 0,MYNFT@nftissuer
   * 0 : precision
   * MYNFT : symbol
   * nftissuer : account name of the issuer
* The precision of NFT is always 0.

### Format of Token Amount
* 1 MYNFT@nftissuer
* The amount of NFT is always 1.


# Management Actions

## setting fee for operations
* It shares with transaction fee of yx.token.


# Actions

## create
Create NFT with its symbol

* The NFT creator naturally becomes the NFT depository.
* Enabling the setting of options cannot be done any more after creation, because the owners of the NFT should know about what options can be set and it must not be changed after they own.
```
clyos push action yx.nft create '{"ysymbol":{"tsymbol":"0,MYGAME","issuer":"gameprovider"},"can_set_options":0}' -p gameprovider
```

### parameters of create
1. ysymbol : NFT symbol and its issuer; precision is always 0
1. can_set_options : 16-bit flags of options
   * See [`yx.token` create parameter](../../contracts/yx.token#parameters-of-create)
   
## issue
Issue one or multiple NFTs to an account by the NFT depository

```
clyos push action yx.nft issue '{"to":"user1","token":{"amount":"1 MYGAME","issuer":"gameprovider"},"ids":[0],"uris":["item1"],"name":"basicsword","memo":"my memo"}' -p gameprovider
clyos push action yx.nft issue '{"to":"user1","token":{"amount":"2 MYGAME","issuer":"gameprovider"},"ids":[1,2],"uris":["item2-1", "item2-2"],"name":"potion1","memo":"my memo"}' -p gameprovider
```

### parameters of issue
1. to : the account who is transferred the NFT
1. token : the amount of NFT with the issuer(=NFT depository)
   * amount : must be positive integer and must match with the length of uris
   * issuer
1. ids : the list of identifiers assigned by the issuer for each NFT; they are used as primary key of `nftokens` table
1. uris : the list of item information URIs for each NFT; the number of uris must match with the amount of NFT
   * Each URI is a string less than or equal to 256 bytes.
   * We recommend to use tiny URL or the rear part of URL that would be composed to the full URL by DApp.
1. name : the name of NFT; less than or equal to 32 bytes
1. memo : string less than or equal to 256 bytes

### inline actions and notifications of issue
* Case 1. If the `to` account is different from the issuer, the issue action does the `transferid` inline action.
   * Case 1-2. single issue
```
#        yx.nft <= yx.nft::issue                {"to":"user1","token":{"amount":"1 MYGAME","issuer":"gameprovider"},"ids":[0],"uris":["item1"],"name":"basicsw...
#        yx.nft <= yx.nft::transferid           {"from":"gameprovider","to":"user1","issuer":"gameprovider","ids":[0],"memo":"my memo"}
#  gameprovider <= yx.nft::transferid           {"from":"gameprovider","to":"user1","issuer":"gameprovider","ids":[0],"memo":"my memo"}
#         user1 <= yx.nft::transferid           {"from":"gameprovider","to":"user1","issuer":"gameprovider","ids":[0],"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"gameprovider","token":{"amount":"100.00 DKRW","issuer":"d1"}}
#  gameprovider <= yx.ntoken::payfee            {"payer":"gameprovider","token":{"amount":"100.00 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"gameprovider","token":{"amount":"100.00 DKRW","issuer":"d1"}}
```
   * Case 1-2. multiple issue
```
#        yx.nft <= yx.nft::issue                {"to":"user1","token":{"amount":"2 MYGAME","issuer":"gameprovider"},"ids":[1,2],"uris":["item2-1","item2-2"],"na...
#        yx.nft <= yx.nft::transferid           {"from":"gameprovider","to":"user1","issuer":"gameprovider","ids":[1,2],"memo":"my memo"}
#  gameprovider <= yx.nft::transferid           {"from":"gameprovider","to":"user1","issuer":"gameprovider","ids":[1,2],"memo":"my memo"}
#         user1 <= yx.nft::transferid           {"from":"gameprovider","to":"user1","issuer":"gameprovider","ids":[1,2],"memo":"my memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"gameprovider","token":{"amount":"100.00 DKRW","issuer":"d1"}}
#  gameprovider <= yx.ntoken::payfee            {"payer":"gameprovider","token":{"amount":"100.00 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"gameprovider","token":{"amount":"100.00 DKRW","issuer":"d1"}}
```

* Case 2. If the to account is the NFT depository itself, there is no `transferid` inline action.
```
#        yx.nft <= yx.nft::issue                {"to":"gameprovider","token":{"amount":"1 MYGAME","issuer":"gameprovider"},"ids":[3],"uris":["item1"],"name":"...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"gameprovider","token":{"amount":"100.00 DKRW","issuer":"d1"}}
#  gameprovider <= yx.ntoken::payfee            {"payer":"gameprovider","token":{"amount":"100.00 DKRW","issuer":"d1"}}
#      yx.txfee <= yx.ntoken::payfee            {"payer":"gameprovider","token":{"amount":"100.00 DKRW","issuer":"d1"}}
```

## redeem
Redeem(burn) NFT from an account by the NFT depository

* At first, the account transfers the NFT to the depository.
```
clyos push action yx.nft transferid '{"from":"user1","to":"gameprovider","issuer":"gameprovider","ids":[0,1],"memo":"transfer for redeem"}' -p user1
```
* Then the NFT depository checks the transfer action is irreversible and calls redeem action.
```
clyos push action yx.nft redeem '{"issuer":"gameprovider","ids":[0,1],"memo":"redeem for user1"}' -p gameprovider
```

### parameters of redeem
1. issuer : account name of the NFT issuer
1. ids : the list of identifiers assigned by the issuer for each NFT
1. memo : string less than or equal to 256 bytes

## transferid
Transfer NFTs

```
clyos push action yx.nft transferid '{"from":"user2","to":"user3","issuer":"gameprovider"},"ids":[0,1],"memo":"my memo"}' -p user2
```

### parameters of transferid
1. from : account name to transfer from
1. to : account name to transfer to
1. issuer : account name of the NFT issuer
1. ids : the list of identifiers assigned by the issuer for each NFT
1. memo : string less than or equal to 256 bytes


## transfer
Transfer a NFT

* This action would be used when the sender owns only one NFT of the symbol and the issuer.
* If the sender has more than one NFT specified by the token, any NFT with the certain uri is transferred.
```
clyos push action yx.nft transfer '{"from":"user2","to":"user3","token":{"amount":"1 MYGAME","issuer":"gameprovider"},"memo":"my memo"}' -p user2
```

### parameters of transfer
1. from : account name to transfer from
1. to : account name to transfer to
1. token : the amount of token with the issuer
   * amount : must be 1
   * issuer
1. memo : string less than or equal to 256 bytes

## setkycrule
See [`yx.token` setkycrule](../../contracts/yx.token#setkycrule)

## setoptions
See [`yx.token` setoptions](../../contracts/yx.token#setoptions)

## freezeacc
See [`yx.token` freezeacc](../../contracts/yx.token#freezeacc)


# Tables

## tstats
See [`yx.token` tstats](../../contracts/yx.token#tstats)

## taccounts
See [`yx.token` taccounts](../../contracts/yx.token#taccounts)

## nftokens
Get all the NFTs issued by the NFT depository
```
clyos get table yx.nft gameprovider nftokens
```

### results of nftokens
```
{
  "rows": [{
      "id": 0,
      "uri": "item1",
      "owner": "user1",
      "value": {
        "amount": "1 MYGAME",
        "issuer": "gameprovider"
      },
      "name": "basicsword"
    }
  ],
  "more": false
}
```

### get the NFTS by the owner
```
clyos get table yx.nft gameprovider nftokens --index 2 --key-type i64 -L user1
```
