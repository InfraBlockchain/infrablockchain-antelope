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
  "initial_timestamp": "2018-07-30T12:00:00.000",
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
mongodb-uri = mongodb://localhost:27017/yosemitetest
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

* eosio.msig(privileged), yx.txfee, yx.identity, yx.ntoken(privileged), yx.token(privileged), yx.dcontract(privileged)
```bash
$YOSEMITE_CLEOS create account eosio eosio.msig EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLEOS create account eosio yx.txfee EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLEOS create account eosio yx.identity EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLEOS create account eosio yx.ntoken EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLEOS create account eosio yx.token EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLEOS create account eosio yx.dcontract EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9

```

Create initial System Depository Account
---

* sysdepo1
```bash
$YOSEMITE_CLEOS create key
Private key: 5KCqedPhHjbaBRi4ujhFsS3aGCcNuXwVGrMaxRW3DvB5Wo3beax
Public key: EOS7myk8qduMNnxo9Q9RZ2uxfaerQhXMPtxKUhxKh5HfSSM1dHeQo
$YOSEMITE_CLEOS wallet import --private-key 5KCqedPhHjbaBRi4ujhFsS3aGCcNuXwVGrMaxRW3DvB5Wo3beax

$YOSEMITE_CLEOS create account eosio sysdepo1 EOS7myk8qduMNnxo9Q9RZ2uxfaerQhXMPtxKUhxKh5HfSSM1dHeQo

```

* sysdepo2
```bash
$YOSEMITE_CLEOS create key
Private key: 5HyRsRWBi4bUXjmnmaskw9BU8a2sdqn3Jfqt21Few9xUyYR5tXr
Public key: EOS62AXYcUxcSQQgKhnQ1oid2w1XNvCKxmPgk7C8fEi4FnQKhHLwM
$YOSEMITE_CLEOS wallet import --private-key 5HyRsRWBi4bUXjmnmaskw9BU8a2sdqn3Jfqt21Few9xUyYR5tXr

$YOSEMITE_CLEOS create account eosio sysdepo2 EOS62AXYcUxcSQQgKhnQ1oid2w1XNvCKxmPgk7C8fEi4FnQKhHLwM
```

Create initial Identity Authority Account
---

* idauth1
```bash
$YOSEMITE_CLEOS create key
Private key: 5KND1U57MvsdWzPi8TY53XpaBPBGQNTdQSTDtEexTVexz9mP6Q9
Public key: EOS5gFJS9EhjNpvcaGuDQqaeGQnNLuF9sWUt2s8surF1tRtSzLJiG
$YOSEMITE_CLEOS wallet import --private-key 5KND1U57MvsdWzPi8TY53XpaBPBGQNTdQSTDtEexTVexz9mP6Q9

$YOSEMITE_CLEOS create account eosio idauth1 EOS5gFJS9EhjNpvcaGuDQqaeGQnNLuF9sWUt2s8surF1tRtSzLJiG

```

* idauth2
```bash
$YOSEMITE_CLEOS create key
Private key: 5K25HU8a7qgkfb9o2skiNupEs9H3wE6CWSd9qjako2gnFioekaX
Public key: EOS68JZEkVt4Z3dkrSmQ4SkH8XyVJVKTmnvawZyWcrxzhoH1WMp4Q
$YOSEMITE_CLEOS wallet import --private-key 5K25HU8a7qgkfb9o2skiNupEs9H3wE6CWSd9qjako2gnFioekaX

$YOSEMITE_CLEOS create account eosio idauth2 EOS68JZEkVt4Z3dkrSmQ4SkH8XyVJVKTmnvawZyWcrxzhoH1WMp4Q
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

Install Transaction Fee System Contract
---
```bash
$YOSEMITE_CLEOS set contract yx.txfee $YOSEMITE_CONTRACTS_DIR/yx.txfee/ -p yx.txfee@active
```

Install Identity System Contract
---
```bash
$YOSEMITE_CLEOS set contract yx.identity $YOSEMITE_CONTRACTS_DIR/yx.identity/ -p yx.identity@active
```

Register Initial Identity Authority
---
```bash
$YOSEMITE_CLEOS push action eosio regidauth '["idauth1","http://idauth1.org",1]' -p idauth1@active -p eosio@active
$YOSEMITE_CLEOS push action eosio authidauth '["idauth1"]' -p eosio@active

$YOSEMITE_CLEOS push action eosio regidauth '["idauth2","http://idauth2.org",1]' -p idauth2@active -p eosio@active
$YOSEMITE_CLEOS push action eosio authidauth '["idauth2"]' -p eosio@active

//$YOSEMITE_CLEOS push action eosio rmvidauth '["idauth2"]' -p eosio@active

$YOSEMITE_CLEOS get table eosio eosio idauthority
```

Register Initial System Depository
---
```bash
$YOSEMITE_CLEOS push action eosio regsysdepo '["sysdepo1","http://sysdepo1.org",1]' -p sysdepo1@active -p eosio@active
$YOSEMITE_CLEOS push action eosio authsysdepo '["sysdepo1"]' -p eosio@active

