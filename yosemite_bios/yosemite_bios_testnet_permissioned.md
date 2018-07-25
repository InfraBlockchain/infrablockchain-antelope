YOSEMITE Blockchain (Permissioned version) BIOS setup for Testnet 
===
(revision 20180724)

Testnet Public Access Node URL
---
* [http://testnet.yosemitelabs.org:8888](http://testnet.yosemitelabs.org:8888)

```bash
YOSEMITE_TESTNET_CLEOS="$YOSEMITE_HOME/build/programs/cleos/cleos -u http://testnet.yosemitelabs.org:8888"
$YOSEMITE_TESTNET_CLEOS get info
{
  "server_version": "a5287943",
  "chain_id": "033ff938380397550f9f87115ec363acbe7497313a1992762da6a71cbdc29e71",
  "head_block_num": 4375,
  "last_irreversible_block_num": 4313,
  "last_irreversible_block_id": "000010d9f26bced3fcb533fd3242d847056c2788560b943ce88aa9a25da5aea0",
  "head_block_id": "00001117897a9d869bded64e3c8005f2a6a7c23c7a020c2c830785af7b23423b",
  "head_block_time": "2018-07-24T06:10:36.500",
  "head_block_producer": "producer.b",
  "virtual_block_cpu_limit": 15867397,
  "virtual_block_net_limit": 83358884,
  "block_cpu_limit": 199900,
  "block_net_limit": 1048576
}
```

Environment Var.
---

```bash
YOSEMITE_HOME=/mnt/yosemite-public-blockchain-git
YOSEMITE_NODEOS=$YOSEMITE_HOME/build/programs/nodeos/nodeos
YOSEMITE_CLEOS=$YOSEMITE_HOME/build/programs/cleos/cleos
YOSEMITE_TESTNET_CLEOS="$YOSEMITE_HOME/build/programs/cleos/cleos -u http://testnet.yosemitelabs.org:8888"
YOSEMITE_KEOSD=$YOSEMITE_HOME/build/programs/keosd/keosd
YOSEMITE_NODE_CONFIG=$YOSEMITE_HOME/yosemite_config/config_yosemite_testnet_boot.ini
YOSEMITE_NODE_GENESIS_JSON=$YOSEMITE_HOME/yosemite_config/genesis_yosemite_testnet.json
YOSEMITE_NODE_DATA_DIR=/mnt/yosemite_node_data
YOSEMITE_DEV_WALLET_DIR=/mnt/yosemite_dev_wallet
YOSEMITE_CONTRACTS_DIR=$YOSEMITE_HOME/build/contracts
```

Initial key
---
```bash
$YOSEMITE_CLEOS create key
Private key: 5KR14orrckPKBxUe4zUZCoY8GF8xhYUtKjiriSnsTGhUKrZTHxo
Public key: EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
```

Genesis Json
---
```json
{
  "initial_timestamp": "2018-07-23T12:00:00.000",
  "initial_key": "EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9",
  "initial_configuration": {
    "max_block_net_usage": 1048576,
    "target_block_net_usage_pct": 1000,
    "max_transaction_net_usage": 524288,
    "base_per_transaction_net_usage": 12,
    "net_usage_leeway": 500,
    "context_free_discount_net_usage_num": 20,
    "context_free_discount_net_usage_den": 100,
    "max_block_cpu_usage": 200000,
    "target_block_cpu_usage_pct": 1000,
    "max_transaction_cpu_usage": 150000,
    "min_transaction_cpu_usage": 100,
    "max_transaction_lifetime": 3600,
    "deferred_trx_expiration_window": 600,
    "max_transaction_delay": 3888000,
    "max_inline_action_size": 4096,
    "max_inline_action_depth": 4,
    "max_authority_depth": 6
  }
}
```

Boot Node
---

* config_yosemite_testnet_boot.ini
```ini
chain-state-db-size-mb = 4096
read-mode = speculative
filter-on = *
contracts-console = true
http-server-address = 0.0.0.0:8888
mongodb-uri = mongodb://localhost:27017/eosmain
p2p-listen-endpoint = 127.0.0.1:9876
p2p-max-nodes-per-host = 25
agent-name = "YOSEMITE Testnet Dev Agent"
max-clients = 25
enable-stale-production = true
producer-name = eosio
signature-provider = EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9=KEY:5KR14orrckPKBxUe4zUZCoY8GF8xhYUtKjiriSnsTGhUKrZTHxo
plugin = eosio::http_plugin
plugin = eosio::chain_plugin
plugin = eosio::chain_api_plugin
plugin = eosio::producer_plugin
plugin = eosio::mongo_db_plugin
plugin = eosio::history_plugin
plugin = eosio::history_api_plugin
```

* fresh genesis state
```bash
$YOSEMITE_NODEOS --config $YOSEMITE_NODE_CONFIG --genesis-json $YOSEMITE_NODE_GENESIS_JSON --data-dir $YOSEMITE_NODE_DATA_DIR
```

* from existing node data
```bash
$YOSEMITE_NODEOS --print-genesis-json --config $YOSEMITE_NODE_CONFIG --data-dir $YOSEMITE_NODE_DATA_DIR
nohup $YOSEMITE_NODEOS --config $YOSEMITE_NODE_CONFIG --data-dir $YOSEMITE_NODE_DATA_DIR > /mnt/nodeos.log 2>&1&
tail -f /mnt/nodeos.log -n 300
pgrep nodeos
pkill -SIGINT nodeos
```

* replay blockchain from tx log
```bash
$YOSEMITE_NODEOS --config $YOSEMITE_NODE_CONFIG --data-dir $YOSEMITE_NODE_DATA_DIR --replay-blockchain
```

Start Wallet (keosd)
---

* run keosd
```bash
nohup $YOSEMITE_KEOSD --unlock-timeout 999999999 --http-server-address 127.0.0.1:8900 --wallet-dir $YOSEMITE_DEV_WALLET_DIR > /mnt/keosd.log 2>&1&
tail -f /mnt/keosd.log -n 300
```

* create wallet / unlock
```bash
$YOSEMITE_CLEOS wallet create
Creating wallet: default
"PW5KH7i8ZEuVMvywMschs3TznhTfCdmgpPBGKJLUQjs6N6oQ7boZj"

$YOSEMITE_CLEOS wallet open
$YOSEMITE_CLEOS wallet unlock
```

* eosio initial key
```bash
$YOSEMITE_CLEOS wallet import --private-key 5KR14orrckPKBxUe4zUZCoY8GF8xhYUtKjiriSnsTGhUKrZTHxo
```

Create System Accounts
---

* eosio.msig(priviliged), yx.kyc, yx.ntoken(priviliged), yx.token(priviliged)
```bash
$YOSEMITE_CLEOS create account eosio eosio.msig EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLEOS create account eosio yx.kyc EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLEOS create account eosio yx.ntoken EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLEOS create account eosio yx.token EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
```

Create initial System Depository Account
---

* sysdepo
```bash
$YOSEMITE_CLEOS create key
Private key: 5KCqedPhHjbaBRi4ujhFsS3aGCcNuXwVGrMaxRW3DvB5Wo3beax
Public key: EOS7myk8qduMNnxo9Q9RZ2uxfaerQhXMPtxKUhxKh5HfSSM1dHeQo
$YOSEMITE_CLEOS wallet import --private-key 5KCqedPhHjbaBRi4ujhFsS3aGCcNuXwVGrMaxRW3DvB5Wo3beax

$YOSEMITE_CLEOS create account eosio sysdepo EOS7myk8qduMNnxo9Q9RZ2uxfaerQhXMPtxKUhxKh5HfSSM1dHeQo

```

* sysdepo2
```bash
$YOSEMITE_CLEOS create key
Private key: 5HyRsRWBi4bUXjmnmaskw9BU8a2sdqn3Jfqt21Few9xUyYR5tXr
Public key: EOS62AXYcUxcSQQgKhnQ1oid2w1XNvCKxmPgk7C8fEi4FnQKhHLwM
$YOSEMITE_CLEOS wallet import --private-key 5HyRsRWBi4bUXjmnmaskw9BU8a2sdqn3Jfqt21Few9xUyYR5tXr

$YOSEMITE_CLEOS create account eosio sysdepo2 EOS62AXYcUxcSQQgKhnQ1oid2w1XNvCKxmPgk7C8fEi4FnQKhHLwM
```

Install Yosemite System Contract
---
```bash
$YOSEMITE_CLEOS set contract eosio $YOSEMITE_CONTRACTS_DIR/yx.system/ -p eosio@active
```

Install Multi-sig System Contract
---
```bash
$YOSEMITE_CLEOS set contract eosio.msig $YOSEMITE_CONTRACTS_DIR/eosio.msig/ -p eosio.msig@active
$YOSEMITE_CLEOS push action eosio setpriv '["eosio.msig",1]' -p eosio@active
```

Register Initial System Depository
---
```bash
$YOSEMITE_CLEOS push action eosio regsysdepo '["sysdepo","http://sysdepo.org",1]' -p sysdepo@active -p eosio@active
$YOSEMITE_CLEOS push action eosio authsysdepo '["sysdepo"]' -p eosio@active

$YOSEMITE_CLEOS push action eosio regsysdepo '["sysdepo2","http://sysdepo2.org",1]' -p sysdepo2@active -p eosio@active
$YOSEMITE_CLEOS push action eosio authsysdepo '["sysdepo2"]' -p eosio@active
```

Install KYC System Contract
---
```bash
$YOSEMITE_CLEOS set contract yx.kyc $YOSEMITE_CONTRACTS_DIR/yx.kyc/
```

Install Native Token Contract
---
```bash
$YOSEMITE_CLEOS set contract yx.ntoken $YOSEMITE_CONTRACTS_DIR/yx.ntoken/
$YOSEMITE_CLEOS push action eosio setpriv '["yx.ntoken",1]' -p eosio@active
```

Install User Token Contract
---
```bash
$YOSEMITE_CLEOS set contract yx.token $YOSEMITE_CONTRACTS_DIR/yx.token/
$YOSEMITE_CLEOS push action eosio setpriv '["yx.token",1]' -p eosio@active
```

Querying the status of System Depositories and Block Producers
---
```bash
$YOSEMITE_CLEOS get table eosio eosio global
$YOSEMITE_CLEOS get table eosio eosio sysdepos
$YOSEMITE_CLEOS get table eosio eosio producers
```

Initial Block Producer Setup
---

* config_yosemite_testnet_boot.ini (one node multi producer setup)
```ini
producer-name = eosio
producer-name = producer.a
producer-name = producer.b
producer-name = producer.c
producer-name = producer.d
producer-name = producer.e

# producer.a
signature-provider = EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y=KEY:5JuTjNtFmqWG1ivMLJgeRgo5w2WV5iwHa2u1n8owvpyUdy7DUr6
# producer.b
signature-provider = EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS=KEY:5JbXQdi7vzRXtxnyBi9PSTzaxACJagDKUFQkLE7Dg9ZYMyykJR8
# producer.c
signature-provider = EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe=KEY:5JEJrZMbDSZmFfRUbt1JVC3wrBv9cJG7QaiP58f8Mr9K1Wf9Cnn
# producer.d
signature-provider = EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ=KEY:5HsDW4pNKWDA91WcBuNNXT8sL5soVhXLHqZU7ZZqJzJC1YghXjT
# producer.e
signature-provider = EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW=KEY:5JFbKCZray6WGQZtkkxqQ9mencwQn5qynjJ1tXJRtaUH7oNRGnP
```

* create block producer accounts
```bash
$YOSEMITE_CLEOS create key
Private key: 5JuTjNtFmqWG1ivMLJgeRgo5w2WV5iwHa2u1n8owvpyUdy7DUr6
Public key: EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y
$YOSEMITE_CLEOS wallet import --private-key 5JuTjNtFmqWG1ivMLJgeRgo5w2WV5iwHa2u1n8owvpyUdy7DUr6

$YOSEMITE_CLEOS create key
Private key: 5JbXQdi7vzRXtxnyBi9PSTzaxACJagDKUFQkLE7Dg9ZYMyykJR8
Public key: EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS
$YOSEMITE_CLEOS wallet import --private-key 5JbXQdi7vzRXtxnyBi9PSTzaxACJagDKUFQkLE7Dg9ZYMyykJR8

$YOSEMITE_CLEOS create key
Private key: 5JEJrZMbDSZmFfRUbt1JVC3wrBv9cJG7QaiP58f8Mr9K1Wf9Cnn
Public key: EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe
$YOSEMITE_CLEOS wallet import --private-key 5JEJrZMbDSZmFfRUbt1JVC3wrBv9cJG7QaiP58f8Mr9K1Wf9Cnn

$YOSEMITE_CLEOS create key
Private key: 5HsDW4pNKWDA91WcBuNNXT8sL5soVhXLHqZU7ZZqJzJC1YghXjT
Public key: EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ
$YOSEMITE_CLEOS wallet import --private-key 5HsDW4pNKWDA91WcBuNNXT8sL5soVhXLHqZU7ZZqJzJC1YghXjT

$YOSEMITE_CLEOS create key
Private key: 5JFbKCZray6WGQZtkkxqQ9mencwQn5qynjJ1tXJRtaUH7oNRGnP
Public key: EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW
$YOSEMITE_CLEOS wallet import --private-key 5JFbKCZray6WGQZtkkxqQ9mencwQn5qynjJ1tXJRtaUH7oNRGnP

$YOSEMITE_CLEOS create account eosio producer.a EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y -p eosio@active
$YOSEMITE_CLEOS create account eosio producer.b EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS -p eosio@active
$YOSEMITE_CLEOS create account eosio producer.c EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe -p eosio@active
$YOSEMITE_CLEOS create account eosio producer.d EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ -p eosio@active
$YOSEMITE_CLEOS create account eosio producer.e EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW -p eosio@active
```

* register and authorize block producers (permissioned)

```bash
$YOSEMITE_CLEOS push action eosio regproducer '["producer.a","EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y","http://producera.io",1]' -p producer.a@active -p eosio@active
$YOSEMITE_CLEOS push action eosio regproducer '["producer.b","EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS","http://producerb.io",1]' -p producer.b@active -p eosio@active

$YOSEMITE_CLEOS push action eosio authproducer '["producer.a"]' -p eosio@active
$YOSEMITE_CLEOS push action eosio authproducer '["producer.b"]' -p eosio@active

$YOSEMITE_CLEOS push action eosio regproducer '["producer.c","EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe","http://producerc.io",1]' -p producer.c@active -p eosio@active
$YOSEMITE_CLEOS push action eosio regproducer '["producer.d","EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ","http://producerd.io",1]' -p producer.d@active -p eosio@active
$YOSEMITE_CLEOS push action eosio regproducer '["producer.e","EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW","http://producere.io",1]' -p producer.e@active -p eosio@active

$YOSEMITE_CLEOS push action eosio authproducer '["producer.c"]' -p eosio@active
$YOSEMITE_CLEOS push action eosio authproducer '["producer.d"]' -p eosio@active
$YOSEMITE_CLEOS push action eosio authproducer '["producer.e"]' -p eosio@active
```

Create Native Token
---
```bash
$YOSEMITE_CLEOS push action yx.ntoken setfee '[ "issuen", "0.0000 DKRW" ]' -p yx.ntoken@active
$YOSEMITE_CLEOS push action yx.ntoken setfee '[ "redeemn", "1000.0000 DKRW" ]' -p yx.ntoken@active
$YOSEMITE_CLEOS push action yx.ntoken setfee '[ "transfern", "10.0000 DKRW" ]' -p yx.ntoken@active
$YOSEMITE_CLEOS push action yx.ntoken setfee '[ "transfer", "5.0000 DKRW" ]' -p yx.ntoken@active

$YOSEMITE_CLEOS push action yx.ntoken createn '["sysdepo"]' -p sysdepo@active
```

Resign "eosio" delegating authority to "eosio.prods"
---
```bash
$YOSEMITE_CLEOS get account eosio
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"eosio","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio.prods","permission":"active"}}]}}' -p eosio@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"eosio","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio.prods","permission":"active"}}]}}' -p eosio@active
$YOSEMITE_CLEOS get account eosio

$YOSEMITE_CLEOS get account eosio.prods
permissions: 
     owner     1:    
        active     4:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
           prod.major     3:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
              prod.minor     2:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
```

Resign "eosio.msig" delegating authority to "eosio"
---
```bash
$YOSEMITE_CLEOS get account eosio.msig
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"eosio.msig","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p eosio.msig@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"eosio.msig","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p eosio.msig@active
$YOSEMITE_CLEOS get account eosio.msig
```

Resign "yx.ntoken", "yx.token", "yx.kyc" delegating authority to "eosio"
---
```bash
$YOSEMITE_CLEOS get account yx.ntoken
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.ntoken","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.ntoken@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.ntoken","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.ntoken@active
$YOSEMITE_CLEOS get account yx.ntoken

$YOSEMITE_CLEOS get account yx.token
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.token","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.token@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.token","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.token@active
$YOSEMITE_CLEOS get account yx.token

$YOSEMITE_CLEOS get account yx.kyc
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.kyc","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.kyc@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.kyc","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.kyc@active
$YOSEMITE_CLEOS get account yx.ntoken
```

Unauthorizing Block Producer
---
```bash
$YOSEMITE_CLEOS push action -j -d eosio rmvproducer '["producer.e"]' -p eosio@active
#$YOSEMITE_CLEOS push action eosio rmvproducer '["producer.e"]' -p eosio@active
#$YOSEMITE_CLEOS push action eosio rmvproducer '["producer.d"]' -p eosio@active
$YOSEMITE_CLEOS get account eosio.prods
```

Create Native Token Issue / Transfer
---
```bash
$YOSEMITE_CLEOS push action yx.kyc setinfo '{"account":"sysdepo", "depository":"sysdepo", "kycvector":15, "addendum":"kyc by sysdepo"}' -p sysdepo@active
$YOSEMITE_CLEOS push action yx.kyc setinfo '{"account":"producer.a", "depository":"sysdepo", "kycvector":15, "addendum":"kyc by sysdepo"}' -p sysdepo@active
$YOSEMITE_CLEOS push action yx.ntoken issuen '["producer.a",{"quantity":"100000.0000 DKRW","issuer":"sysdepo"},"issuen test"]' -p sysdepo@active

$YOSEMITE_CLEOS push action yx.kyc setinfo '{"account":"producer.b", "depository":"sysdepo", "kycvector":15, "addendum":"kyc by sysdepo"}' -p sysdepo@active
$YOSEMITE_CLEOS push action yx.ntoken issuen '["producer.b",{"quantity":"100000.0000 DKRW","issuer":"sysdepo"},"issuen test"]' -p sysdepo@active

$YOSEMITE_CLEOS push action yx.ntoken transfern '[ "producer.a", "producer.b", {"quantity":"10000.0000 DKRW","issuer":"sysdepo"}, "producer.a", "memo" ]' -p producer.a

$YOSEMITE_CLEOS push action yx.ntoken printbalance '{"owner":"producer.a", "symbol":{"symbol":"4,DKRW","issuer":"sysdepo"}}' -p yx.ntoken
```

Claim Producer Rewards
---
```bash
$YOSEMITE_CLEOS push action yx.kyc setinfo '{"account":"producer.e", "depository":"sysdepo", "kycvector":15, "addendum":"kyc by sysdepo"}' -p sysdepo@active
$YOSEMITE_CLEOS push action eosio claimrewards '["producer.e"]' -p producer.e@active
$YOSEMITE_CLEOS get table eosio eosio producers
```

Synchronize action data 
---
```bash
$YOSEMITE_TESTNET_CLEOS get actions yx.ntoken
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#    0   2018-07-24T05:37:15.000            eosio::setcode => eosio         7245985d... {"account":"yx.ntoken","vmtype":0,"vmversion":0,"code":"0061...
#    1   2018-07-24T05:37:15.000             eosio::setabi => eosio         7245985d... {"account":"yx.ntoken","abi":"0e656f73696f3a3a6162692f312e30...
#    2   2018-07-24T05:43:14.500         yx.ntoken::setfee => yx.ntoken     d0765b61... {"operation":"issuen","fee":"0.0000 DKRW"}...
#    3   2018-07-24T05:43:19.000         yx.ntoken::setfee => yx.ntoken     3ca8bc7a... {"operation":"redeemn","fee":"1000.0000 DKRW"}...
#    4   2018-07-24T05:43:23.500         yx.ntoken::setfee => yx.ntoken     be3988b2... {"operation":"transfern","fee":"10.0000 DKRW"}...
#    5   2018-07-24T05:43:28.500         yx.ntoken::setfee => yx.ntoken     1a10d2b1... {"operation":"transfer","fee":"5.0000 DKRW"}...
#    6   2018-07-24T05:43:40.500        yx.ntoken::createn => yx.ntoken     a181210c... {"depository":"sysdepo"}...
#    7   2018-07-24T05:46:35.000         eosio::updateauth => eosio         37e16971... {"account":"yx.ntoken","permission":"owner","parent":"","aut...
#    8   2018-07-24T05:46:39.500         eosio::updateauth => eosio         10aadff3... {"account":"yx.ntoken","permission":"active","parent":"owner...
#    9   2018-07-24T05:48:06.000         yx.ntoken::issuen => yx.ntoken     a60a8741... {"to":"producer.a","quantity":{"quantity":"100000.0000 DKRW"...
#   10   2018-07-24T05:48:17.500         yx.ntoken::issuen => yx.ntoken     af6f9b44... {"to":"producer.b","quantity":{"quantity":"100000.0000 DKRW"...
#   11   2018-07-24T05:48:33.500      yx.ntoken::transfern => yx.ntoken     8fe5f464... {"from":"producer.a","to":"producer.b","quantity":{"quantity...
#   12   2018-07-24T05:49:07.000   yx.ntoken::printbalance => yx.ntoken     f12c119a... {"owner":"producer.a","symbol":{"symbol":"4,DKRW","issuer":"...
#   13   2018-07-24T05:49:19.500   yx.ntoken::printbalance => yx.ntoken     3221cf3e... {"owner":"eosio","symbol":{"symbol":"4,DKRW","issuer":"sysde...
#   14   2018-07-24T05:49:29.500      yx.ntoken::transfern => yx.ntoken     b13b64b3... {"from":"producer.a","to":"producer.b","quantity":{"quantity...
#   15   2018-07-24T05:49:32.500      yx.ntoken::transfern => yx.ntoken     be17abfd... {"from":"producer.a","to":"producer.b","quantity":{"quantity...
...

$YOSEMITE_TESTNET_CLEOS get actions yx.ntoken -1 -1
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#   19   2018-07-24T05:50:25.000   yx.ntoken::printbalance => yx.ntoken     2d50cf84... {"owner":"producer.e","symbol":{"symbol":"4,DKRW","issuer":"...
```

Get transaction data
---

```bash
$YOSEMITE_TESTNET_CLEOS get transaction aeff830d9467eb8ee8e821c533b756f5312c505e144d2abefb33ccbbf819d878
{
  "id": "aeff830d9467eb8ee8e821c533b756f5312c505e144d2abefb33ccbbf819d878",
  "trx": {
    "receipt": {
      "status": "executed",
      "cpu_usage_us": 1192,
      "net_usage_words": 24,
      "trx": [
        1,{
          "signatures": [
            "SIG_K1_K6KUxsTsF8W4u1Kn2j9CFALRBEea7VW2GgysVKDHGjzMZgQuWYqHMaQWiT6LvjoTTjSFmoHZC519nuEJT2DgPJrfXTSeRg",
            "SIG_K1_K6RHk5HDbi8ZiA8aD3ePWD2wudWey9ASLPz3w4wEKX9TkA3CxbgN7CVB44HyUJSeLZpuqvMTPbDhnoSWUJ9vCu1Ukq9u6P"
          ],
          "compression": "none",
          "packed_context_free_data": "",
          "packed_trx": "e2bb565ba50543b3cd4e00000000010000000000ea305500ae423ad15b99ba0200400257219de8ad00000000a8ed32320000000000ea305500000000a8ed32324000400257219de8ad00029e315fcbb95274a7b8ff1e8cf20016a646c917771849ae5cf37af5c8ef18a68213687474703a2f2f70726f6475636572642e696f010000"
        }
      ]
    },
    "trx": {
      "expiration": "2018-07-24T05:40:50",
      "ref_block_num": 1445,
      "ref_block_prefix": 1322103619,
      "max_net_usage_words": 0,
      "max_cpu_usage_ms": 0,
      "delay_sec": 0,
      "context_free_actions": [],
      "actions": [{
          "account": "eosio",
          "name": "regproducer",
          "authorization": [{
              "actor": "producer.d",
              "permission": "active"
            },{
              "actor": "eosio",
              "permission": "active"
            }
          ],
          "data": {
            "producer": "producer.d",
            "producer_key": "EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ",
            "url": "http://producerd.io",
            "location": 1
          },
          "hex_data": "00400257219de8ad00029e315fcbb95274a7b8ff1e8cf20016a646c917771849ae5cf37af5c8ef18a68213687474703a2f2f70726f6475636572642e696f0100"
        }
      ],
      "transaction_extensions": [],
      "signatures": [
        "SIG_K1_K6KUxsTsF8W4u1Kn2j9CFALRBEea7VW2GgysVKDHGjzMZgQuWYqHMaQWiT6LvjoTTjSFmoHZC519nuEJT2DgPJrfXTSeRg",
        "SIG_K1_K6RHk5HDbi8ZiA8aD3ePWD2wudWey9ASLPz3w4wEKX9TkA3CxbgN7CVB44HyUJSeLZpuqvMTPbDhnoSWUJ9vCu1Ukq9u6P"
      ],
      "context_free_data": []
    }
  },
  "block_time": "2018-07-24T05:40:20.500",
  "block_num": 1448,
  "last_irreversible_block": 7793,
  "traces": [{
      "receipt": {
        "receiver": "eosio",
        "act_digest": "6b0d465f2772ca5fc91aa1cfd6e4bd1a72db0125bf3cd5b76088a11ec798ae44",
        "global_sequence": 1481,
        "recv_sequence": 1481,
        "auth_sequence": [[
            "eosio",
            1473
          ],[
            "producer.d",
            1
          ]
        ],
        "code_sequence": 1,
        "abi_sequence": 1
      },
      "act": {
        "account": "eosio",
        "name": "regproducer",
        "authorization": [{
            "actor": "producer.d",
            "permission": "active"
          },{
            "actor": "eosio",
            "permission": "active"
          }
        ],
        "data": {
          "producer": "producer.d",
          "producer_key": "EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ",
          "url": "http://producerd.io",
          "location": 1
        },
        "hex_data": "00400257219de8ad00029e315fcbb95274a7b8ff1e8cf20016a646c917771849ae5cf37af5c8ef18a68213687474703a2f2f70726f6475636572642e696f0100"
      },
      "elapsed": 1140,
      "cpu_usage": 0,
      "console": "",
      "total_cpu_usage": 0,
      "trx_id": "aeff830d9467eb8ee8e821c533b756f5312c505e144d2abefb33ccbbf819d878",
      "inline_traces": []
    }
  ]
}
```