# Roles of the contract
* To provide the infrastructure for the digital contract & signing services on the legal business

# Before Testing The Contract
* Assume that yx.dcontract contract is prepared.
* Assume that accounts legal1, user1, user2 and user3 are created.

## deploy the contract
`cleos set contract yx.dcontract build/contracts/yx.dcontract -p yx.dcontract`

# Operations

## create a digital contract
```
cleos push action yx.dcontract create '{"creator":"legal1", "dochash":"123456", "adddochash":"", "signers":["user1", "user2"], "expiration":"2018-08-31T02:49:57", "options":0}' -p legal1
```
* expiration : ISO8601 format, combined date and time to seconds e.g. 2018-08-31T02:49:57
* After the transaction is done, dapp can get the contract id, which is the sequenced 64bit integer. The id is used with the creator account for other operations.

## add additional signers
```
cleos push action yx.dcontract addsigners '{"creator":"legal1", "id":"1", "signers":["user3"]}' -p legal1
```

## sign the digital contract
```
cleos push action yx.dcontract sign '{"creator":"legal1", "id":"1", "signer":"user1", "signerinfo":""}' -p user1
cleos push action yx.dcontract sign '{"creator":"legal1", "id":"1", "signer":"user2", "signerinfo":"bluewolf"}' -p user2
cleos push action yx.dcontract sign '{"creator":"legal1", "id":"1", "signer":"user3", "signerinfo":"hello"}' -p user3
```

## change the additional document hash address
```
cleos push action yx.dcontract upadddochash '{"creator":"legal1", "id":"1", "adddochash":"oiuoiu"}' -p legal1
```

## remove the digital contract
```
cleos push action yx.dcontract remove '{"creator":"legal1", "id":"1"}' -p legal1
```