$YOSEMITE_CLEOS push action eosio regsysdepo '["sysdepo2","http://sysdepo2.org",1]' -p sysdepo2@active -p eosio@active
$YOSEMITE_CLEOS push action eosio authsysdepo '["sysdepo2"]' -p eosio@active

$YOSEMITE_CLEOS push action yx.identity setidinfo "{\"account\":\"sysdepo1\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"sysdepo\"}" -p idauth1@active
$YOSEMITE_CLEOS push action yx.identity setidinfo "{\"account\":\"sysdepo2\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"sysdepo\"}" -p idauth1@active
$YOSEMITE_CLEOS get table -L sysdepo1 -l 1 yx.identity yx.identity identity
$YOSEMITE_CLEOS get table -L sysdepo2 -l 1 yx.identity yx.identity identity

$YOSEMITE_CLEOS get table eosio eosio sysdepos
```

Install Native Token Contract
---
```bash
$YOSEMITE_CLEOS set contract yx.ntoken $YOSEMITE_CONTRACTS_DIR/yx.ntoken/
$YOSEMITE_CLEOS push action eosio setpriv '["yx.ntoken",1]' -p eosio@active

$YOSEMITE_CLEOS push action yx.ntoken setkycrule "{\"type\":0, \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p eosio@active
$YOSEMITE_CLEOS push action yx.ntoken setkycrule "{\"type\":1, \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p eosio@active

```

Install User Token Contract
---
```bash
$YOSEMITE_CLEOS set contract yx.token $YOSEMITE_CONTRACTS_DIR/yx.token/
$YOSEMITE_CLEOS push action eosio setpriv '["yx.token",1]' -p eosio@active
```

Install DContract Contract
---
```bash
$YOSEMITE_CLEOS set contract yx.dcontract $YOSEMITE_CONTRACTS_DIR/yx.dcontract/
$YOSEMITE_CLEOS push action eosio setpriv '["yx.dcontract",1]' -p eosio@active
```

Querying the status of System Depositories, Identity Authorities and Block Producers
---
```bash
$YOSEMITE_CLEOS get table eosio eosio global
$YOSEMITE_CLEOS get table eosio eosio sysdepos
$YOSEMITE_CLEOS get table eosio eosio idauthority
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

Setup Initial Transaction Fees
---
```bash
// yx.system
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.newacc", "1000.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.regprod", "3000000.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.regsysdep", "2000000.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.regidauth", "2000000.0000 DKRW" ]' -p eosio@active

// yx.ntoken
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.nissue", "0.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.nredeem", "1000.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.transfer", "100.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.ntransfer", "200.0000 DKRW" ]' -p eosio@active

// yx.token
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.tcreate", "10000.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.tissue", "500.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.tredeem", "500.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.ttransfer", "100.0000 DKRW" ]' -p eosio@active

// yx.dcontract
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.dccreate", "500.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.dcaddsign", "100.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.dcsign", "300.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.dcupadd", "50.0000 DKRW" ]' -p eosio@active
$YOSEMITE_CLEOS push action yx.txfee settxfee '[ "tf.dcremove", "0.0000 DKRW" ]' -p eosio@active

$YOSEMITE_CLEOS get table -l 100 yx.txfee yx.txfee txfees
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

Resign "yx.txfee", "yx.identity", "yx.ntoken", "yx.token", "yx.dcontract" delegating authority to "eosio"
---
```bash
$YOSEMITE_CLEOS get account yx.txfee
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.txfee","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.txfee@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.txfee","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.txfee@active
$YOSEMITE_CLEOS get account yx.txfee

$YOSEMITE_CLEOS get account yx.identity
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.identity","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.identity@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.identity","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.identity@active
$YOSEMITE_CLEOS get account yx.identity

$YOSEMITE_CLEOS get account yx.ntoken
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.ntoken","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.ntoken@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.ntoken","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.ntoken@active
$YOSEMITE_CLEOS get account yx.ntoken

$YOSEMITE_CLEOS get account yx.token
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.token","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.token@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.token","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.token@active
$YOSEMITE_CLEOS get account yx.token

$YOSEMITE_CLEOS get account yx.dcontract
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.dcontract","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.dcontract@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"yx.dcontract","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio","permission":"active"}}]}}' -p yx.dcontract@active
$YOSEMITE_CLEOS get account yx.dcontract
```

Unauthorizing Block Producer
---
```bash
$YOSEMITE_CLEOS push action -j -d eosio rmvproducer '["producer.e"]' -p eosio@active
#$YOSEMITE_CLEOS push action eosio rmvproducer '["producer.e"]' -p eosio@active
#$YOSEMITE_CLEOS push action eosio rmvproducer '["producer.d"]' -p eosio@active
$YOSEMITE_CLEOS get account eosio.prods
```

Create New User Accounts
---

```bash
$YOSEMITE_CLEOS create key
Private key: 5JsqBiEGge54oBRPEuPZ4rTm8EHxzDNzXMbwNoTh8y5Lms1YMJT
Public key: EOS7qw27KHaVWJEgA113u9xQVUtEubUKFhRJfiDt752iAqb6qbv9V
$YOSEMITE_CLEOS wallet import --private-key 5JsqBiEGge54oBRPEuPZ4rTm8EHxzDNzXMbwNoTh8y5Lms1YMJT

