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
* Transaction fee for operations is set by [yx.txfee](../../contracts/yx.txfee/)::settxfee operation.
* It shares with transaction fee of yx.token.
