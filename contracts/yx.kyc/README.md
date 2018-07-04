# Roles of yx.kyc contract
* To provide 'kyc'ed information for dapps
* To provide 'kyc'ed information for other system contracts or future smart contracts

# To test yx.kyc contract
* Assume that yx.token contract is prepared.
* Assume that accounts d1, d2 and user1 are created.
* *Assume that d1 is the depository of the native token.*

## deploy yx.kyc contract
`cleos set contract yx.kyc build/contracts/yx.kyc -p yx.kyc`

## set user's KYC information
* [Temporary] fee is not determined.
```
cleos push action yx.kyc setinfo '{"account":"user1", "depository":"d1", "level":"1", "addendum":"abcde"}' -p d1
```

## update user's KYC information
* [Temporary] fee is not determined.
### level
```
cleos push action yx.kyc uplevel '{"account":"user1", "level":"2"}' -p d1
```
### addendum
```
cleos push action yx.kyc upaddendum '{"account":"user1", "addendum":"123"}' -p d1
cleos push action yx.kyc upaddendum '{"account":"user1", "addendum":""}' -p d1
```

## get kyc table
```
cleos get table yx.kyc yx.kyc kyc
```

## delete user's KYC information
```
cleos push action yx.kyc delinfo '{"account":"user1"}' -p d1
```
