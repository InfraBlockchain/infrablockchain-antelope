YOSEMITE Blockchain (Permissioned version) BIOS setup for Testnet 
===
(revision 20180817 for Testnet v0.4.0)

Testnet Public Access Node URL
---
* [http://testnet.yosemitelabs.org:8888](http://testnet.yosemitelabs.org:8888)

```bash
YOSEMITE_CLI_TESTNET="$YOSEMITE_HOME/build/programs/clyos/clyos -u http://testnet.yosemitelabs.org:8888"
$YOSEMITE_CLI_TESTNET get info
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
YOSEMITE_NODE_BIN_NAME=yosemite
YOSEMITE_CLI_BIN_NAME=clyos
YOSEMITE_KEYD_BIN_NAME=keyos
YOSEMITE_HOME=/mnt/yosemite-public-blockchain-git
YOSEMITE_NODE=$YOSEMITE_HOME/build/programs/$YOSEMITE_NODE_BIN_NAME/$YOSEMITE_NODE_BIN_NAME
YOSEMITE_NODE_LOG_FILE=/mnt/$YOSEMITE_NODE_BIN_NAME.log
YOSEMITE_CLI=$YOSEMITE_HOME/build/programs/$YOSEMITE_CLI_BIN_NAME/$YOSEMITE_CLI_BIN_NAME
YOSEMITE_CLI_TESTNET="$YOSEMITE_CLI -u http://testnet.yosemitelabs.org:8888"
YOSEMITE_KEYD=$YOSEMITE_HOME/build/programs/$YOSEMITE_KEYD_BIN_NAME/$YOSEMITE_KEYD_BIN_NAME
YOSEMITE_KEYD_LOG_FILE=/mnt/$YOSEMITE_KEYD_BIN_NAME.log
YOSEMITE_KEYD_WALLET_PASSWORD=PW5KH7i8ZEuVMvywMschs3TznhTfCdmgpPBGKJLUQjs6N6oQ7boZj
YOSEMITE_NODE_CONFIG=$YOSEMITE_HOME/yosemite_config/config_yosemite_testnet_boot.ini
YOSEMITE_NODE_GENESIS_JSON=$YOSEMITE_HOME/yosemite_config/genesis_yosemite_testnet.json
YOSEMITE_NODE_DATA_DIR=/mnt/yosemite_node_data
YOSEMITE_DEV_WALLET_DIR=/mnt/yosemite_dev_wallet
YOSEMITE_CONTRACTS_DIR=$YOSEMITE_HOME/build/contracts
YOSEMITE_MONGOD=/home/ubuntu/opt/mongodb/bin/mongod
YOSEMITE_MONGODB_CONFIG=/home/ubuntu/opt/mongodb/mongod.conf
YOSEMITE_MONGODB_DATA_DIR=/mnt/mongodb
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
producer-name = yosemite
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
$YOSEMITE_NODE --config $YOSEMITE_NODE_CONFIG --genesis-json $YOSEMITE_NODE_GENESIS_JSON --data-dir $YOSEMITE_NODE_DATA_DIR
```

* from existing node data
```bash
$YOSEMITE_NODE --print-genesis-json --config $YOSEMITE_NODE_CONFIG --data-dir $YOSEMITE_NODE_DATA_DIR
nohup $YOSEMITE_NODE --config $YOSEMITE_NODE_CONFIG --data-dir $YOSEMITE_NODE_DATA_DIR > /mnt/yosemite.log 2>&1&
tail -f /mnt/yosemite.log -n 300
pgrep yosemite
pkill -SIGINT yosemite
```

* replay blockchain from tx log
```bash
$YOSEMITE_NODE --config $YOSEMITE_NODE_CONFIG --data-dir $YOSEMITE_NODE_DATA_DIR --replay-blockchain
```

Start Wallet (keyos)
---

* run key daemon
```bash
nohup $YOSEMITE_KEYD --unlock-timeout 999999999 --http-server-address 127.0.0.1:8900 --wallet-dir $YOSEMITE_DEV_WALLET_DIR > /mnt/keyos.log 2>&1&
tail -f /mnt/keyos.log -n 300
pgrep keyos
pkill -SIGINT keyos
```

* create wallet / unlock
```bash
$YOSEMITE_CLI wallet create
Creating wallet: default
"PW5KH7i8ZEuVMvywMschs3TznhTfCdmgpPBGKJLUQjs6N6oQ7boZj"

$YOSEMITE_CLI wallet open
$YOSEMITE_CLI wallet unlock
```

* yosemite initial key
```bash
$YOSEMITE_CLI wallet import --private-key 5KR14orrckPKBxUe4zUZCoY8GF8xhYUtKjiriSnsTGhUKrZTHxo
```

Create System Accounts
---

* yx.msig(privileged), yx.txfee, yx.identity, yx.ntoken(privileged), yx.token(privileged), yx.dcontract(privileged)
```bash
$YOSEMITE_CLI create account yosemite yx.msig EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLI create account yosemite yx.txfee EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLI create account yosemite yx.identity EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLI create account yosemite yx.ntoken EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLI create account yosemite yx.token EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9
$YOSEMITE_CLI create account yosemite yx.dcontract EOS6HrSCEbKTgZLe8stDgFB3Pip2tKtBxTPuffuoynnZnfUxHS3x9

```

Create initial Identity Authority Account
---

* idauth1
```bash
$YOSEMITE_CLI create key
Private key: 5KND1U57MvsdWzPi8TY53XpaBPBGQNTdQSTDtEexTVexz9mP6Q9
Public key: EOS5gFJS9EhjNpvcaGuDQqaeGQnNLuF9sWUt2s8surF1tRtSzLJiG
$YOSEMITE_CLI wallet import --private-key 5KND1U57MvsdWzPi8TY53XpaBPBGQNTdQSTDtEexTVexz9mP6Q9

$YOSEMITE_CLI create account yosemite idauth1 EOS5gFJS9EhjNpvcaGuDQqaeGQnNLuF9sWUt2s8surF1tRtSzLJiG

```

* idauth2
```bash
$YOSEMITE_CLI create key
Private key: 5K25HU8a7qgkfb9o2skiNupEs9H3wE6CWSd9qjako2gnFioekaX
Public key: EOS68JZEkVt4Z3dkrSmQ4SkH8XyVJVKTmnvawZyWcrxzhoH1WMp4Q
$YOSEMITE_CLI wallet import --private-key 5K25HU8a7qgkfb9o2skiNupEs9H3wE6CWSd9qjako2gnFioekaX

$YOSEMITE_CLI create account yosemite idauth2 EOS68JZEkVt4Z3dkrSmQ4SkH8XyVJVKTmnvawZyWcrxzhoH1WMp4Q
```


Create initial System Depository Account
---

* sysdepo1
```bash
$YOSEMITE_CLI create key
Private key: 5KCqedPhHjbaBRi4ujhFsS3aGCcNuXwVGrMaxRW3DvB5Wo3beax
Public key: EOS7myk8qduMNnxo9Q9RZ2uxfaerQhXMPtxKUhxKh5HfSSM1dHeQo
$YOSEMITE_CLI wallet import --private-key 5KCqedPhHjbaBRi4ujhFsS3aGCcNuXwVGrMaxRW3DvB5Wo3beax

$YOSEMITE_CLI create account yosemite sysdepo1 EOS7myk8qduMNnxo9Q9RZ2uxfaerQhXMPtxKUhxKh5HfSSM1dHeQo

```

* sysdepo2
```bash
$YOSEMITE_CLI create key
Private key: 5HyRsRWBi4bUXjmnmaskw9BU8a2sdqn3Jfqt21Few9xUyYR5tXr
Public key: EOS62AXYcUxcSQQgKhnQ1oid2w1XNvCKxmPgk7C8fEi4FnQKhHLwM
$YOSEMITE_CLI wallet import --private-key 5HyRsRWBi4bUXjmnmaskw9BU8a2sdqn3Jfqt21Few9xUyYR5tXr

$YOSEMITE_CLI create account yosemite sysdepo2 EOS62AXYcUxcSQQgKhnQ1oid2w1XNvCKxmPgk7C8fEi4FnQKhHLwM
```

Install Yosemite System Contract
---
```bash
$YOSEMITE_CLI set contract yosemite $YOSEMITE_CONTRACTS_DIR/yx.system/ -p yosemite@active
```

Install Multi-sig System Contract
---
```bash
$YOSEMITE_CLI set contract yx.msig $YOSEMITE_CONTRACTS_DIR/eosio.msig/ -p yosemite.msig@active
$YOSEMITE_CLI push action yosemite setpriv '["yx.msig",1]' -p yosemite@active
```

Install Transaction Fee System Contract
---
```bash
$YOSEMITE_CLI set contract yx.txfee $YOSEMITE_CONTRACTS_DIR/yx.txfee/ -p yx.txfee@active
```

Install Identity System Contract
---
```bash
$YOSEMITE_CLI set contract yx.identity $YOSEMITE_CONTRACTS_DIR/yx.identity/ -p yx.identity@active
```

Register Initial Identity Authority
---
```bash
$YOSEMITE_CLI push action yosemite regidauth '["idauth1","http://idauth1.org",1]' -p idauth1@active -p yosemite@active
$YOSEMITE_CLI push action yosemite authidauth '["idauth1"]' -p yosemite@active

$YOSEMITE_CLI push action yosemite regidauth '["idauth2","http://idauth2.org",1]' -p idauth2@active -p yosemite@active
$YOSEMITE_CLI push action yosemite authidauth '["idauth2"]' -p yosemite@active

//$YOSEMITE_CLI push action yosemite rmvidauth '["idauth2"]' -p yosemite@active

$YOSEMITE_CLI get table yosemite yosemite idauthority
```

Register Initial System Depository
---
```bash
$YOSEMITE_CLI push action yosemite regsysdepo '["sysdepo1","http://sysdepo1.org",1]' -p sysdepo1@active -p yosemite@active
$YOSEMITE_CLI push action yosemite authsysdepo '["sysdepo1"]' -p yosemite@active

$YOSEMITE_CLI push action yosemite regsysdepo '["sysdepo2","http://sysdepo2.org",1]' -p sysdepo2@active -p yosemite@active
$YOSEMITE_CLI push action yosemite authsysdepo '["sysdepo2"]' -p yosemite@active

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"sysdepo1\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"sysdepo\"}" -p idauth1@active
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"sysdepo2\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"sysdepo\"}" -p idauth1@active
$YOSEMITE_CLI get table -L sysdepo1 -l 1 yx.identity yx.identity identity
$YOSEMITE_CLI get table -L sysdepo2 -l 1 yx.identity yx.identity identity

$YOSEMITE_CLI get table yosemite yosemite sysdepos
```

Install Native Token Contract
---
```bash
$YOSEMITE_CLI set contract yx.ntoken $YOSEMITE_CONTRACTS_DIR/yx.ntoken/
$YOSEMITE_CLI push action yosemite setpriv '["yx.ntoken",1]' -p yosemite@active

$YOSEMITE_CLI push action yx.ntoken setkycrule "{\"type\":0, \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p yosemite@active
$YOSEMITE_CLI push action yx.ntoken setkycrule "{\"type\":1, \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p yosemite@active

```

Install User Token Contract
---
```bash
$YOSEMITE_CLI set contract yx.token $YOSEMITE_CONTRACTS_DIR/yx.token/
$YOSEMITE_CLI push action yosemite setpriv '["yx.token",1]' -p yosemite@active
```

Install DContract Contract
---
```bash
$YOSEMITE_CLI set contract yx.dcontract $YOSEMITE_CONTRACTS_DIR/yx.dcontract/
$YOSEMITE_CLI push action yosemite setpriv '["yx.dcontract",1]' -p yosemite@active
```

Querying the status of System Depositories, Identity Authorities and Block Producers
---
```bash
$YOSEMITE_CLI get table yosemite yosemite global
$YOSEMITE_CLI get table yosemite yosemite sysdepos
$YOSEMITE_CLI get table yosemite yosemite idauthority
$YOSEMITE_CLI get table yosemite yosemite producers
```

Initial Block Producer Setup
---

* config_yosemite_testnet_boot.ini (one node multi producer setup)
```ini
producer-name = yosemite
producer-name = producer.a
producer-name = producer.b
producer-name = producer.c
producer-name = producer.d
producer-name = producer.e
producer-name = producer.f
producer-name = producer.g
producer-name = producer.h
producer-name = producer.i
producer-name = producer.j

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
# producer.f
signature-provider = EOS6mUfvfWUJFVyQi23YwNtXMUHfSuExkr7WwWYr4RVzVD5TyASYh=KEY:5KQmxBJPcM6dtgkPCnbZxDxR7fYufyAXmQpowtccDosSHCMnvzD
# producer.g
signature-provider = EOS4xWpwU957GRJzXBjRRFWKegp772Z636xDXMyNopT4eR7pAq7cn=KEY:5J3stfy2eL19D99A9BH6rWdNyBJN52KGBLgGzPcd2i1H2E2c8RE
# producer.h
signature-provider = EOS8JjyHbTYw2uzqC14h48HwHKZFrBjJUfcKDo4hbPPui1ftBRktx=KEY:5JWtg8FYV8vsTLtVKyFKFDDWgHnxC8K8HrJBqPhp7EAT9NAC8CR
# producer.i
signature-provider = EOS8mRvKmUuiSVwxvL9R4wg1z9Tkjs7STuaNZZAivUxkWtFUxBW4H=KEY:5J5L2x6E23sMykwirFJMHpWrHn9oypowdFcTs5ThFFNCBRpSZCJ
# producer.j
signature-provider = EOS7wwywnEFqM2fjWTvTuF5E8ELu9hpbWke3aCUYqEkfw9aP1G2ph=KEY:5Jj8nMEZiL4fZSxDf8xMHmVs94JrPiwwH66EE3b3pM9YpUyQdAv
```

* create block producer accounts
```bash
$YOSEMITE_CLI create key
Private key: 5JuTjNtFmqWG1ivMLJgeRgo5w2WV5iwHa2u1n8owvpyUdy7DUr6
Public key: EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y
$YOSEMITE_CLI wallet import --private-key 5JuTjNtFmqWG1ivMLJgeRgo5w2WV5iwHa2u1n8owvpyUdy7DUr6

$YOSEMITE_CLI create key
Private key: 5JbXQdi7vzRXtxnyBi9PSTzaxACJagDKUFQkLE7Dg9ZYMyykJR8
Public key: EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS
$YOSEMITE_CLI wallet import --private-key 5JbXQdi7vzRXtxnyBi9PSTzaxACJagDKUFQkLE7Dg9ZYMyykJR8

$YOSEMITE_CLI create key
Private key: 5JEJrZMbDSZmFfRUbt1JVC3wrBv9cJG7QaiP58f8Mr9K1Wf9Cnn
Public key: EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe
$YOSEMITE_CLI wallet import --private-key 5JEJrZMbDSZmFfRUbt1JVC3wrBv9cJG7QaiP58f8Mr9K1Wf9Cnn

$YOSEMITE_CLI create key
Private key: 5HsDW4pNKWDA91WcBuNNXT8sL5soVhXLHqZU7ZZqJzJC1YghXjT
Public key: EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ
$YOSEMITE_CLI wallet import --private-key 5HsDW4pNKWDA91WcBuNNXT8sL5soVhXLHqZU7ZZqJzJC1YghXjT

$YOSEMITE_CLI create key
Private key: 5JFbKCZray6WGQZtkkxqQ9mencwQn5qynjJ1tXJRtaUH7oNRGnP
Public key: EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW
$YOSEMITE_CLI wallet import --private-key 5JFbKCZray6WGQZtkkxqQ9mencwQn5qynjJ1tXJRtaUH7oNRGnP
 
$YOSEMITE_CLI create key
Private key: 5KQmxBJPcM6dtgkPCnbZxDxR7fYufyAXmQpowtccDosSHCMnvzD
Public key: EOS6mUfvfWUJFVyQi23YwNtXMUHfSuExkr7WwWYr4RVzVD5TyASYh
$YOSEMITE_CLI wallet import --private-key 5KQmxBJPcM6dtgkPCnbZxDxR7fYufyAXmQpowtccDosSHCMnvzD

$YOSEMITE_CLI create key
Private key: 5J3stfy2eL19D99A9BH6rWdNyBJN52KGBLgGzPcd2i1H2E2c8RE
Public key: EOS4xWpwU957GRJzXBjRRFWKegp772Z636xDXMyNopT4eR7pAq7cn
$YOSEMITE_CLI wallet import --private-key 5J3stfy2eL19D99A9BH6rWdNyBJN52KGBLgGzPcd2i1H2E2c8RE

$YOSEMITE_CLI create key
Private key: 5JWtg8FYV8vsTLtVKyFKFDDWgHnxC8K8HrJBqPhp7EAT9NAC8CR
Public key: EOS8JjyHbTYw2uzqC14h48HwHKZFrBjJUfcKDo4hbPPui1ftBRktx
$YOSEMITE_CLI wallet import --private-key 5JWtg8FYV8vsTLtVKyFKFDDWgHnxC8K8HrJBqPhp7EAT9NAC8CR

$YOSEMITE_CLI create key
Private key: 5J5L2x6E23sMykwirFJMHpWrHn9oypowdFcTs5ThFFNCBRpSZCJ
Public key: EOS8mRvKmUuiSVwxvL9R4wg1z9Tkjs7STuaNZZAivUxkWtFUxBW4H
$YOSEMITE_CLI wallet import --private-key 5J5L2x6E23sMykwirFJMHpWrHn9oypowdFcTs5ThFFNCBRpSZCJ

$YOSEMITE_CLI create key
Private key: 5Jj8nMEZiL4fZSxDf8xMHmVs94JrPiwwH66EE3b3pM9YpUyQdAv
Public key: EOS7wwywnEFqM2fjWTvTuF5E8ELu9hpbWke3aCUYqEkfw9aP1G2ph
$YOSEMITE_CLI wallet import --private-key 5Jj8nMEZiL4fZSxDf8xMHmVs94JrPiwwH66EE3b3pM9YpUyQdAv

$YOSEMITE_CLI create account yosemite producer.a EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.b EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.c EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.d EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.e EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.f EOS6mUfvfWUJFVyQi23YwNtXMUHfSuExkr7WwWYr4RVzVD5TyASYh -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.g EOS4xWpwU957GRJzXBjRRFWKegp772Z636xDXMyNopT4eR7pAq7cn -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.h EOS8JjyHbTYw2uzqC14h48HwHKZFrBjJUfcKDo4hbPPui1ftBRktx -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.i EOS8mRvKmUuiSVwxvL9R4wg1z9Tkjs7STuaNZZAivUxkWtFUxBW4H -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.j EOS7wwywnEFqM2fjWTvTuF5E8ELu9hpbWke3aCUYqEkfw9aP1G2ph -p yosemite@active

```

* register and authorize block producers (permissioned)

```bash
$YOSEMITE_CLI push action yosemite regproducer '["producer.a","EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y","http://producera.io",1]' -p producer.a@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.b","EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS","http://producerb.io",1]' -p producer.b@active -p yosemite@active

$YOSEMITE_CLI push action yosemite authproducer '["producer.a"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.b"]' -p yosemite@active

$YOSEMITE_CLI push action yosemite regproducer '["producer.c","EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe","http://producerc.io",1]' -p producer.c@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.d","EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ","http://producerd.io",1]' -p producer.d@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.e","EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW","http://producere.io",1]' -p producer.e@active -p yosemite@active

$YOSEMITE_CLI push action yosemite authproducer '["producer.c"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.d"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.e"]' -p yosemite@active

$YOSEMITE_CLI push action yosemite regproducer '["producer.f","EOS6mUfvfWUJFVyQi23YwNtXMUHfSuExkr7WwWYr4RVzVD5TyASYh","http://producerf.io",1]' -p producer.f@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.g","EOS4xWpwU957GRJzXBjRRFWKegp772Z636xDXMyNopT4eR7pAq7cn","http://producerg.io",1]' -p producer.g@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.h","EOS8JjyHbTYw2uzqC14h48HwHKZFrBjJUfcKDo4hbPPui1ftBRktx","http://producerh.io",1]' -p producer.h@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.i","EOS8mRvKmUuiSVwxvL9R4wg1z9Tkjs7STuaNZZAivUxkWtFUxBW4H","http://produceri.io",1]' -p producer.i@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.j","EOS7wwywnEFqM2fjWTvTuF5E8ELu9hpbWke3aCUYqEkfw9aP1G2ph","http://producerj.io",1]' -p producer.j@active -p yosemite@active

$YOSEMITE_CLI push action yosemite authproducer '["producer.f"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.g"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.h"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.i"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.j"]' -p yosemite@active
```

Setup Initial Transaction Fees
---
```bash
// yx.system
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.newacc", "1000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.regprod", "3000000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.regsysdep", "2000000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.regidauth", "2000000.0000 DKRW" ]' -p yosemite@active

// yx.ntoken
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.nissue", "0.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.nredeem", "1000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.transfer", "100.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.ntransfer", "200.0000 DKRW" ]' -p yosemite@active

// yx.token
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.tcreate", "10000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.tissue", "500.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.tredeem", "500.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.ttransfer", "100.0000 DKRW" ]' -p yosemite@active

// yx.dcontract
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.dccreate", "500.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.dcaddsign", "100.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.dcsign", "300.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.dcupadd", "50.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.dcremove", "0.0000 DKRW" ]' -p yosemite@active

$YOSEMITE_CLI get table -l 100 yx.txfee yx.txfee txfees
$YOSEMITE_CLI get table -L tf.transfer -l 1 yx.txfee yx.txfee txfees
```

Resign "yosemite" delegating authority to "yx.prods"
---
```bash
$YOSEMITE_CLI get account yosemite
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yosemite","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yx.prods","permission":"active"}}]}}' -p yosemite@owner
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yosemite","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yx.prods","permission":"active"}}]}}' -p yosemite@active
$YOSEMITE_CLI get account yosemite

$YOSEMITE_CLI get account yx.prods
permissions: 
     owner     1:    
        active     4:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
           prod.major     3:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
              prod.minor     2:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
```

Resign "yx.msig" delegating authority to "yosemite"
---
```bash
$YOSEMITE_CLI get account yx.msig
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.msig","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yosemite.msig@owner
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.msig","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yosemite.msig@active
$YOSEMITE_CLI get account yx.msig
```

Resign "yx.txfee", "yx.identity", "yx.ntoken", "yx.token", "yx.dcontract" delegating authority to "yosemite"
---
```bash
$YOSEMITE_CLI get account yx.txfee
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.txfee","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.txfee@owner
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.txfee","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.txfee@active
$YOSEMITE_CLI get account yx.txfee

$YOSEMITE_CLI get account yx.identity
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.identity","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.identity@owner
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.identity","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.identity@active
$YOSEMITE_CLI get account yx.identity

$YOSEMITE_CLI get account yx.ntoken
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.ntoken","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.ntoken@owner
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.ntoken","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.ntoken@active
$YOSEMITE_CLI get account yx.ntoken

$YOSEMITE_CLI get account yx.token
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.token","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.token@owner
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.token","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.token@active
$YOSEMITE_CLI get account yx.token

$YOSEMITE_CLI get account yx.dcontract
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.dcontract","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.dcontract@owner
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.dcontract","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.dcontract@active
$YOSEMITE_CLI get account yx.dcontract
```

Unauthorizing Block Producer
---
```bash
$YOSEMITE_CLI push action -j -d yosemite rmvproducer '["producer.e"]' -p yosemite@active
#$YOSEMITE_CLI push action yosemite rmvproducer '["producer.e"]' -p yosemite@active
#$YOSEMITE_CLI push action yosemite rmvproducer '["producer.d"]' -p yosemite@active
$YOSEMITE_CLI get account yx.prods
```

Create New User Accounts
---

```bash
$YOSEMITE_CLI create key
Private key: 5JsqBiEGge54oBRPEuPZ4rTm8EHxzDNzXMbwNoTh8y5Lms1YMJT
Public key: EOS7qw27KHaVWJEgA113u9xQVUtEubUKFhRJfiDt752iAqb6qbv9V
$YOSEMITE_CLI wallet import --private-key 5JsqBiEGge54oBRPEuPZ4rTm8EHxzDNzXMbwNoTh8y5Lms1YMJT

$YOSEMITE_CLI create key
Private key: 5JMsmWCrQ7YEDfUNhmPts7W3AuUFxeWpionyP8EuMf35kG3AeCU
Public key: EOS7hbLPy5cmtNBdHibrS8CD1YUsKbvq7t9nGVvnJtTKHvxsq6ynf
$YOSEMITE_CLI wallet import --private-key 5JMsmWCrQ7YEDfUNhmPts7W3AuUFxeWpionyP8EuMf35kG3AeCU

$YOSEMITE_CLI create key
Private key: 5K2iUzbSy4qyEzjf93DGDxBMrL8c8VGnSvLhE26DEJhEwvVEwj8
Public key: EOS75qaGrHrXskTUJLoedxwc7KbLxksfgiYBpgeiMcTCsnhiuhx6U
$YOSEMITE_CLI wallet import --private-key 5K2iUzbSy4qyEzjf93DGDxBMrL8c8VGnSvLhE26DEJhEwvVEwj8

$YOSEMITE_CLI create key
Private key: 5HrWpMMjCM1UhxKAvCg8NKRyExapwS1vyiQXe8VBpNEDsNMn4nE
Public key: EOS584bQn471vv2q7N1xeDXayfDtiim8kPsNm45TRuJ9D51YPFFmX
$YOSEMITE_CLI wallet import --private-key 5HrWpMMjCM1UhxKAvCg8NKRyExapwS1vyiQXe8VBpNEDsNMn4nE

$YOSEMITE_CLI create key
Private key: 5HsUEcT7J56iRrHFiGR9pGeQAiLaow4AbR7QA38w9T44SBnEMYk
Public key: EOS5UbAi7wTM1wpFNp81bghD9EfV1HHSn8n39Qz4jKD1oHmW7oyNS
$YOSEMITE_CLI wallet import --private-key 5HsUEcT7J56iRrHFiGR9pGeQAiLaow4AbR7QA38w9T44SBnEMYk

// Identity Authority can create accounts having 12 chracters without paying 'tf.newacc' transaction fee
$YOSEMITE_CLI create account idauth1 useraccount1 EOS7qw27KHaVWJEgA113u9xQVUtEubUKFhRJfiDt752iAqb6qbv9V -p idauth1@active
$YOSEMITE_CLI create account idauth1 useraccount2 EOS7hbLPy5cmtNBdHibrS8CD1YUsKbvq7t9nGVvnJtTKHvxsq6ynf -p idauth1@active
$YOSEMITE_CLI create account idauth1 useraccount3 EOS75qaGrHrXskTUJLoedxwc7KbLxksfgiYBpgeiMcTCsnhiuhx6U -p idauth1@active

// Only system account can create accounts having less than 12 characters
$YOSEMITE_CLI create account yosemite com EOS584bQn471vv2q7N1xeDXayfDtiim8kPsNm45TRuJ9D51YPFFmX -p yosemite@active

// Only suffix account can create accounts having arbitrary prefix with the same suffix name
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"com\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"1f32i7t23\"}" -p idauth1@active
$YOSEMITE_CLI push action yx.ntoken nissue '["com",{"amount":"5000000.0000 DKRW","issuer":"sysdepo1"},"nissue com"]' -p sysdepo1@active
executed transaction: c9fb1c0cf6c6c9d73e377bcfd405802e129f85cf86a68a57bc15f91d252e1924  136 bytes  3565 us
#     yx.ntoken <= yx.ntoken::nissue            {"to":"com","token":{"amount":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"nissue com"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","token":{"amount":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"nis...
#      sysdepo1 <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","token":{"amount":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"nis...
#           com <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","token":{"amount":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"nis...
$YOSEMITE_CLI create account com acquire.com EOS5UbAi7wTM1wpFNp81bghD9EfV1HHSn8n39Qz4jKD1oHmW7oyNS -p com@active
executed transaction: 9268f4e3b7e44caaf1a7c3528ff44ce6e9cfdb8d6dfcf33aa2cc44d7a6c5604a  200 bytes  4900 us
#      yosemite <= yosemite::newaccount         {"creator":"com","name":"acquire.com","owner":{"threshold":1,"keys":[{"key":"EOS5UbAi7wTM1wpFNp81bgh...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"com","token":"1000.0000 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"com","token":{"amount":"1000.0000 DKRW","issuer":"sysdepo1"}}
#           com <= yx.ntoken::feetransfer       {"payer":"com","token":{"amount":"1000.0000 DKRW","issuer":"sysdepo1"}}
#      yx.txfee <= yx.ntoken::feetransfer       {"payer":"com","token":{"amount":"1000.0000 DKRW","issuer":"sysdepo1"}}

$YOSEMITE_CLI get table yx.ntoken sysdepo1 ntaccounts
$YOSEMITE_CLI get table yx.ntoken sysdepo1 ntstats
$YOSEMITE_CLI get table yx.ntoken com ntaccounts
$YOSEMITE_CLI get table yx.ntoken yx.txfee ntaccounts

$YOSEMITE_CLI get table -L com -l 1 yx.identity yx.identity identity
``` 

Managing Account Identity Info (including KYC)
---
```bash
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccount1\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 0111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth1@active
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccount2\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"23uyiuye\"}" -p idauth1@active
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccount3\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"vewv23r3\"}" -p idauth1@active

$YOSEMITE_CLI get table -L useraccount1 -l 1 yx.identity yx.identity identity

// update user account type
$YOSEMITE_CLI push action yx.identity settype "{\"account\":\"useraccount1\", \"type\":$(echo 'ibase=2; 11111111' | bc)}" -p idauth1@active

// update kyc status
$YOSEMITE_CLI push action yx.identity setkyc "{\"account\":\"useraccount1\", \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p idauth1@active

// update account state (e.g. blacklist account)
$YOSEMITE_CLI push action yx.identity setstate "{\"account\":\"useraccount1\", \"state\":$(echo 'ibase=2; 0001' | bc)}" -p idauth1@active

// update identity-authority specific data
$YOSEMITE_CLI push action yx.identity setdata "{\"account\":\"useraccount1\", \"data\":\"23fiuygy3\"}" -p idauth1@active
```

Native Token Issue / Transfer
---

```bash

$YOSEMITE_CLI push action yx.ntoken nissue '["useraccount2",{"amount":"100000.0000 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active
executed transaction: 5d60dada4af867696f4bf43842c9820218da2fbb8166d2c3d83f32bd7c07ad89  136 bytes  3767 us
#     yx.ntoken <= yx.ntoken::nissue            {"to":"useraccount2","token":{"amount":"100000.0000 DKRW","issuer":"sysdepo1"},"memo":"nissue test"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"useraccount2","token":{"amount":"100000.0000 DKRW","issuer":"sysdepo1"},"me...
#      sysdepo1 <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"useraccount2","token":{"amount":"100000.0000 DKRW","issuer":"sysdepo1"},"me...
#  useraccount2 <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"useraccount2","token":{"amount":"100000.0000 DKRW","issuer":"sysdepo1"},"me..

$YOSEMITE_CLI push action yx.ntoken nissue '["useraccount3",{"amount":"100000.0000 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active

// unable to issue to blacklisted account
//$YOSEMITE_CLI push action yx.ntoken nissue '["useraccount1",{"amount":"100000.0000 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active


$YOSEMITE_CLI push action yx.ntoken transfer '[ "useraccount2", "useraccount3", "10000.0000 DKRW", "memo" ]' -p useraccount2
executed transaction: 0bf46f27f6689de95d875e2ad9f6f5428b139f377a5790313e618de371150dc5  136 bytes  7379 us
#     yx.ntoken <= yx.ntoken::transfer          {"from":"useraccount2","to":"useraccount3","amount":"10000.0000 DKRW","memo":"memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"useraccount2","token":"100.0000 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.0000 DKRW","issuer":"sysdepo1"}}
#  useraccount2 <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.0000 DKRW","issuer":"sysdepo1"}}
#      yx.txfee <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.0000 DKRW","issuer":"sysdepo1"}}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.0000 DKRW","issuer":"sysdepo1"},...
#  useraccount2 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.0000 DKRW","issuer":"sysdepo1"},...
#  useraccount3 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.0000 DKRW","issuer":"sysdepo1"},...

$YOSEMITE_CLI push action yx.ntoken ntransfer '[ "useraccount2", "useraccount3", {"amount":"10000.0000 DKRW","issuer":"sysdepo1"}, "memo" ]' -p useraccount2
executed transaction: f443593e42ff28e1515a26d87d9b0b476849954ceb77ef1057e8dd07a857cea8  144 bytes  4947 us
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.0000 DKRW","issuer":"sysdepo1"},...
#  useraccount2 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.0000 DKRW","issuer":"sysdepo1"},...
#  useraccount3 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.0000 DKRW","issuer":"sysdepo1"},...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"useraccount2","token":"200.0000 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"200.0000 DKRW","issuer":"sysdepo1"}}
#  useraccount2 <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"200.0000 DKRW","issuer":"sysdepo1"}}
#      yx.txfee <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"200.0000 DKRW","issuer":"sysdepo1"}}

$YOSEMITE_CLI push action yx.ntoken wptransfer '[ "useraccount3", "useraccount2", "10000.0000 DKRW", "useraccount2", "memo" ]' -p useraccount3 -p useraccount2
executed transaction: 8f7e234e837021535c8eb9302ed3052307e972ac9da001de7db35c80714f846f  168 bytes  7554 us
#     yx.ntoken <= yx.ntoken::wptransfer        {"from":"useraccount3","to":"useraccount2","amount":"10000.0000 DKRW","payer":"useraccount2","memo":...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"useraccount2","token":"100.0000 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.0000 DKRW","issuer":"sysdepo1"}}
#  useraccount2 <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.0000 DKRW","issuer":"sysdepo1"}}
#      yx.txfee <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.0000 DKRW","issuer":"sysdepo1"}}
#     yx.ntoken <= yx.ntoken::wpntransfer       {"from":"useraccount3","to":"useraccount2","token":{"amount":"10000.0000 DKRW","issuer":"sysdepo1"},...
#  useraccount3 <= yx.ntoken::wpntransfer       {"from":"useraccount3","to":"useraccount2","token":{"amount":"10000.0000 DKRW","issuer":"sysdepo1"},...
#  useraccount2 <= yx.ntoken::wpntransfer       {"from":"useraccount3","to":"useraccount2","token":{"amount":"10000.0000 DKRW","issuer":"sysdepo1"},...

$YOSEMITE_CLI get table yx.ntoken sysdepo1 ntstats
$YOSEMITE_CLI get table yx.ntoken useraccount1 ntaccounts
$YOSEMITE_CLI get table yx.ntoken useraccount2 ntaccounts
$YOSEMITE_CLI get table yx.ntoken useraccount3 ntaccounts
$YOSEMITE_CLI get table yx.ntoken yx.txfee ntaccounts
$YOSEMITE_CLI get table yx.ntoken sysdepo1 ntaccounts

```

Claim Producer Rewards
---
---
```bash
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"producer.c\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"bp\"}" -p idauth1@active

$YOSEMITE_CLI push action yosemite claimrewards '["producer.c"]' -p producer.c@active
executed transaction: 96c1b0e367c4bbc85801242dc5a8ba33de274c4aee6ce898ac0b9008c8ca655c  104 bytes  4891 us
#      yosemite <= yosemite::claimrewards       {"owner":"producer.c"}
#     yx.ntoken <= yx.ntoken::transfer          {"from":"yx.txfee","to":"producer.c","amount":"247.8260 DKRW","memo":""}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"yx.txfee","to":"producer.c","token":{"amount":"247.8260 DKRW","issuer":"sysdepo1"},"memo":"...
#      yx.txfee <= yx.ntoken::ntransfer         {"from":"yx.txfee","to":"producer.c","token":{"amount":"247.8260 DKRW","issuer":"sysdepo1"},"memo":"...
#    producer.c <= yx.ntoken::ntransfer         {"from":"yx.txfee","to":"producer.c","token":{"amount":"247.8260 DKRW","issuer":"sysdepo1"},"memo":"...

$YOSEMITE_CLI get table yosemite yosemite producers

$YOSEMITE_CLI get table yx.ntoken yx.txfee ntaccounts
$YOSEMITE_CLI get table yx.ntoken producer.c ntaccounts

```

Transaction Vote 
---

```bash
$YOSEMITE_CLI push action --help
Push a transaction with a single action
Usage: cleos push action [OPTIONS] account action data

Options:
...
  -v,--trx-vote TEXT          transaction vote target account, Transaction-as-a-Vote(TaaV) for YOSEMITE Proof-of-Transaction(PoT)
...
  
$YOSEMITE_CLI push action yx.ntoken transfer '[ "useraccount3", "useraccount2", "10000.0000 DKRW", "memo" ]' -p useraccount3 -v producer.f
$YOSEMITE_CLI get table yosemite yosemite producers
```

Synchronize action data 
---
```bash
$YOSEMITE_CLI_TESTNET get actions yx.ntoken 0 9
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#    0   2018-08-17T06:32:57.500         yosemite::setcode => yosemite      c5c43a79... {"account":"yx.ntoken","vmtype":0,"vmversion":0,"code":"0061...
#    1   2018-08-17T06:32:57.500          yosemite::setabi => yosemite      c5c43a79... {"account":"yx.ntoken","abi":"0e656f73696f3a3a6162692f312e30...
#    2   2018-08-17T06:32:57.500     yx.ntoken::setkycrule => yx.ntoken     9b066054... {"type":0,"kyc":15}...
#    3   2018-08-17T06:32:57.500     yx.ntoken::setkycrule => yx.ntoken     1ebd6b37... {"type":1,"kyc":15}...
#    4   2018-08-17T06:36:10.000      yosemite::updateauth => yosemite      dc14ff64... {"account":"yx.ntoken","permission":"owner","parent":"","aut...
#    5   2018-08-17T06:36:10.000      yosemite::updateauth => yosemite      d153eff4... {"account":"yx.ntoken","permission":"active","parent":"owner...
#    6   2018-08-17T06:36:11.500         yx.ntoken::nissue => yx.ntoken     eb2f8abc... {"to":"com","token":{"amount":"5000000.0000 DKRW","issuer":"...
#    7   2018-08-17T06:36:11.500      yx.ntoken::ntransfer => yx.ntoken     eb2f8abc... {"from":"sysdepo1","to":"com","token":{"amount":"5000000.000...
#    8   2018-08-17T06:36:11.500         yx.ntoken::payfee => yx.ntoken     d59e98ad... {"payer":"com","token":{"amount":"1000.0000 DKRW","issuer":"...
#    9   2018-08-17T06:36:14.000         yx.ntoken::nissue => yx.ntoken     e91aaf07... {"to":"useraccount2","token":{"amount":"100000.0000 DKRW","i...

$YOSEMITE_CLI_TESTNET get actions yx.ntoken 10 9
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#   10   2018-08-17T06:36:14.000      yx.ntoken::ntransfer => yx.ntoken     e91aaf07... {"from":"sysdepo1","to":"useraccount2","token":{"amount":"10...
#   11   2018-08-17T06:36:14.000         yx.ntoken::nissue => yx.ntoken     ad9e02d1... {"to":"useraccount3","token":{"amount":"100000.0000 DKRW","i...
#   12   2018-08-17T06:36:14.000      yx.ntoken::ntransfer => yx.ntoken     ad9e02d1... {"from":"sysdepo1","to":"useraccount3","token":{"amount":"10...
#   13   2018-08-17T06:36:14.000       yx.ntoken::transfer => yx.ntoken     eadb558f... {"from":"useraccount2","to":"useraccount3","amount":"10000.0...
#   14   2018-08-17T06:36:14.000      yx.ntoken::ntransfer => yx.ntoken     eadb558f... {"from":"useraccount2","to":"useraccount3","token":{"amount"...
#   15   2018-08-17T06:36:14.000         yx.ntoken::payfee => yx.ntoken     eadb558f... {"payer":"useraccount2","token":{"amount":"100.0000 DKRW","i...
#   16   2018-08-17T06:36:14.000      yx.ntoken::ntransfer => yx.ntoken     c15fabd8... {"from":"useraccount2","to":"useraccount3","token":{"amount"...
#   17   2018-08-17T06:36:14.000         yx.ntoken::payfee => yx.ntoken     c15fabd8... {"payer":"useraccount2","token":{"amount":"200.0000 DKRW","i...
#   18   2018-08-17T06:36:14.000     yx.ntoken::wptransfer => yx.ntoken     12e702db... {"from":"useraccount3","to":"useraccount2","amount":"10000.0...
#   19   2018-08-17T06:36:14.000    yx.ntoken::wpntransfer => yx.ntoken     12e702db... {"from":"useraccount3","to":"useraccount2","token":{"amount"...


$YOSEMITE_CLI_TESTNET get actions yx.ntoken -1 -1
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#   31   2018-08-17T06:36:19.500         yx.ntoken::payfee => yx.ntoken     a5b8539d... {"payer":"useraccount3","token":{"amount":"100.0000 DKRW","i...
```

Get transaction data
---

```bash
$YOSEMITE_CLI_TESTNET get transaction e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296
{
  "id": "e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296",
  "trx": {
    "receipt": {
      "status": "executed",
      "cpu_usage_us": 3658,
      "net_usage_words": 17,
      "trx": [
        1,{
          "signatures": [
            "SIG_K1_K8PtT9kUxbtkMLsj7NJDMRHA5vDTqjYLt3n6JztttQUAtFppvU3QCWTWiHLMHmecL8pS4WSAizcL1N2zkPwbwVyPv4Nru6"
          ],
          "compression": "none",
          "packed_context_free_data": "",
          "packed_trx": "fb6c765b9e01fd78dd6300000000010000980ad23c41f700000000288db19b01000000815695b0c700000000a8ed32322c20f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e6973737565207465737400"
        }
      ]
    },
    "trx": {
      "expiration": "2018-08-17T06:36:43",
      "ref_block_num": 414,
      "ref_block_prefix": 1675458813,
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
            "token": {
              "amount": "100000.0000 DKRW",
              "issuer": "sysdepo1"
            },
            "memo": "nissue test"
          },
          "hex_data": "20f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
        }
      ],
      "transaction_extensions": [],
      "signatures": [
        "SIG_K1_K8PtT9kUxbtkMLsj7NJDMRHA5vDTqjYLt3n6JztttQUAtFppvU3QCWTWiHLMHmecL8pS4WSAizcL1N2zkPwbwVyPv4Nru6"
      ],
      "context_free_data": []
    }
  },
  "block_time": "2018-08-17T06:36:14.000",
  "block_num": 479,
  "last_irreversible_block": 4674,
  "traces": [{
      "receipt": {
        "receiver": "yx.ntoken",
        "act_digest": "ce13bb5b5ed2505fd02a6eef6eecdf88420b6033a44bcd9b00c0044273b70ad3",
        "global_sequence": 600,
        "recv_sequence": 6,
        "auth_sequence": [[
            "sysdepo1",
            6
          ]
        ],
        "code_sequence": 1,
        "abi_sequence": 1
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
          "token": {
            "amount": "100000.0000 DKRW",
            "issuer": "sysdepo1"
          },
          "memo": "nissue test"
        },
        "hex_data": "20f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
      },
      "elapsed": 1084,
      "cpu_usage": 0,
      "console": "",
      "total_cpu_usage": 0,
      "trx_id": "e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296",
      "inline_traces": [{
          "receipt": {
            "receiver": "yx.ntoken",
            "act_digest": "775d2609eb6b4445d6b062eee51b50dcc0052a66555c17ed3f4a41981b844ff2",
            "global_sequence": 601,
            "recv_sequence": 7,
            "auth_sequence": [[
                "sysdepo1",
                7
              ],[
                "yosemite",
                561
              ]
            ],
            "code_sequence": 1,
            "abi_sequence": 1
          },
          "act": {
            "account": "yx.ntoken",
            "name": "ntransfer",
            "authorization": [{
                "actor": "sysdepo1",
                "permission": "active"
              },{
                "actor": "yosemite",
                "permission": "active"
              }
            ],
            "data": {
              "from": "sysdepo1",
              "to": "useraccount2",
              "token": {
                "amount": "100000.0000 DKRW",
                "issuer": "sysdepo1"
              },
              "memo": "nissue test"
            },
            "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
          },
          "elapsed": 2386,
          "cpu_usage": 0,
          "console": "",
          "total_cpu_usage": 0,
          "trx_id": "e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296",
          "inline_traces": [{
              "receipt": {
                "receiver": "sysdepo1",
                "act_digest": "775d2609eb6b4445d6b062eee51b50dcc0052a66555c17ed3f4a41981b844ff2",
                "global_sequence": 602,
                "recv_sequence": 2,
                "auth_sequence": [[
                    "sysdepo1",
                    8
                  ],[
                    "yosemite",
                    562
                  ]
                ],
                "code_sequence": 1,
                "abi_sequence": 1
              },
              "act": {
                "account": "yx.ntoken",
                "name": "ntransfer",
                "authorization": [{
                    "actor": "sysdepo1",
                    "permission": "active"
                  },{
                    "actor": "yosemite",
                    "permission": "active"
                  }
                ],
                "data": {
                  "from": "sysdepo1",
                  "to": "useraccount2",
                  "token": {
                    "amount": "100000.0000 DKRW",
                    "issuer": "sysdepo1"
                  },
                  "memo": "nissue test"
                },
                "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
              },
              "elapsed": 3,
              "cpu_usage": 0,
              "console": "",
              "total_cpu_usage": 0,
              "trx_id": "e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296",
              "inline_traces": []
            },{
              "receipt": {
                "receiver": "useraccount2",
                "act_digest": "775d2609eb6b4445d6b062eee51b50dcc0052a66555c17ed3f4a41981b844ff2",
                "global_sequence": 603,
                "recv_sequence": 1,
                "auth_sequence": [[
                    "sysdepo1",
                    9
                  ],[
                    "yosemite",
                    563
                  ]
                ],
                "code_sequence": 1,
                "abi_sequence": 1
              },
              "act": {
                "account": "yx.ntoken",
                "name": "ntransfer",
                "authorization": [{
                    "actor": "sysdepo1",
                    "permission": "active"
                  },{
                    "actor": "yosemite",
                    "permission": "active"
                  }
                ],
                "data": {
                  "from": "sysdepo1",
                  "to": "useraccount2",
                  "token": {
                    "amount": "100000.0000 DKRW",
                    "issuer": "sysdepo1"
                  },
                  "memo": "nissue test"
                },
                "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
              },
              "elapsed": 3,
              "cpu_usage": 0,
              "console": "",
              "total_cpu_usage": 0,
              "trx_id": "e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296",
              "inline_traces": []
            }
          ]
        }
      ]
    },{
      "receipt": {
        "receiver": "yx.ntoken",
        "act_digest": "775d2609eb6b4445d6b062eee51b50dcc0052a66555c17ed3f4a41981b844ff2",
        "global_sequence": 601,
        "recv_sequence": 7,
        "auth_sequence": [[
            "sysdepo1",
            7
          ],[
            "yosemite",
            561
          ]
        ],
        "code_sequence": 1,
        "abi_sequence": 1
      },
      "act": {
        "account": "yx.ntoken",
        "name": "ntransfer",
        "authorization": [{
            "actor": "sysdepo1",
            "permission": "active"
          },{
            "actor": "yosemite",
            "permission": "active"
          }
        ],
        "data": {
          "from": "sysdepo1",
          "to": "useraccount2",
          "token": {
            "amount": "100000.0000 DKRW",
            "issuer": "sysdepo1"
          },
          "memo": "nissue test"
        },
        "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
      },
      "elapsed": 2386,
      "cpu_usage": 0,
      "console": "",
      "total_cpu_usage": 0,
      "trx_id": "e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296",
      "inline_traces": [{
          "receipt": {
            "receiver": "sysdepo1",
            "act_digest": "775d2609eb6b4445d6b062eee51b50dcc0052a66555c17ed3f4a41981b844ff2",
            "global_sequence": 602,
            "recv_sequence": 2,
            "auth_sequence": [[
                "sysdepo1",
                8
              ],[
                "yosemite",
                562
              ]
            ],
            "code_sequence": 1,
            "abi_sequence": 1
          },
          "act": {
            "account": "yx.ntoken",
            "name": "ntransfer",
            "authorization": [{
                "actor": "sysdepo1",
                "permission": "active"
              },{
                "actor": "yosemite",
                "permission": "active"
              }
            ],
            "data": {
              "from": "sysdepo1",
              "to": "useraccount2",
              "token": {
                "amount": "100000.0000 DKRW",
                "issuer": "sysdepo1"
              },
              "memo": "nissue test"
            },
            "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
          },
          "elapsed": 3,
          "cpu_usage": 0,
          "console": "",
          "total_cpu_usage": 0,
          "trx_id": "e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296",
          "inline_traces": []
        },{
          "receipt": {
            "receiver": "useraccount2",
            "act_digest": "775d2609eb6b4445d6b062eee51b50dcc0052a66555c17ed3f4a41981b844ff2",
            "global_sequence": 603,
            "recv_sequence": 1,
            "auth_sequence": [[
                "sysdepo1",
                9
              ],[
                "yosemite",
                563
              ]
            ],
            "code_sequence": 1,
            "abi_sequence": 1
          },
          "act": {
            "account": "yx.ntoken",
            "name": "ntransfer",
            "authorization": [{
                "actor": "sysdepo1",
                "permission": "active"
              },{
                "actor": "yosemite",
                "permission": "active"
              }
            ],
            "data": {
              "from": "sysdepo1",
              "to": "useraccount2",
              "token": {
                "amount": "100000.0000 DKRW",
                "issuer": "sysdepo1"
              },
              "memo": "nissue test"
            },
            "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
          },
          "elapsed": 3,
          "cpu_usage": 0,
          "console": "",
          "total_cpu_usage": 0,
          "trx_id": "e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296",
          "inline_traces": []
        }
      ]
    },{
      "receipt": {
        "receiver": "sysdepo1",
        "act_digest": "775d2609eb6b4445d6b062eee51b50dcc0052a66555c17ed3f4a41981b844ff2",
        "global_sequence": 602,
        "recv_sequence": 2,
        "auth_sequence": [[
            "sysdepo1",
            8
          ],[
            "yosemite",
            562
          ]
        ],
        "code_sequence": 1,
        "abi_sequence": 1
      },
      "act": {
        "account": "yx.ntoken",
        "name": "ntransfer",
        "authorization": [{
            "actor": "sysdepo1",
            "permission": "active"
          },{
            "actor": "yosemite",
            "permission": "active"
          }
        ],
        "data": {
          "from": "sysdepo1",
          "to": "useraccount2",
          "token": {
            "amount": "100000.0000 DKRW",
            "issuer": "sysdepo1"
          },
          "memo": "nissue test"
        },
        "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
      },
      "elapsed": 3,
      "cpu_usage": 0,
      "console": "",
      "total_cpu_usage": 0,
      "trx_id": "e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296",
      "inline_traces": []
    },{
      "receipt": {
        "receiver": "useraccount2",
        "act_digest": "775d2609eb6b4445d6b062eee51b50dcc0052a66555c17ed3f4a41981b844ff2",
        "global_sequence": 603,
        "recv_sequence": 1,
        "auth_sequence": [[
            "sysdepo1",
            9
          ],[
            "yosemite",
            563
          ]
        ],
        "code_sequence": 1,
        "abi_sequence": 1
      },
      "act": {
        "account": "yx.ntoken",
        "name": "ntransfer",
        "authorization": [{
            "actor": "sysdepo1",
            "permission": "active"
          },{
            "actor": "yosemite",
            "permission": "active"
          }
        ],
        "data": {
          "from": "sysdepo1",
          "to": "useraccount2",
          "token": {
            "amount": "100000.0000 DKRW",
            "issuer": "sysdepo1"
          },
          "memo": "nissue test"
        },
        "hex_data": "000000815695b0c720f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
      },
      "elapsed": 3,
      "cpu_usage": 0,
      "console": "",
      "total_cpu_usage": 0,
      "trx_id": "e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296",
      "inline_traces": []
    }
  ]
}
```