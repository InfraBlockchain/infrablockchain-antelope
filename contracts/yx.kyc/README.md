# Roles of the contract
* To provide account's KYC(Know Your Customer) information to DApps
* To provide account's KYC information to other system contracts or future smart contracts

# KYC Vector(kycvector)
* When a depository wants to set or update KYC information of an account to the blockchain, it must set the account's authentication methods.
* Yosemite Public Blockchain defines the authentication methods as the array of bits, called KYC vector(kycvector).
   * KYC_VECTOR_EMAIL_AUTH = 0x00000001 : e-mail authentication
   * KYC_VECTOR_PHONE_AUTH = 0x00000002 : phone authentication by call, SMS message or other ways
   * KYC_VECTOR_REAL_NAME_AUTH = 0x00000004 : real name authentication by social security number, passport or other ways
   * KYC_VECTOR_BANK_ACCOUNT_AUTH = 0x00000008 : bank account authentication
* The depository can make KYC vector with bit OR(|) operation. The type of KYC vector is unsigned 32-bit integer.
```c
uint32_t kycvector = KYC_VECTOR_REAL_NAME_AUTH | KYC_VECTOR_PHONE_AUTH;
```

# Before Testing The Contract
* Assume that yx.token contract is prepared.
* Assume that accounts d1, d2 and user1 are created.
* *Assume that d1 is the depository of the native token.*

## deploy the contract
`cleos set contract yx.kyc build/contracts/yx.kyc -p yx.kyc`

# Operations

## set user's KYC information
```
cleos push action yx.kyc setinfo '{"account":"user1", "depository":"d1", "kycvector":1, "addendum":"abcde"}' -p d1
```

## update user's KYC information
### kycvector
```
cleos push action yx.kyc upkycvector '{"account":"user1", "kycvector":"2"}' -p d1
```
### addendum
```
cleos push action yx.kyc upaddendum '{"account":"user1", "addendum":"123"}' -p d1
cleos push action yx.kyc upaddendum '{"account":"user1", "addendum":""}' -p d1
```

## delete user's KYC information
```
cleos push action yx.kyc delinfo '{"account":"user1"}' -p d1
```

## get kyc table
```
cleos get table yx.kyc yx.kyc kyc
```