$YOSEMITE_CLEOS create key
Private key: 5JMsmWCrQ7YEDfUNhmPts7W3AuUFxeWpionyP8EuMf35kG3AeCU
Public key: EOS7hbLPy5cmtNBdHibrS8CD1YUsKbvq7t9nGVvnJtTKHvxsq6ynf
$YOSEMITE_CLEOS wallet import --private-key 5JMsmWCrQ7YEDfUNhmPts7W3AuUFxeWpionyP8EuMf35kG3AeCU

$YOSEMITE_CLEOS create key
Private key: 5K2iUzbSy4qyEzjf93DGDxBMrL8c8VGnSvLhE26DEJhEwvVEwj8
Public key: EOS75qaGrHrXskTUJLoedxwc7KbLxksfgiYBpgeiMcTCsnhiuhx6U
$YOSEMITE_CLEOS wallet import --private-key 5K2iUzbSy4qyEzjf93DGDxBMrL8c8VGnSvLhE26DEJhEwvVEwj8

$YOSEMITE_CLEOS create key
Private key: 5HrWpMMjCM1UhxKAvCg8NKRyExapwS1vyiQXe8VBpNEDsNMn4nE
Public key: EOS584bQn471vv2q7N1xeDXayfDtiim8kPsNm45TRuJ9D51YPFFmX
$YOSEMITE_CLEOS wallet import --private-key 5HrWpMMjCM1UhxKAvCg8NKRyExapwS1vyiQXe8VBpNEDsNMn4nE

$YOSEMITE_CLEOS create key
Private key: 5HsUEcT7J56iRrHFiGR9pGeQAiLaow4AbR7QA38w9T44SBnEMYk
Public key: EOS5UbAi7wTM1wpFNp81bghD9EfV1HHSn8n39Qz4jKD1oHmW7oyNS
$YOSEMITE_CLEOS wallet import --private-key 5HsUEcT7J56iRrHFiGR9pGeQAiLaow4AbR7QA38w9T44SBnEMYk

// Identity Authority can create accounts having 12 chracters without paying 'tf.newacc' transaction fee
$YOSEMITE_CLEOS create account idauth1 useraccount1 EOS7qw27KHaVWJEgA113u9xQVUtEubUKFhRJfiDt752iAqb6qbv9V -p idauth1@active
$YOSEMITE_CLEOS create account idauth1 useraccount2 EOS7hbLPy5cmtNBdHibrS8CD1YUsKbvq7t9nGVvnJtTKHvxsq6ynf -p idauth1@active
$YOSEMITE_CLEOS create account idauth1 useraccount3 EOS75qaGrHrXskTUJLoedxwc7KbLxksfgiYBpgeiMcTCsnhiuhx6U -p idauth1@active

// Only system account can create accounts having less than 12 characters
$YOSEMITE_CLEOS create account eosio com EOS584bQn471vv2q7N1xeDXayfDtiim8kPsNm45TRuJ9D51YPFFmX -p eosio@active

// Only suffix account can create accounts having arbitrary prefix with the same suffix name
$YOSEMITE_CLEOS push action yx.identity setidinfo "{\"account\":\"com\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"1f32i7t23\"}" -p idauth1@active
$YOSEMITE_CLEOS push action yx.ntoken nissue '["com",{"asset":"5000000.0000 DKRW","issuer":"sysdepo1"},"nissue com"]' -p sysdepo1@active
executed transaction: e2dc09299d5596fa957da04a3ac7a33d4f4d1c5046ddf966638787eab078926a  136 bytes  3845 us
#     yx.ntoken <= yx.ntoken::nissue            {"to":"com","asset":{"asset":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"nissue com"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","asset":{"asset":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"niss...
#      sysdepo1 <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","asset":{"asset":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"niss...
#           com <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","asset":{"asset":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"niss...
$YOSEMITE_CLEOS create account com acquire.com EOS5UbAi7wTM1wpFNp81bghD9EfV1HHSn8n39Qz4jKD1oHmW7oyNS -p com@active

$YOSEMITE_CLEOS get table yx.ntoken sysdepo1 ntaccounts
$YOSEMITE_CLEOS get table yx.ntoken com ntaccounts
$YOSEMITE_CLEOS get table yx.ntoken yx.txfee ntaccounts

