# Roles of the contract
* To provide account's KYC(Know Your Customer) information to DApps
* To provide account's KYC information to other system contracts or future smart contracts

# Authentication Vector(authvector)
* When a depository wants to set or update KYC information of an account to the blockchain, it must set the account's authentication information.
* Yosemite Public Blockchain defines the authentication information as the array of bits, called authentication vector(authvector);
   * KYC_AUTHVECTOR_EMAIL_AUTH = 0x00000001 : e-mail authentication
   * KYC_AUTHVECTOR_PHONE_AUTH = 0x00000002 : phone authentication by call, SMS message or other ways
   * KYC_AUTHVECTOR_REAL_NAME_AUTH = 0x00000004 : real name authentication by social security number, passport or other ways
   * KYC_AUTHVECTOR_BANK_ACCOUNT_AUTH = 0x00000008 : bank account authentication
* The depository can make authentication vector with bit OR(|) operation. The type of authentication vector is unsigned 32-bit integer.
```c
uint32_t authvector = KYC_AUTHVECTOR_REAL_NAME_AUTH | KYC_AUTHVECTOR_PHONE_AUTH;
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
cleos push action yx.kyc setinfo '{"account":"user1", "depository":"d1", "authvector":1, "addendum":"abcde"}' -p d1
```

## update user's KYC information
### authvector
```
cleos push action yx.kyc upauthvector '{"account":"user1", "authvector":"2"}' -p d1
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