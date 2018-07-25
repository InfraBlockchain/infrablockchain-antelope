# Roles of the contract
* To provide the infrastructure for the digital contract & signing services on the legal business

# Before Testing The Contract
* Assume that yx.dcontract contract is prepared.
* Assume that accounts legal1, user1, user2 and user3 are created.

## deploy the contract
`cleos set contract yx.dcontract build/contracts/yx.dcontract -p yx.dcontract`

## setting fee for operations
* Fee for operations is set by `setfee` operation.
* The majority of the current depositories must agree using multisig feature.
* [Temporary] For test convenience, this operation is not yet multisig.
```
cleos push action yx.dcontract setfee '[ "create", "500.0000 DKRW" ]' -p yx.dcontract
cleos push action yx.dcontract setfee '[ "addsigners", "100.0000 DKRW" ]' -p yx.dcontract
cleos push action yx.dcontract setfee '[ "sign", "100.0000 DKRW" ]' -p yx.dcontract
cleos push action yx.dcontract setfee '[ "upadddochash", "5.0000 DKRW" ]' -p yx.dcontract
cleos push action yx.dcontract setfee '[ "remove", "0.0000 DKRW" ]' -p yx.dcontract
```


# Operations

## create a digital contract
```
cleos push action yx.dcontract create '{"creator":"legal1", "dochash":"123456", "adddochash":"", "signers":["user1", "user2"], "expiration":"2018-08-31T02:49:57", "options":0}' -p legal1
```
* expiration : ISO8601 format, combined date and time to seconds e.g. 2018-08-31T02:49:57
* After the transaction is done, dapp can get the contract id, which is the sequenced 64bit integer. The id is used with the creator account for other operations.

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