$YOSEMITE_CLEOS get table -L com -l 1 yx.identity yx.identity identity
``` 

Managing Account Identity Info (including KYC)
---
```bash
$YOSEMITE_CLEOS push action yx.identity setidinfo "{\"account\":\"useraccount1\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 0111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth1@active
$YOSEMITE_CLEOS push action yx.identity setidinfo "{\"account\":\"useraccount2\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"23uyiuye\"}" -p idauth1@active
$YOSEMITE_CLEOS push action yx.identity setidinfo "{\"account\":\"useraccount3\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"vewv23r3\"}" -p idauth1@active

$YOSEMITE_CLEOS get table -L useraccount1 -l 1 yx.identity yx.identity identity

// update user account type
$YOSEMITE_CLEOS push action yx.identity settype "{\"account\":\"useraccount1\", \"type\":$(echo 'ibase=2; 11111111' | bc)}" -p idauth1@active

// update kyc status
$YOSEMITE_CLEOS push action yx.identity setkyc "{\"account\":\"useraccount1\", \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p idauth1@active

// update account state (e.g. blacklist account)
$YOSEMITE_CLEOS push action yx.identity setstate "{\"account\":\"useraccount1\", \"state\":$(echo 'ibase=2; 0001' | bc)}" -p idauth1@active

// update identity-authority specific data
$YOSEMITE_CLEOS push action yx.identity setdata "{\"account\":\"useraccount1\", \"data\":\"23fiuygy3\"}" -p idauth1@active
```

Native Token Issue / Transfer
---

```bash

$YOSEMITE_CLEOS push action yx.ntoken nissue '["useraccount2",{"asset":"100000.0000 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active
executed transaction: 9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939  136 bytes  2655 us
#     yx.ntoken <= yx.ntoken::nissue            {"to":"useraccount2","asset":{"asset":"100000.0000 DKRW","issuer":"sysdepo1"},"memo":"nissue test"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"useraccount2","asset":{"asset":"100000.0000 DKRW","issuer":"sysdepo1"},"mem...
#      sysdepo1 <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"useraccount2","asset":{"asset":"100000.0000 DKRW","issuer":"sysdepo1"},"mem...
#  useraccount2 <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"useraccount2","asset":{"asset":"100000.0000 DKRW","issuer":"sysdepo1"},"mem...

$YOSEMITE_CLEOS push action yx.ntoken nissue '["useraccount3",{"asset":"100000.0000 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active

// unable to issue to blacklisted account
//$YOSEMITE_CLEOS push action yx.ntoken nissue '["useraccount1",{"asset":"100000.0000 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active


$YOSEMITE_CLEOS push action yx.ntoken transfer '[ "useraccount2", "useraccount3", "10000.0000 DKRW", "memo" ]' -p useraccount2
executed transaction: d59eb7abe0d41c21da8b75c87e3b81726c6e256601d689d8901fc15f5e4df97e  136 bytes  6459 us
#     yx.ntoken <= yx.ntoken::transfer          {"from":"useraccount2","to":"useraccount3","asset":"10000.0000 DKRW","memo":"memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"useraccount2","asset":"100.0000 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"useraccount2","asset":{"asset":"100.0000 DKRW","issuer":"sysdepo1"}}
#  useraccount2 <= yx.ntoken::feetransfer       {"payer":"useraccount2","asset":{"asset":"100.0000 DKRW","issuer":"sysdepo1"}}
#      yx.txfee <= yx.ntoken::feetransfer       {"payer":"useraccount2","asset":{"asset":"100.0000 DKRW","issuer":"sysdepo1"}}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","asset":{"asset":"10000.0000 DKRW","issuer":"sysdepo1"},"...
#  useraccount2 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","asset":{"asset":"10000.0000 DKRW","issuer":"sysdepo1"},"...
#  useraccount3 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","asset":{"asset":"10000.0000 DKRW","issuer":"sysdepo1"},"...

$YOSEMITE_CLEOS push action yx.ntoken ntransfer '[ "useraccount2", "useraccount3", {"asset":"10000.0000 DKRW","issuer":"sysdepo1"}, "memo" ]' -p useraccount2
executed transaction: 1e77d4d9ce89ee58397c558cea0d1b8eee6689e8133fefe57829dbc97c3f7589  144 bytes  3812 us
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","asset":{"asset":"10000.0000 DKRW","issuer":"sysdepo1"},"...
#  useraccount2 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","asset":{"asset":"10000.0000 DKRW","issuer":"sysdepo1"},"...
#  useraccount3 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","asset":{"asset":"10000.0000 DKRW","issuer":"sysdepo1"},"...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"useraccount2","asset":"200.0000 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"useraccount2","asset":{"asset":"200.0000 DKRW","issuer":"sysdepo1"}}
#  useraccount2 <= yx.ntoken::feetransfer       {"payer":"useraccount2","asset":{"asset":"200.0000 DKRW","issuer":"sysdepo1"}}
#      yx.txfee <= yx.ntoken::feetransfer       {"payer":"useraccount2","asset":{"asset":"200.0000 DKRW","issuer":"sysdepo1"}}

