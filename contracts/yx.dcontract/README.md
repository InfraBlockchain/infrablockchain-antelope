# Abtract
* This system contract provides the infrastructure for creation of digital contracts and digital signing service for them.
* The name of the system contract is `yx.dcontract`.

# Digital Signing
* The features related to digital signing are governed by public key cryptography which has long been developed by computer cryptography.
   * Most blockchains including Yosemite Public Blockchain(YosemiteChain or Yosemite) have their account system based on public key cryptography. So digital signing is natural to YosemiteChain.
* In most cases, an account of YosemiteChain has its own private key and the public key, which are the key pair that can never be changed after created.
* Digital signature is made by the private key who owns the key(=knows the password) and its integrity can be checked by anyone who knows its paired public key.
   * For more information, refer to https://en.wikipedia.org/wiki/Digital_signature and https://en.wikipedia.org/wiki/Public-key_cryptography
* YosemiteChain can check the integrity of digital signature with all accounts because it knows all the public keys of accounts.

## digital signing of the action of digital contract creation
* When the creation action is delivered to this system contract, it checks that the action is sent by the service provider who wants to create the digital contract.
* The creation action includes the list of accounts who are contract signers.

## digital signing of the digital contract
* The action for digital contract signing is signed by one of the contract signers, not the service provider.


# Management Actions

## setting fee for actions
* Transaction fee for operations is set by [yx.txfee](../../contracts/yx.txfee/)::settxfee operation.
```
cleos push action yx.txfee settxfee '{"operation":"tf.dccreate", "fee":"50.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.dcaddsign", "fee":"10.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.dcsign", "fee":"30.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.dcupadd", "fee":"5.0000 DKRW"}}' -p eosio
cleos push action yx.txfee settxfee '{"operation":"tf.dcremove", "fee":"0.0000 DKRW"}}' -p eosio
```


# Actions

## create a digital contract
```
cleos push action yx.dcontract create '{"creator":"legal1", "dochash":"123456", "adddochash":"", "signers":["user1", "user2"], "expiration":"2018-08-31T02:49:57", "options":0}' -p legal1
```
* expiration : ISO8601 format, combined date and time to seconds e.g. 2018-08-31T02:49:57
* After the transaction is done, dapp can get the contract id, which is the sequenced 64bit integer. The id is used with the creator account for other actions.

## add additional signers
```
cleos push action yx.dcontract addsigners '{"dcid":{"creator":"legal1", "sequence":"1"}, "signers":["user3"]}' -p legal1
```

## sign the digital contract
```
cleos push action yx.dcontract sign '{"dcid":{"creator":"legal1", "sequence":"1"}, "signer":"user1", "signerinfo":""}' -p user1
cleos push action yx.dcontract sign '{"dcid":{"creator":"legal1", "sequence":"1"}, "signer":"user2", "signerinfo":"bluewolf"}' -p user2
cleos push action yx.dcontract sign '{"dcid":{"creator":"legal1", "sequence":"1"}, "signer":"user3", "signerinfo":"hello"}' -p user3
```

## change the additional document hash address
```
cleos push action yx.dcontract upadddochash '{"dcid":{"creator":"legal1", "sequence":"1"}, "adddochash":"oiuoiu"}' -p legal1
```

## remove the digital contract
```
cleos push action yx.dcontract remove '{"dcid":{"creator":"legal1", "sequence":"1"}}' -p legal1
```

## get digital contract information
```
cleos get table yx.dcontract legal1 dcontracts
```

## get the signer information
```
cleos get table yx.dcontract user1 signers
```