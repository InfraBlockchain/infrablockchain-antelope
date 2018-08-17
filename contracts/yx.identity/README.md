YOSEMITE Identity Contract
===

The Identity Authorities have the right for managing user info and KYC info on blockchain.
The active block producers can authorize the self-registered Identity Authroities to be promoted to manage the blockchain account's identity information on blockchain.
For each blockchain account, Identity Authority can manage below information

* account type (e.g. normal, system, ...)
* KYC info. (e.g. email, phone, real name, bank account, ...)
* account state (e.g. blacklisted account, ...)
* Identity-Authority specific data (arbitrary string data)

Once an Identity Authority sets identity info. for an account, only the Identity Authority account can modify the identity info for the account.


Environment Var.
---

```bash
YOSEMITE_CLYOS=$YOSEMITE_HOME/build/programs/clyos/clyos
YOSEMITE_CONTRACTS_DIR=$YOSEMITE_HOME/build/contracts
```

Install Identity System Contract
---
```bash
$YOSEMITE_CLYOS create account yosemite yx.identity EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLYOS set contract yx.identity $YOSEMITE_CONTRACTS_DIR/yx.identity/ -p yx.identity@active
```

YOSEMITE Identity Codes
---

```c
#define YOSEMITE_ID_ACC_TYPE_NORMAL        0b0000000000000000
#define YOSEMITE_ID_ACC_TYPE_SYSTEM        0b1000000000000000

#define YOSEMITE_ID_KYC_NO_AUTH            0b0000000000000000
#define YOSEMITE_ID_KYC_EMAIL_AUTH         0b0000000000000001
#define YOSEMITE_ID_KYC_PHONE_AUTH         0b0000000000000010
#define YOSEMITE_ID_KYC_REAL_NAME_AUTH     0b0000000000000100
#define YOSEMITE_ID_KYC_BANK_ACCOUNT_AUTH  0b0000000000001000
#define YOSEMITE_ID_KYC_MAX_AUTH           0b0000000000001111

#define YOSEMITE_ID_ACC_STATE_CLEAR                       0x00000000
#define YOSEMITE_ID_ACC_STATE_BLACKLISTED                 0x00000001
#define YOSEMITE_ID_ACC_STATE_BLACKLISTED_NTOKEN_SEND     0x00000002
#define YOSEMITE_ID_ACC_STATE_BLACKLISTED_NTOKEN_RECEIVE  0x00000004
#define YOSEMITE_ID_ACC_STATE_MAX                         0x00000007
```
@see https://github.com/YosemiteLabs/yosemite-public-blockchain/blob/yosemite-master/contracts/yosemitelib/identity.hpp

Managing Account Identity Info (including KYC)
---

#### setting identity Information for an account by an authorized Identity Authority

```bash
$YOSEMITE_CLYOS push action yx.identity setidinfo "{\"account\":\"useraccount1\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 0111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth1@active
$YOSEMITE_CLYOS push action yx.identity setidinfo "{\"account\":\"useraccount2\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"23uyiuye\"}" -p idauth1@active
$YOSEMITE_CLYOS push action yx.identity setidinfo "{\"account\":\"useraccount3\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"vewv23r3\"}" -p idauth1@active
```

#### Querying identity information for an account

```bash
$YOSEMITE_CLYOS get table -L useraccount1 -l 1 yx.identity yx.identity identity
{
  "rows": [{
      "account": "useraccount1",
      "identity_authority": "idauth1",
      "type": 255,
      "kyc": 15,
      "state": 1,
      "data": "23fiuygy3"
    }
  ],
  "more": true
}
```


#### update user account type

```bash
$YOSEMITE_CLYOS push action yx.identity settype "{\"account\":\"useraccount1\", \"type\":$(echo 'ibase=2; 11111111' | bc)}" -p idauth1@active
```

#### update kyc status

```bash
$YOSEMITE_CLYOS push action yx.identity setkyc "{\"account\":\"useraccount1\", \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p idauth1@active
```

#### update account state (e.g. blacklist account)

```bash
$YOSEMITE_CLYOS push action yx.identity setstate "{\"account\":\"useraccount1\", \"state\":$(echo 'ibase=2; 0001' | bc)}" -p idauth1@active
```

#### update identity-authority specific data

```bash
$YOSEMITE_CLYOS push action yx.identity setdata "{\"account\":\"useraccount1\", \"data\":\"23fiuygy3\"}" -p idauth1@active
```

Checking Identity Information in Smart Contract Code
---

```cpp

namespace yosemite {
    struct identity_info {
        account_name account;
        account_name identity_authority; // identity authority account managing the identity info. of this 'account'
        uint16_t type; // account type (e.g. normal, system, ...)
        uint16_t kyc; // KYC status (e.g. email, phone, real name, bank account, ...)
        uint32_t state; // account state (e.g. flag for blacklisted account, Identity-Authority specific flags, ...)
        std::string data; // Identity-Authority specific data
    };

    const identity_info get_identity_info(const account_name account);
    
    uint16_t get_identity_account_type(const account_name account);
    
    bool is_account_type(const account_name account, uint16_t type_code);

    uint16_t get_identity_kyc_status(const account_name account);

    bool has_kyc_status(const account_name account, uint16_t kyc_flags);

    bool has_all_kyc_status(const account_name account, uint16_t kyc_flags);

    uint32_t get_identity_account_state(const account_name account);

    bool has_account_state(const account_name account, uint32_t state_flag);

    bool has_all_account_state(const account_name account, uint32_t state_flags);

    std::string get_identity_data(const account_name account);
}    
```