$YOSEMITE_CLEOS push action yx.ntoken wptransfer '[ "useraccount3", "useraccount2", "10000.0000 DKRW", "useraccount2", "memo" ]' -p useraccount3 -p useraccount2
#     yx.ntoken <= yx.ntoken::wptransfer        {"from":"useraccount3","to":"useraccount2","asset":"10000.0000 DKRW","payer":"useraccount2","memo":"...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"useraccount2","asset":"100.0000 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"useraccount2","asset":{"asset":"100.0000 DKRW","issuer":"sysdepo1"}}
#  useraccount2 <= yx.ntoken::feetransfer       {"payer":"useraccount2","asset":{"asset":"100.0000 DKRW","issuer":"sysdepo1"}}
#      yx.txfee <= yx.ntoken::feetransfer       {"payer":"useraccount2","asset":{"asset":"100.0000 DKRW","issuer":"sysdepo1"}}
#     yx.ntoken <= yx.ntoken::wpntransfer       "30f2d414217315d620f2d414217315d600e1f5050000000004444b5257000000000000815695b0c720f2d414217315d6046...
#  useraccount3 <= yx.ntoken::wpntransfer       "30f2d414217315d620f2d414217315d600e1f5050000000004444b5257000000000000815695b0c720f2d414217315d6046...
#  useraccount2 <= yx.ntoken::wpntransfer       "30f2d414217315d620f2d414217315d600e1f5050000000004444b5257000000000000815695b0c720f2d414217315d6046...

$YOSEMITE_CLEOS get table yx.ntoken sysdepo1 ntstats
$YOSEMITE_CLEOS get table yx.ntoken useraccount1 ntaccounts
$YOSEMITE_CLEOS get table yx.ntoken useraccount2 ntaccounts
$YOSEMITE_CLEOS get table yx.ntoken useraccount3 ntaccounts
$YOSEMITE_CLEOS get table yx.ntoken yx.txfee ntaccounts
$YOSEMITE_CLEOS get table yx.ntoken sysdepo1 ntaccounts

```

Claim Producer Rewards
---
```bash
$YOSEMITE_CLEOS push action yx.identity setidinfo "{\"account\":\"producer.c\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"bp\"}" -p idauth1@active

$YOSEMITE_CLEOS push action eosio claimrewards '["producer.c"]' -p producer.c@active
executed transaction: 96672bbf34019be7f616ac1194af1ca2e68ffb0e332503155b6075cb964893c5  104 bytes  4045 us
#         eosio <= eosio::claimrewards          {"owner":"producer.c"}
#     yx.ntoken <= yx.ntoken::transfer          {"from":"yx.txfee","to":"producer.c","asset":"472.6714 DKRW","memo":""}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"yx.txfee","to":"producer.c","asset":{"asset":"472.6714 DKRW","issuer":"sysdepo1"},"memo":""...
#      yx.txfee <= yx.ntoken::ntransfer         {"from":"yx.txfee","to":"producer.c","asset":{"asset":"472.6714 DKRW","issuer":"sysdepo1"},"memo":""...
#    producer.c <= yx.ntoken::ntransfer         {"from":"yx.txfee","to":"producer.c","asset":{"asset":"472.6714 DKRW","issuer":"sysdepo1"},"memo":""...

$YOSEMITE_CLEOS get table eosio eosio producers

$YOSEMITE_CLEOS get table yx.ntoken yx.txfee ntaccounts
$YOSEMITE_CLEOS get table yx.ntoken producer.c ntaccounts

```

Synchronize action data 
---
```bash
$YOSEMITE_CLEOS get actions yx.ntoken 0 9
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#    0   2018-07-31T04:59:06.500            eosio::setcode => eosio         b5c5537e... {"account":"yx.ntoken","vmtype":0,"vmversion":0,"code":"0061...
#    1   2018-07-31T04:59:06.500             eosio::setabi => eosio         b5c5537e... {"account":"yx.ntoken","abi":"0e656f73696f3a3a6162692f312e30...
#    2   2018-07-31T05:03:25.500         eosio::updateauth => eosio         ec8cbc9a... {"account":"yx.ntoken","permission":"owner","parent":"","aut...
#    3   2018-07-31T05:03:25.500         eosio::updateauth => eosio         81df49ce... {"account":"yx.ntoken","permission":"active","parent":"owner...
#    4   2018-07-31T05:15:03.000     yx.ntoken::setkycrule => yx.ntoken     0c8f3ad2... {"type":0,"kyc":15}...
#    5   2018-07-31T05:15:09.500     yx.ntoken::setkycrule => yx.ntoken     fc194369... {"type":1,"kyc":15}...
#    6   2018-07-31T05:29:35.000         yx.ntoken::nissue => yx.ntoken     e2dc0929... {"to":"com","asset":{"asset":"5000000.0000 DKRW","issuer":"s...
#    7   2018-07-31T05:29:35.000      yx.ntoken::ntransfer => yx.ntoken     e2dc0929... {"from":"sysdepo1","to":"com","asset":{"asset":"5000000.0000...
#    8   2018-07-31T05:30:15.000         yx.ntoken::payfee => yx.ntoken     f1021725... {"payer":"com","asset":"1000.0000 DKRW"}...
#    9   2018-07-31T05:30:15.000    yx.ntoken::feetransfer => yx.ntoken     f1021725... {"payer":"com","asset":{"asset":"1000.0000 DKRW","issuer":"s...

