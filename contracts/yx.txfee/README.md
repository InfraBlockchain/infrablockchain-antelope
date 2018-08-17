YOSEMITE Transaction Fee Contract
===

* System contract managing the transaction fee amount for each charged system transaction.
* Transaction fee amounts should be agreed by the active block producers by signing txfee_contract::settxfee transaction
* This contract is deployed on the system transaction fee account (YOSEMITE_TX_FEE_ACCOUNT) on which transaction fees generated on the blockchain will be saved

Environment Var.
---

```bash
YOSEMITE_CLYOS=$YOSEMITE_HOME/build/programs/clyos/clyos
YOSEMITE_CONTRACTS_DIR=$YOSEMITE_HOME/build/contracts
```

Install Transaction Fee System Contract
---
```bash
$YOSEMITE_CLYOS set contract yx.txfee $YOSEMITE_CONTRACTS_DIR/yx.txfee/ -p yx.txfee@active
```

YOSEMITE Transaction Fee Operation Names
---

```c

// yx.system
#define YOSEMITE_TX_FEE_OP_NAME_SYSTEM_NEW_ACCOUNT     N(tf.newacc)
#define YOSEMITE_TX_FEE_OP_NAME_SYSTEM_REG_PRODUCER    N(tf.regprod)
#define YOSEMITE_TX_FEE_OP_NAME_SYSTEM_REG_SYS_DEPO    N(tf.regsysdep)
#define YOSEMITE_TX_FEE_OP_NAME_SYSTEM_REG_ID_AUTH     N(tf.regidauth)

// yx.ntoken
#define YOSEMITE_TX_FEE_OP_NAME_NTOKEN_ISSUE           N(tf.nissue)
#define YOSEMITE_TX_FEE_OP_NAME_NTOKEN_REDEEM          N(tf.nredeem)
#define YOSEMITE_TX_FEE_OP_NAME_NTOKEN_TRANSFER        N(tf.transfer)  // = wptransfer
#define YOSEMITE_TX_FEE_OP_NAME_NTOKEN_NTRANSFER       N(tf.ntransfer) // = wpntransfer

// yx.token
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_CREATE           N(tf.tcreate)
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_ISSUE            N(tf.tissue)
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_REDEEM           N(tf.tredeem)
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_TRANSFER         N(tf.ttransfer)  // = wptransfer
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_SETKYCRULE       N(tf.tsetkyc)
#define YOSEMITE_TX_FEE_OP_NAME_TOKEN_SETOPTIONS       N(tf.tsetopts)

// yx.dcontract
#define YOSEMITE_TX_FEE_OP_NAME_DCONTRACT_CREATE       N(tf.dccreate)
#define YOSEMITE_TX_FEE_OP_NAME_DCONTRACT_ADDSIGNERS   N(tf.dcaddsign)
#define YOSEMITE_TX_FEE_OP_NAME_DCONTRACT_SIGN         N(tf.dcsign)
#define YOSEMITE_TX_FEE_OP_NAME_DCONTRACT_UPADDDOCHASH N(tf.dcupadd)
#define YOSEMITE_TX_FEE_OP_NAME_DCONTRACT_REMOVE       N(tf.dcremove)

```

@see https://github.com/YosemiteLabs/yosemite-public-blockchain/blob/yosemite-master/contracts/yosemitelib/transaction_fee.hpp

Setup Transaction Fees
---

Transaction fees can be updated by the active block producers

```bash
// yx.system
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.newacc", "1000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.regprod", "3000000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.regsysdep", "2000000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.regidauth", "2000000.0000 DKRW" ]' -p yosemite@active

// yx.ntoken
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.nissue", "0.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.nredeem", "1000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.transfer", "100.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.ntransfer", "200.0000 DKRW" ]' -p yosemite@active

// yx.token
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.tcreate", "10000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.tissue", "500.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.tredeem", "500.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.ttransfer", "100.0000 DKRW" ]' -p yosemite@active

// yx.dcontract
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.dccreate", "500.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.dcaddsign", "100.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.dcsign", "300.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.dcupadd", "50.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLYOS push action yx.txfee settxfee '[ "tf.dcremove", "0.0000 DKRW" ]' -p yosemite@active

$YOSEMITE_CLYOS get table -l 100 yx.txfee yx.txfee txfees
{
  "rows": [{
      "operation": "tf.dcaddsign",
      "fee": 1000000
    },{
...
    },{
      "operation": "tf.newacc",
      "fee": 10000000
    },{
      "operation": "tf.nissue",
      "fee": 0
    },{
      "operation": "tf.nredeem",
      "fee": 10000000
    },{
      "operation": "tf.ntransfer",
      "fee": 2000000
    },{
...
}

```

Querying the accumulated native token amount in the Transaction Fee Profit Pool
---

```bash
$YOSEMITE_CLYOS get table yx.ntoken yx.txfee ntaccounts
{
  "rows": [{
      "depository": "sysdepo1",
      "amount": 19273286
    }
  ],
  "more": false
}
```

Getting Current Transaction Fee Amount in Smart Contract Code
---

```cpp
namespace yosemite {

    typedef uint64_t operation_name;

    eosio::asset get_transaction_fee(const operation_name operation_name);

    int64_t get_transaction_fee_amount(const operation_name operation_name);
}
``` 