INFRABLOCKCHAIN Transaction Fee Contract
===

* System contract managing the transaction fee amount for each charged system transaction.
* Transaction fee amounts should be agreed by the active block producers by signing txfee_contract::settxfee transaction
* This contract is deployed on the system transaction fee account (INFRABLOCKCHAIN_SYS_TX_FEE_ACCOUNT) on which transaction fees generated on the blockchain will be saved

Environment Var.
---

```bash
INFRA_CLI=$INFRABLOCKCHAIN_HOME/build/programs/infra-cli/infra-cli
INFRABLOCKCHAIN_CONTRACTS_DIR=$INFRABLOCKCHAIN_HOME/build/contracts
```

Install Transaction Fee System Contract
---
```bash
$INFRA_CLI set contract sys.txfee $INFRABLOCKCHAIN_CONTRACTS_DIR/sys.txfee/ -p sys.txfee@active
```

INFRABLOCKCHAIN Transaction Fee Operation Names
---
[`infrablockchainlib/transaction_fee.hpp`](../../contracts/infrablockchainlib/transaction_fee.hpp)

Setup Transaction Fees
---

Transaction fees can be updated by the active block producers

```bash
# infrasys
$INFRA_CLI push action sys.txfee settxfee '[ "tf.newacc", "1000.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.regprod", "3000000.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.regsysdep", "2000000.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.regidauth", "2000000.00 DKRW" ]' -p infrasys@active

// yx.dcontract
$INFRA_CLI push action sys.txfee settxfee '[ "tf.dccreate", "500.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.dcaddsign", "100.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.dcsign", "300.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.dcupadd", "50.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.dcremove", "0.00 DKRW" ]' -p infrasys@active

$INFRA_CLI get table -l 100 sys.txfee sys.txfee txfees
{
  "rows": [{
      "operation": "tf.dcaddsign",
      "fee": 10000
    },{
...
    },{
      "operation": "tf.newacc",
      "fee": 100000
    },{
      "operation": "tf.nissue",
      "fee": 0
    },{
      "operation": "tf.nredeem",
      "fee": 100000
    },{
      "operation": "tf.ntransfer",
      "fee": 20000
    },{
...
}

```

Querying the accumulated native token amount in the Transaction Fee Profit Pool
---

```bash
$INFRA_CLI get table yx.ntoken sys.txfee ntaccounts
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
namespace infrablockchain {

    typedef uint64_t operation_name;

    eosio::asset get_transaction_fee(const operation_name operation_name);

    int64_t get_transaction_fee_amount(const operation_name operation_name);
}
``` 