$YOSEMITE_CLEOS get actions yx.ntoken 10 9
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#   10   2018-07-31T05:30:15.000    yx.ntoken::feetransfer => com           f1021725... {"payer":"com","asset":{"asset":"1000.0000 DKRW","issuer":"s...
#   11   2018-07-31T05:30:15.000    yx.ntoken::feetransfer => yx.txfee      f1021725... {"payer":"com","asset":{"asset":"1000.0000 DKRW","issuer":"s...
#   12   2018-07-31T05:47:10.500         yx.ntoken::payfee => yx.ntoken     467068a5... {"payer":"com","asset":"1000.0000 DKRW"}...
#   13   2018-07-31T05:47:10.500    yx.ntoken::feetransfer => yx.ntoken     467068a5... {"payer":"com","asset":{"asset":"1000.0000 DKRW","issuer":"s...
#   14   2018-07-31T05:47:10.500    yx.ntoken::feetransfer => com           467068a5... {"payer":"com","asset":{"asset":"1000.0000 DKRW","issuer":"s...
#   15   2018-07-31T05:47:10.500    yx.ntoken::feetransfer => yx.txfee      467068a5... {"payer":"com","asset":{"asset":"1000.0000 DKRW","issuer":"s...
#   16   2018-07-31T06:00:43.000            eosio::setcode => eosio         a4ac8fe4... {"account":"yx.ntoken","vmtype":0,"vmversion":0,"code":"0061...
#   17   2018-07-31T06:00:43.000             eosio::setabi => eosio         a4ac8fe4... {"account":"yx.ntoken","abi":"0e656f73696f3a3a6162692f312e30...
#   18   2018-07-31T06:12:34.000         yx.ntoken::nissue => yx.ntoken     9e4b6f78... {"to":"useraccount2","asset":{"asset":"100000.0000 DKRW","is...
#   19   2018-07-31T06:12:34.000      yx.ntoken::ntransfer => yx.ntoken     9e4b6f78... {"from":"sysdepo1","to":"useraccount2","asset":{"asset":"100...

$YOSEMITE_CLEOS get actions yx.ntoken -1 -1
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#   40   2018-07-31T06:27:40.500      yx.ntoken::ntransfer => yx.ntoken     96672bbf... {"from":"yx.txfee","to":"producer.c","asset":{"asset":"472.6...
```

Get transaction data
---

```bash
$YOSEMITE_CLEOS get transaction 9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939
{
  "id": "9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939",
  "trx": {
    "receipt": {
      "status": "executed",
      "cpu_usage_us": 2655,
      "net_usage_words": 17,
      "trx": [
        1,{
          "signatures": [
            "SIG_K1_K6ucQu58RuWDBDu8k2Bx1Lqg9qs665bqtjDoQ3ArVPmYi3ueNUCUcXvHDg8NVGSqq2eaEEacSjAc7n3cNvsZrtMP7Tvc3P"
          ],
          "compression": "none",
          "packed_context_free_data": "",
          "packed_trx": "effd5f5b5023c5e8968f00000000010000980ad23c41f700000000288db19b01000000815695b0c700000000a8ed32322c20f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e6973737565207465737400"
        }
      ]
    },
    "trx": {
      "expiration": "2018-07-31T06:13:03",
      "ref_block_num": 9040,
      "ref_block_prefix": 2409031877,
      "max_net_usage_words": 0,
      "max_cpu_usage_ms": 0,
      "delay_sec": 0,
      "context_free_actions": [],
      "actions": [{
          "account": "yx.ntoken",
          "name": "nissue",
          "authorization": [{
              "actor": "sysdepo1",
              "permission": "active"
            }
          ],
          "data": {
            "to": "useraccount2",
            "asset": {
              "asset": "100000.0000 DKRW",
              "issuer": "sysdepo1"
            },
            "memo": "nissue test"
          },
          "hex_data": "20f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
        }
      ],
      "transaction_extensions": [],
      "signatures": [
        "SIG_K1_K6ucQu58RuWDBDu8k2Bx1Lqg9qs665bqtjDoQ3ArVPmYi3ueNUCUcXvHDg8NVGSqq2eaEEacSjAc7n3cNvsZrtMP7Tvc3P"
      ],
      "context_free_data": []
    }
  },
  "block_time": "2018-07-31T06:12:34.000",
  "block_num": 9109,
  "last_irreversible_block": 14232,
  "traces": [{
      "receipt": {
        "receiver": "yx.ntoken",
        "act_digest": "ce13bb5b5ed2505fd02a6eef6eecdf88420b6033a44bcd9b00c0044273b70ad3",
        "global_sequence": 9229,
        "recv_sequence": 9,
        "auth_sequence": [[
            "sysdepo1",
            6
          ]
        ],
        "code_sequence": 2,
        "abi_sequence": 2
      },
      "act": {
        "account": "yx.ntoken",
        "name": "nissue",
        "authorization": [{
            "actor": "sysdepo1",
            "permission": "active"
          }
        ],
        "data": {
          "to": "useraccount2",
          "asset": {
            "asset": "100000.0000 DKRW",
            "issuer": "sysdepo1"
          },
          "memo": "nissue test"
        },
        "hex_data": "20f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
      },
      "elapsed": 848,
      "cpu_usage": 0,
      "console": "",
      "total_cpu_usage": 0,
      "trx_id": "9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939",
      "inline_traces": [{
          "receipt": {
            "receiver": "yx.ntoken",
            "act_digest": "fc2d6d63ce979b64dbbc21403447d96695a43f0dfd408570baedb138d40e9abc",
            "global_sequence": 9230,
            "recv_sequence": 10,
            "auth_sequence": [[
                "eosio",
                9175
              ],[
                "sysdepo1",
                7
              ]
            ],
            "code_sequence": 2,
            "abi_sequence": 2
          },
          "act": {
            "account": "yx.ntoken",
            "name": "ntransfer",
            "authorization": [{
                "actor": "sysdepo1",
                "permission": "active"
              },{
                "actor": "eosio",
                "permission": "active"
              }
            ],
            "data": {
              "from": "sysdepo1",
              "to": "useraccount2",
              "asset": {
                "asset": "100000.0000 DKRW",
                "issuer": "sysdepo1"
              },
              "memo": "nissue test"
            },
            "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
          },
          "elapsed": 1656,
          "cpu_usage": 0,
          "console": "",
          "total_cpu_usage": 0,
          "trx_id": "9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939",
          "inline_traces": [{
              "receipt": {
                "receiver": "sysdepo1",
                "act_digest": "fc2d6d63ce979b64dbbc21403447d96695a43f0dfd408570baedb138d40e9abc",
                "global_sequence": 9231,
                "recv_sequence": 2,
                "auth_sequence": [[
                    "eosio",
                    9176
                  ],[
                    "sysdepo1",
                    8
                  ]
                ],
                "code_sequence": 2,
                "abi_sequence": 2
              },
              "act": {
                "account": "yx.ntoken",
                "name": "ntransfer",
                "authorization": [{
                    "actor": "sysdepo1",
                    "permission": "active"
                  },{
                    "actor": "eosio",
                    "permission": "active"
                  }
                ],
                "data": {
                  "from": "sysdepo1",
                  "to": "useraccount2",
                  "asset": {
                    "asset": "100000.0000 DKRW",
                    "issuer": "sysdepo1"
                  },
                  "memo": "nissue test"
                },
                "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
              },
              "elapsed": 2,
              "cpu_usage": 0,
              "console": "",
              "total_cpu_usage": 0,
              "trx_id": "9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939",
              "inline_traces": []
            },{
              "receipt": {
                "receiver": "useraccount2",
                "act_digest": "fc2d6d63ce979b64dbbc21403447d96695a43f0dfd408570baedb138d40e9abc",
                "global_sequence": 9232,
                "recv_sequence": 1,
                "auth_sequence": [[
                    "eosio",
                    9177
                  ],[
                    "sysdepo1",
                    9
                  ]
                ],
                "code_sequence": 2,
                "abi_sequence": 2
              },
              "act": {
                "account": "yx.ntoken",
                "name": "ntransfer",
                "authorization": [{
                    "actor": "sysdepo1",
                    "permission": "active"
                  },{
                    "actor": "eosio",
                    "permission": "active"
                  }
                ],
                "data": {
                  "from": "sysdepo1",
                  "to": "useraccount2",
                  "asset": {
                    "asset": "100000.0000 DKRW",
                    "issuer": "sysdepo1"
                  },
                  "memo": "nissue test"
                },
                "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
              },
              "elapsed": 4,
              "cpu_usage": 0,
              "console": "",
              "total_cpu_usage": 0,
              "trx_id": "9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939",
              "inline_traces": []
            }
          ]
        }
      ]
    },{
      "receipt": {
        "receiver": "yx.ntoken",
        "act_digest": "fc2d6d63ce979b64dbbc21403447d96695a43f0dfd408570baedb138d40e9abc",
        "global_sequence": 9230,
        "recv_sequence": 10,
        "auth_sequence": [[
            "eosio",
            9175
          ],[
            "sysdepo1",
            7
          ]
        ],
        "code_sequence": 2,
        "abi_sequence": 2
      },
      "act": {
        "account": "yx.ntoken",
        "name": "ntransfer",
        "authorization": [{
            "actor": "sysdepo1",
            "permission": "active"
          },{
            "actor": "eosio",
            "permission": "active"
          }
        ],
        "data": {
          "from": "sysdepo1",
          "to": "useraccount2",
          "asset": {
            "asset": "100000.0000 DKRW",
            "issuer": "sysdepo1"
          },
          "memo": "nissue test"
        },
        "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
      },
      "elapsed": 1656,
      "cpu_usage": 0,
      "console": "",
      "total_cpu_usage": 0,
      "trx_id": "9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939",
      "inline_traces": [{
          "receipt": {
            "receiver": "sysdepo1",
            "act_digest": "fc2d6d63ce979b64dbbc21403447d96695a43f0dfd408570baedb138d40e9abc",
            "global_sequence": 9231,
            "recv_sequence": 2,
            "auth_sequence": [[
                "eosio",
                9176
              ],[
                "sysdepo1",
                8
              ]
            ],
            "code_sequence": 2,
            "abi_sequence": 2
          },
          "act": {
            "account": "yx.ntoken",
            "name": "ntransfer",
            "authorization": [{
                "actor": "sysdepo1",
                "permission": "active"
              },{
                "actor": "eosio",
                "permission": "active"
              }
            ],
            "data": {
              "from": "sysdepo1",
              "to": "useraccount2",
              "asset": {
                "asset": "100000.0000 DKRW",
                "issuer": "sysdepo1"
              },
              "memo": "nissue test"
            },
            "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
          },
          "elapsed": 2,
          "cpu_usage": 0,
          "console": "",
          "total_cpu_usage": 0,
          "trx_id": "9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939",
          "inline_traces": []
        },{
          "receipt": {
            "receiver": "useraccount2",
            "act_digest": "fc2d6d63ce979b64dbbc21403447d96695a43f0dfd408570baedb138d40e9abc",
            "global_sequence": 9232,
            "recv_sequence": 1,
            "auth_sequence": [[
                "eosio",
                9177
              ],[
                "sysdepo1",
                9
              ]
            ],
            "code_sequence": 2,
            "abi_sequence": 2
          },
          "act": {
            "account": "yx.ntoken",
            "name": "ntransfer",
            "authorization": [{
                "actor": "sysdepo1",
                "permission": "active"
              },{
                "actor": "eosio",
                "permission": "active"
              }
            ],
            "data": {
              "from": "sysdepo1",
              "to": "useraccount2",
              "asset": {
                "asset": "100000.0000 DKRW",
                "issuer": "sysdepo1"
              },
              "memo": "nissue test"
            },
            "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
          },
          "elapsed": 4,
          "cpu_usage": 0,
          "console": "",
          "total_cpu_usage": 0,
          "trx_id": "9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939",
          "inline_traces": []
        }
      ]
    },{
      "receipt": {
        "receiver": "sysdepo1",
        "act_digest": "fc2d6d63ce979b64dbbc21403447d96695a43f0dfd408570baedb138d40e9abc",
        "global_sequence": 9231,
        "recv_sequence": 2,
        "auth_sequence": [[
            "eosio",
            9176
          ],[
            "sysdepo1",
            8
          ]
        ],
        "code_sequence": 2,
        "abi_sequence": 2
      },
      "act": {
        "account": "yx.ntoken",
        "name": "ntransfer",
        "authorization": [{
            "actor": "sysdepo1",
            "permission": "active"
          },{
            "actor": "eosio",
            "permission": "active"
          }
        ],
        "data": {
          "from": "sysdepo1",
          "to": "useraccount2",
          "asset": {
            "asset": "100000.0000 DKRW",
            "issuer": "sysdepo1"
          },
          "memo": "nissue test"
        },
        "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
      },
      "elapsed": 2,
      "cpu_usage": 0,
      "console": "",
      "total_cpu_usage": 0,
      "trx_id": "9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939",
      "inline_traces": []
    },{
      "receipt": {
        "receiver": "useraccount2",
        "act_digest": "fc2d6d63ce979b64dbbc21403447d96695a43f0dfd408570baedb138d40e9abc",
        "global_sequence": 9232,
        "recv_sequence": 1,
        "auth_sequence": [[
            "eosio",
            9177
          ],[
            "sysdepo1",
            9
          ]
        ],
        "code_sequence": 2,
        "abi_sequence": 2
      },
      "act": {
        "account": "yx.ntoken",
        "name": "ntransfer",
        "authorization": [{
            "actor": "sysdepo1",
            "permission": "active"
          },{
            "actor": "eosio",
            "permission": "active"
          }
        ],
        "data": {
          "from": "sysdepo1",
          "to": "useraccount2",
          "asset": {
            "asset": "100000.0000 DKRW",
            "issuer": "sysdepo1"
          },
          "memo": "nissue test"
        },
        "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
      },
      "elapsed": 4,
      "cpu_usage": 0,
      "console": "",
      "total_cpu_usage": 0,
      "trx_id": "9e4b6f7880f8c5e2bf5e34934e191279d9f86e5b732ee26f1f1c97f8d7a42939",
      "inline_traces": []
    }
  ]
}
```