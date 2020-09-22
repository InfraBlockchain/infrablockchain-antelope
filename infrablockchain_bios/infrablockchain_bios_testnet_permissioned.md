InfraBlockchain (Permissioned version) BIOS setup for Testnet 
===
(revision 20180820 for Testnet v0.4.0)

Testnet Public Access Node URL
---
* [http://testnet.yosemitelabs.org:8888](http://testnet.yosemitelabs.org:8888)

```bash
INFRA_CLI_TESTNET="$INFRABLOCKCHAIN_HOME/build/programs/infra-cli/infra-cli -u http://testnet.yosemitelabs.org:8888"
$INFRA_CLI_TESTNET get info
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
INFRA_NODE_BIN_NAME=infra-node
INFRA_CLI_BIN_NAME=infra-cli
INFRA_KEYSTORE_BIN_NAME=infra-keystore
INFRABLOCKCHAIN_HOME=/mnt/infrablockchain-git
INFRA_NODE=$INFRABLOCKCHAIN_HOME/build/programs/$INFRA_NODE_BIN_NAME/$INFRA_NODE_BIN_NAME
INFRA_NODE_LOG_FILE=/mnt/$INFRA_NODE_BIN_NAME.log
INFRA_CLI="$INFRABLOCKCHAIN_HOME/build/programs/$INFRA_CLI_BIN_NAME/$INFRA_CLI_BIN_NAME --wallet-url http://127.0.0.1:8900/"
INFRA_CLI_TESTNET="$INFRA_CLI -u http://testnet.yosemitelabs.org:8888 --wallet-url http://127.0.0.1:8900/"
INFRA_KEYSTORE=$INFRABLOCKCHAIN_HOME/build/programs/$INFRA_KEYSTORE_BIN_NAME/$INFRA_KEYSTORE_BIN_NAME
INFRA_KEYSTORE_LOG_FILE=/mnt/$INFRA_KEYSTORE_BIN_NAME.log
INFRA_KEYSTORE_WALLET_PASSWORD=PW5KH7i8ZEuVMvywMschs3TznhTfCdmgpPBGKJLUQjs6N6oQ7boZj
INFRA_NODE_CONFIG=$INFRABLOCKCHAIN_HOME/infrablockchain_config/config_infrablockchain_testnet_boot.ini
INFRA_NODE_GENESIS_JSON=$INFRABLOCKCHAIN_HOME/infrablockchain_config/genesis_infrablockchain_testnet.json
INFRA_NODE_DATA_DIR=/mnt/infrablockchain_node_data
INFRABLOCKCHAIN_DEV_WALLET_DIR=/mnt/infrablockchain_dev_wallet
INFRABLOCKCHAIN_CONTRACTS_DIR=$INFRABLOCKCHAIN_HOME/build/contracts
INFRABLOCKCHAIN_MONGOD=/home/ubuntu/opt/mongodb/bin/mongod
INFRABLOCKCHAIN_MONGODB_CONFIG=/home/ubuntu/opt/mongodb/mongod.conf
INFRABLOCKCHAIN_MONGODB_DATA_DIR=/mnt/mongodb
```

Initial key
---
```bash
$INFRA_CLI create key --to-console
Private key: YPV_5J7MF5bWVzjLczmVFMwiRjD5TVBG1o8UELy4jkaikXETbaMHZSb
Public key: YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
```

Genesis Json
---
```json
{
  "initial_timestamp": "2018-08-15T12:00:00.000",
  "initial_key": "YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3",
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

* config_infrablockchain_testnet_boot.ini
```ini
chain-state-db-size-mb = 4096
read-mode = speculative
filter-on = *
contracts-console = true
http-server-address = 0.0.0.0:8888
mongodb-uri = mongodb://localhost:27017/infrablockchaintest
p2p-listen-endpoint = 127.0.0.1:9876
p2p-max-nodes-per-host = 25
agent-name = "InfraBlockchain Testnet Dev Agent"
max-clients = 25
enable-stale-production = true
producer-name = infrasys
signature-provider = YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3=KEY:YPV_5J7MF5bWVzjLczmVFMwiRjD5TVBG1o8UELy4jkaikXETbaMHZSb
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
$INFRA_NODE --config $INFRA_NODE_CONFIG --genesis-json $INFRA_NODE_GENESIS_JSON --data-dir $INFRA_NODE_DATA_DIR
```

* from existing node data
```bash
$INFRA_NODE --print-genesis-json --config $INFRA_NODE_CONFIG --data-dir $INFRA_NODE_DATA_DIR
nohup $INFRA_NODE --config $INFRA_NODE_CONFIG --data-dir $INFRA_NODE_DATA_DIR > /mnt/infra-node.log 2>&1&
tail -f /mnt/infra-node.log -n 300
pgrep infra-node
pkill -SIGINT infra-node
```

* replay blockchain from tx log
```bash
$INFRA_NODE --config $INFRA_NODE_CONFIG --data-dir $INFRA_NODE_DATA_DIR --replay-blockchain
```

Start Wallet (infra-keystore)
---

* run key daemon
```bash
nohup $INFRA_KEYSTORE --unlock-timeout 999999999 --http-server-address 127.0.0.1:8900 --wallet-dir $INFRABLOCKCHAIN_DEV_WALLET_DIR > /mnt/infra-keystore.log 2>&1&
tail -f /mnt/infra-keystore.log -n 300
pgrep infra-keystore
pkill -SIGINT infra-keystore
```

* create wallet / unlock
```bash
$INFRA_CLI wallet create
Creating wallet: default
"PW5KH7i8ZEuVMvywMschs3TznhTfCdmgpPBGKJLUQjs6N6oQ7boZj"

$INFRA_CLI wallet open
$INFRA_CLI wallet unlock
```

* infrablockchain initial key
```bash
$INFRA_CLI wallet import --private-key YPV_5J7MF5bWVzjLczmVFMwiRjD5TVBG1o8UELy4jkaikXETbaMHZSb
```

Create System Accounts
---

```bash
$INFRA_CLI create account infrasys sys.msig YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$INFRA_CLI create account infrasys sys.txfee YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$INFRA_CLI create account infrasys sys.identity YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$INFRA_CLI create account infrasys yx.ntoken YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$INFRA_CLI create account infrasys yx.token YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$INFRA_CLI create account infrasys yx.dcontract YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$INFRA_CLI create account infrasys yx.nft YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$INFRA_CLI create account infrasys yx.nftex YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$INFRA_CLI create account infrasys yx.escrow YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3

```

Create initial Identity Authority Account
---

* idauth1
```bash
$INFRA_CLI create key --to-console
Private key: YPV_5K5Qe2ndgcfhWqdUjHnfhvCKVqCE82Rih6JrNQuuVJBcathAx9m
Public key: YOS6EfGUaA5MNLH1GiHd64DcDr3HMgY1AM3WR1vdHKaah9Z4cWPZq
$INFRA_CLI wallet import --private-key YPV_5K5Qe2ndgcfhWqdUjHnfhvCKVqCE82Rih6JrNQuuVJBcathAx9m

$INFRA_CLI create account infrasys idauth1 YOS6EfGUaA5MNLH1GiHd64DcDr3HMgY1AM3WR1vdHKaah9Z4cWPZq

```

* idauth2
```bash
$INFRA_CLI create key --to-console
Private key: YPV_5JRzPhyg9jY4thDJ1rLsSr4zdLLoA7UEMQRw3vJFGmGKTZF5Kbx
Public key: YOS5VvWLCqXL3AQyuXZfUTnEiWuhJNV6Nq3YbbS7Z98cN7GiRo9LR
$INFRA_CLI wallet import --private-key YPV_5JRzPhyg9jY4thDJ1rLsSr4zdLLoA7UEMQRw3vJFGmGKTZF5Kbx

$INFRA_CLI create account infrasys idauth2 YOS5VvWLCqXL3AQyuXZfUTnEiWuhJNV6Nq3YbbS7Z98cN7GiRo9LR
```


Create initial System Depository Account
---

* sysdepo1
```bash
$INFRA_CLI create key --to-console
Private key: YPV_5J8WZRwfYVzsH8i3LaRQLGorVD788RFYqALEsLmeK8a6Kq6re2W
Public key: YOS6o8hwGio5V2LgBxFMZi4bdVG5AvcHeFiMpaREKuZ29BwXpNTpH
$INFRA_CLI wallet import --private-key YPV_5J8WZRwfYVzsH8i3LaRQLGorVD788RFYqALEsLmeK8a6Kq6re2W

$INFRA_CLI create account infrasys sysdepo1 YOS6o8hwGio5V2LgBxFMZi4bdVG5AvcHeFiMpaREKuZ29BwXpNTpH

```

* sysdepo2
```bash
$INFRA_CLI create key --to-console
Private key: YPV_5K5MBizJ6HuG1kdvQTeMnpKjpFSZHm64Nj3X6MvJR6AnTF8mHkx
Public key: YOS61njg9Zs9oQmAmwLnrcVBrniWKshKAhoQJJqNVVnYXCPSRKLTE
$INFRA_CLI wallet import --private-key YPV_5K5MBizJ6HuG1kdvQTeMnpKjpFSZHm64Nj3X6MvJR6AnTF8mHkx

$INFRA_CLI create account infrasys sysdepo2 YOS61njg9Zs9oQmAmwLnrcVBrniWKshKAhoQJJqNVVnYXCPSRKLTE
```

Install InfraBlockchain System Contract
---
```bash
$INFRA_CLI set contract infrasys $INFRABLOCKCHAIN_CONTRACTS_DIR/infrasys/ -p infrasys@active
```

Install Multi-sig System Contract
---
```bash
$INFRA_CLI set contract sys.msig $INFRABLOCKCHAIN_CONTRACTS_DIR/eosio.msig/ -p infrasys.msig@active
$INFRA_CLI push action infrasys setpriv '["sys.msig",1]' -p infrasys@active
```

Install Transaction Fee System Contract
---
```bash
$INFRA_CLI set contract sys.txfee $INFRABLOCKCHAIN_CONTRACTS_DIR/sys.txfee/ -p sys.txfee@active
```

Install Identity System Contract
---
```bash
$INFRA_CLI set contract sys.identity $INFRABLOCKCHAIN_CONTRACTS_DIR/sys.identity/ -p sys.identity@active
```

Register Initial Identity Authority
---
```bash
$INFRA_CLI push action infrasys regidauth '["idauth1","http://idauth1.org",1]' -p idauth1@active -p infrasys@active
$INFRA_CLI push action infrasys authidauth '["idauth1"]' -p infrasys@active

$INFRA_CLI push action infrasys regidauth '["idauth2","http://idauth2.org",1]' -p idauth2@active -p infrasys@active
$INFRA_CLI push action infrasys authidauth '["idauth2"]' -p infrasys@active

//$INFRA_CLI push action infrasys rmvidauth '["idauth2"]' -p infrasys@active

$INFRA_CLI get table infrasys infrasys idauthority
```

Register Initial System Depository
---
```bash
$INFRA_CLI push action infrasys regsysdepo '["sysdepo1","http://sysdepo1.org",1]' -p sysdepo1@active -p infrasys@active
$INFRA_CLI push action infrasys authsysdepo '["sysdepo1"]' -p infrasys@active

$INFRA_CLI push action infrasys regsysdepo '["sysdepo2","http://sysdepo2.org",1]' -p sysdepo2@active -p infrasys@active
$INFRA_CLI push action infrasys authsysdepo '["sysdepo2"]' -p infrasys@active

$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"sysdepo1\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"sysdepo\"}" -p idauth1@active
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"sysdepo2\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"sysdepo\"}" -p idauth1@active
$INFRA_CLI get table -L sysdepo1 -l 1 sys.identity sys.identity identity
$INFRA_CLI get table -L sysdepo2 -l 1 sys.identity sys.identity identity

$INFRA_CLI get table infrasys infrasys sysdepos
```

Install Native Token Contract
---
```bash
$INFRA_CLI set contract yx.ntoken $INFRABLOCKCHAIN_CONTRACTS_DIR/yx.ntoken/
$INFRA_CLI push action infrasys setpriv '["yx.ntoken",1]' -p infrasys@active

$INFRA_CLI push action yx.ntoken setkycrule "{\"type\":0, \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p infrasys@active
$INFRA_CLI push action yx.ntoken setkycrule "{\"type\":1, \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p infrasys@active

```

Install User Token Contract
---
```bash
$INFRA_CLI set contract yx.token $INFRABLOCKCHAIN_CONTRACTS_DIR/yx.token/
$INFRA_CLI push action infrasys setpriv '["yx.token",1]' -p infrasys@active
```

Install DigitalContract Contract
---
```bash
$INFRA_CLI set contract yx.dcontract $INFRABLOCKCHAIN_CONTRACTS_DIR/yx.dcontract/
$INFRA_CLI push action infrasys setpriv '["yx.dcontract",1]' -p infrasys@active
```

Install Non-Fungible-Token(ERC721) Contract
---
```bash
$INFRA_CLI set contract yx.nft $INFRABLOCKCHAIN_CONTRACTS_DIR/yx.nft/
$INFRA_CLI push action infrasys setpriv '["yx.nft",1]' -p infrasys@active
```

Install Non-Fungible-Token(ERC721) Exchange Contract
---
```bash
$INFRA_CLI set contract yx.nftex $INFRABLOCKCHAIN_CONTRACTS_DIR/yx.nftex/
$INFRA_CLI push action infrasys setpriv '["yx.nftex",1]' -p infrasys@active
```

Install Token Escrow Contract
---
```bash
$INFRA_CLI set contract yx.escrow $INFRABLOCKCHAIN_CONTRACTS_DIR/yx.escrow/
$INFRA_CLI push action infrasys setpriv '["yx.escrow",1]' -p infrasys@active
```

Querying the status of System Depositories, Identity Authorities and Block Producers
---
```bash
$INFRA_CLI get table infrasys infrasys global
$INFRA_CLI get table infrasys infrasys sysdepos
$INFRA_CLI get table infrasys infrasys idauthority
$INFRA_CLI get table infrasys infrasys producers
```

Initial Block Producer Setup
---

* config_infrablockchain_testnet_boot.ini (one node multi producer setup)
```ini
producer-name = infrasys
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
signature-provider = YOS5Audoa4mpZaYhp7vwYVCUnsQCUVifftdPipvkfZ9qVggoYoHUn=KEY:YPV_5HsXETBYTpZbgu99EDVmdihxCkVm3i1DU6PL6BQRXAxJnacsooh
# producer.b
signature-provider = YOS5aw9PzjxJCTi23FWtcB6Q8feMhfLg7Toh7PwGoWge4K4xNWQdm=KEY:YPV_5JpFP6dYxrNDGnGKuj6kGNovk5a2QK8V7c1pyp5b93DSjE2WDA3
# producer.c
signature-provider = YOS8cvC5FJozTTVfUVXZ4E4kz1eNsKoBsnG7J76Fw1gX1wstGoUWo=KEY:YPV_5KcHTPMwDxJ9yr14f8PZXgsoVbNnez1e85d3UfvWjvNFR817M7d
# producer.d
signature-provider = YOS6ig1G6hpk1Tzj1Ko8zfysATY4eqpb9znyEnx25zbkHscV6qHvy=KEY:YPV_5J93otpSC4UZXakAT1FFXndc2tUyPPYth7hkuFZ9UQpvtS9vXtZ
# producer.e
signature-provider = YOS72LDKqDc2KvyN1XeEYhv7AbkMUYB8B3fJ55yMn4ZqLzeqxz3w1=KEY:YPV_5KD33jEkkZ5K7GK4Sbp26YfDWT1vxyY4K931QFdoyBb2JXV4qSd
# producer.f
signature-provider = YOS5SzGAGCMznawLLY9xkpa4ta62CfTKu6di9AjZ9bWCBJ1pFu641=KEY:YPV_5JpAqiPZ5RrcKrTxXuGzEtSvyV4sC2E1tsxJGtCyXAREfoXLtVt
# producer.g
signature-provider = YOS5t1fHFunR2rWq5z8NHPrxj1H4xG5Vq4bGKcH33yg1eZMCVPQRq=KEY:YPV_5JW4NzsCak4hJCkoq85WJi9gdf1upGnY4XNAaYtxTnw4za7f5Kg
# producer.h
signature-provider = YOS5unphoov9UCG8AidDYv14fMJVRnHuihScASHbdFXHuDeDAS2s6=KEY:YPV_5JZ3EDx6PmSYXRs8gLvxhr6ZHvMJnJdnW6jD4AB5KnKguWmomjh
# producer.i
signature-provider = YOS5KM5t7td26VGDSEHXPHoUHB1Md1NnGRbT9EkXDaBf5nyhw18is=KEY:YPV_5KQvzWLVeKxgYY6oNGcYDg2QRViTA255nBuqHvoC2yPwaxqxhWh
# producer.j
signature-provider = YOS5oduMFs5Lrbb8ZEc11KtyoVqfUjvaRxbUsQGgTqsEq18p1KqoC=KEY:YPV_5Kgq6gkf2ybW86J7pk3HhFZvtGo9nU1AxukPptxYHLohpBGpiDm
```

* create block producer accounts
```bash
$INFRA_CLI create key --to-console
Private key: YPV_5HsXETBYTpZbgu99EDVmdihxCkVm3i1DU6PL6BQRXAxJnacsooh
Public key: YOS5Audoa4mpZaYhp7vwYVCUnsQCUVifftdPipvkfZ9qVggoYoHUn
$INFRA_CLI wallet import --private-key YPV_5HsXETBYTpZbgu99EDVmdihxCkVm3i1DU6PL6BQRXAxJnacsooh

$INFRA_CLI create key --to-console
Private key: YPV_5JpFP6dYxrNDGnGKuj6kGNovk5a2QK8V7c1pyp5b93DSjE2WDA3
Public key: YOS5aw9PzjxJCTi23FWtcB6Q8feMhfLg7Toh7PwGoWge4K4xNWQdm
$INFRA_CLI wallet import --private-key YPV_5JpFP6dYxrNDGnGKuj6kGNovk5a2QK8V7c1pyp5b93DSjE2WDA3

$INFRA_CLI create key --to-console
Private key: YPV_5KcHTPMwDxJ9yr14f8PZXgsoVbNnez1e85d3UfvWjvNFR817M7d
Public key: YOS8cvC5FJozTTVfUVXZ4E4kz1eNsKoBsnG7J76Fw1gX1wstGoUWo
$INFRA_CLI wallet import --private-key YPV_5KcHTPMwDxJ9yr14f8PZXgsoVbNnez1e85d3UfvWjvNFR817M7d

$INFRA_CLI create key --to-console
Private key: YPV_5J93otpSC4UZXakAT1FFXndc2tUyPPYth7hkuFZ9UQpvtS9vXtZ
Public key: YOS6ig1G6hpk1Tzj1Ko8zfysATY4eqpb9znyEnx25zbkHscV6qHvy
$INFRA_CLI wallet import --private-key YPV_5J93otpSC4UZXakAT1FFXndc2tUyPPYth7hkuFZ9UQpvtS9vXtZ
 
$INFRA_CLI create key --to-console
Private key: YPV_5KD33jEkkZ5K7GK4Sbp26YfDWT1vxyY4K931QFdoyBb2JXV4qSd
Public key: YOS72LDKqDc2KvyN1XeEYhv7AbkMUYB8B3fJ55yMn4ZqLzeqxz3w1
$INFRA_CLI wallet import --private-key YPV_5KD33jEkkZ5K7GK4Sbp26YfDWT1vxyY4K931QFdoyBb2JXV4qSd

$INFRA_CLI create key --to-console
Private key: YPV_5JpAqiPZ5RrcKrTxXuGzEtSvyV4sC2E1tsxJGtCyXAREfoXLtVt
Public key: YOS5SzGAGCMznawLLY9xkpa4ta62CfTKu6di9AjZ9bWCBJ1pFu641
$INFRA_CLI wallet import --private-key YPV_5JpAqiPZ5RrcKrTxXuGzEtSvyV4sC2E1tsxJGtCyXAREfoXLtVt

$INFRA_CLI create key --to-console
Private key: YPV_5JW4NzsCak4hJCkoq85WJi9gdf1upGnY4XNAaYtxTnw4za7f5Kg
Public key: YOS5t1fHFunR2rWq5z8NHPrxj1H4xG5Vq4bGKcH33yg1eZMCVPQRq
$INFRA_CLI wallet import --private-key YPV_5JW4NzsCak4hJCkoq85WJi9gdf1upGnY4XNAaYtxTnw4za7f5Kg

$INFRA_CLI create key --to-console
Private key: YPV_5JZ3EDx6PmSYXRs8gLvxhr6ZHvMJnJdnW6jD4AB5KnKguWmomjh
Public key: YOS5unphoov9UCG8AidDYv14fMJVRnHuihScASHbdFXHuDeDAS2s6
$INFRA_CLI wallet import --private-key YPV_5JZ3EDx6PmSYXRs8gLvxhr6ZHvMJnJdnW6jD4AB5KnKguWmomjh

$INFRA_CLI create key --to-console
Private key: YPV_5KQvzWLVeKxgYY6oNGcYDg2QRViTA255nBuqHvoC2yPwaxqxhWh
Public key: YOS5KM5t7td26VGDSEHXPHoUHB1Md1NnGRbT9EkXDaBf5nyhw18is
$INFRA_CLI wallet import --private-key YPV_5KQvzWLVeKxgYY6oNGcYDg2QRViTA255nBuqHvoC2yPwaxqxhWh

$INFRA_CLI create key --to-console
Private key: YPV_5Kgq6gkf2ybW86J7pk3HhFZvtGo9nU1AxukPptxYHLohpBGpiDm
Public key: YOS5oduMFs5Lrbb8ZEc11KtyoVqfUjvaRxbUsQGgTqsEq18p1KqoC
$INFRA_CLI wallet import --private-key YPV_5Kgq6gkf2ybW86J7pk3HhFZvtGo9nU1AxukPptxYHLohpBGpiDm

$INFRA_CLI create account infrasys producer.a YOS5Audoa4mpZaYhp7vwYVCUnsQCUVifftdPipvkfZ9qVggoYoHUn -p infrasys@active
$INFRA_CLI create account infrasys producer.b YOS5aw9PzjxJCTi23FWtcB6Q8feMhfLg7Toh7PwGoWge4K4xNWQdm -p infrasys@active
$INFRA_CLI create account infrasys producer.c YOS8cvC5FJozTTVfUVXZ4E4kz1eNsKoBsnG7J76Fw1gX1wstGoUWo -p infrasys@active
$INFRA_CLI create account infrasys producer.d YOS6ig1G6hpk1Tzj1Ko8zfysATY4eqpb9znyEnx25zbkHscV6qHvy -p infrasys@active
$INFRA_CLI create account infrasys producer.e YOS72LDKqDc2KvyN1XeEYhv7AbkMUYB8B3fJ55yMn4ZqLzeqxz3w1 -p infrasys@active
$INFRA_CLI create account infrasys producer.f YOS5SzGAGCMznawLLY9xkpa4ta62CfTKu6di9AjZ9bWCBJ1pFu641 -p infrasys@active
$INFRA_CLI create account infrasys producer.g YOS5t1fHFunR2rWq5z8NHPrxj1H4xG5Vq4bGKcH33yg1eZMCVPQRq -p infrasys@active
$INFRA_CLI create account infrasys producer.h YOS5unphoov9UCG8AidDYv14fMJVRnHuihScASHbdFXHuDeDAS2s6 -p infrasys@active
$INFRA_CLI create account infrasys producer.i YOS5KM5t7td26VGDSEHXPHoUHB1Md1NnGRbT9EkXDaBf5nyhw18is -p infrasys@active
$INFRA_CLI create account infrasys producer.j YOS5oduMFs5Lrbb8ZEc11KtyoVqfUjvaRxbUsQGgTqsEq18p1KqoC -p infrasys@active

```

* register and authorize block producers (permissioned)

```bash
$INFRA_CLI push action infrasys regproducer '["producer.a","YOS5Audoa4mpZaYhp7vwYVCUnsQCUVifftdPipvkfZ9qVggoYoHUn","http://producera.io",1]' -p producer.a@active -p infrasys@active
$INFRA_CLI push action infrasys regproducer '["producer.b","YOS5aw9PzjxJCTi23FWtcB6Q8feMhfLg7Toh7PwGoWge4K4xNWQdm","http://producerb.io",1]' -p producer.b@active -p infrasys@active

$INFRA_CLI push action infrasys authproducer '["producer.a"]' -p infrasys@active
$INFRA_CLI push action infrasys authproducer '["producer.b"]' -p infrasys@active

$INFRA_CLI push action infrasys regproducer '["producer.c","YOS8cvC5FJozTTVfUVXZ4E4kz1eNsKoBsnG7J76Fw1gX1wstGoUWo","http://producerc.io",1]' -p producer.c@active -p infrasys@active
$INFRA_CLI push action infrasys regproducer '["producer.d","YOS6ig1G6hpk1Tzj1Ko8zfysATY4eqpb9znyEnx25zbkHscV6qHvy","http://producerd.io",1]' -p producer.d@active -p infrasys@active
$INFRA_CLI push action infrasys regproducer '["producer.e","YOS72LDKqDc2KvyN1XeEYhv7AbkMUYB8B3fJ55yMn4ZqLzeqxz3w1","http://producere.io",1]' -p producer.e@active -p infrasys@active

$INFRA_CLI push action infrasys authproducer '["producer.c"]' -p infrasys@active
$INFRA_CLI push action infrasys authproducer '["producer.d"]' -p infrasys@active
$INFRA_CLI push action infrasys authproducer '["producer.e"]' -p infrasys@active

$INFRA_CLI push action infrasys regproducer '["producer.f","YOS5SzGAGCMznawLLY9xkpa4ta62CfTKu6di9AjZ9bWCBJ1pFu641","http://producerf.io",1]' -p producer.f@active -p infrasys@active
$INFRA_CLI push action infrasys regproducer '["producer.g","YOS5t1fHFunR2rWq5z8NHPrxj1H4xG5Vq4bGKcH33yg1eZMCVPQRq","http://producerg.io",1]' -p producer.g@active -p infrasys@active
$INFRA_CLI push action infrasys regproducer '["producer.h","YOS5unphoov9UCG8AidDYv14fMJVRnHuihScASHbdFXHuDeDAS2s6","http://producerh.io",1]' -p producer.h@active -p infrasys@active
$INFRA_CLI push action infrasys regproducer '["producer.i","YOS5KM5t7td26VGDSEHXPHoUHB1Md1NnGRbT9EkXDaBf5nyhw18is","http://produceri.io",1]' -p producer.i@active -p infrasys@active
$INFRA_CLI push action infrasys regproducer '["producer.j","YOS5oduMFs5Lrbb8ZEc11KtyoVqfUjvaRxbUsQGgTqsEq18p1KqoC","http://producerj.io",1]' -p producer.j@active -p infrasys@active

$INFRA_CLI push action infrasys authproducer '["producer.f"]' -p infrasys@active
$INFRA_CLI push action infrasys authproducer '["producer.g"]' -p infrasys@active
$INFRA_CLI push action infrasys authproducer '["producer.h"]' -p infrasys@active
$INFRA_CLI push action infrasys authproducer '["producer.i"]' -p infrasys@active
$INFRA_CLI push action infrasys authproducer '["producer.j"]' -p infrasys@active
```

Setup Initial Transaction Fees
---
```bash
// yx.system
$INFRA_CLI push action sys.txfee settxfee '[ "tf.newacc", "1000.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.regprod", "3000000.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.regsysdep", "2000000.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.regidauth", "2000000.00 DKRW" ]' -p infrasys@active

// yx.ntoken
$INFRA_CLI push action sys.txfee settxfee '[ "tf.nissue", "0.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.nredeem", "1000.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.transfer", "100.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.ntransfer", "200.00 DKRW" ]' -p infrasys@active

// yx.token
$INFRA_CLI push action sys.txfee settxfee '[ "tf.tcreate", "10000.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.tissue", "500.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.tredeem", "500.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.ttransfer", "100.00 DKRW" ]' -p infrasys@active

// yx.dcontract
$INFRA_CLI push action sys.txfee settxfee '[ "tf.dccreate", "500.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.dcaddsign", "100.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.dcsign", "300.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.dcupadd", "50.00 DKRW" ]' -p infrasys@active
$INFRA_CLI push action sys.txfee settxfee '[ "tf.dcremove", "0.00 DKRW" ]' -p infrasys@active

$INFRA_CLI get table -l 100 sys.txfee sys.txfee txfees
$INFRA_CLI get table -L tf.transfer -l 1 sys.txfee sys.txfee txfees
```

Resign "infrasys" delegating authority to "sys.prods"
---
```bash
$INFRA_CLI get account infrasys
$INFRA_CLI push action infrasys updateauth '{"account":"infrasys","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"sys.prods","permission":"active"}}]}}' -p infrasys@owner
$INFRA_CLI push action infrasys updateauth '{"account":"infrasys","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"sys.prods","permission":"active"}}]}}' -p infrasys@active
$INFRA_CLI get account infrasys

$INFRA_CLI get account sys.prods
permissions: 
     owner     1:    
        active     4:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
           prod.major     3:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
              prod.minor     2:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
```

Resign "sys.msig" delegating authority to "infrasys"
---
```bash
$INFRA_CLI get account sys.msig
$INFRA_CLI push action infrasys updateauth '{"account":"sys.msig","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p sys.msig@owner
$INFRA_CLI push action infrasys updateauth '{"account":"sys.msig","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p sys.msig@active
$INFRA_CLI get account sys.msig
```

Resign System Contract Accounts delegating authority to "infrasys"
---
```bash
$INFRA_CLI get account sys.txfee
$INFRA_CLI push action infrasys updateauth '{"account":"sys.txfee","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p sys.txfee@owner
$INFRA_CLI push action infrasys updateauth '{"account":"sys.txfee","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p sys.txfee@active
$INFRA_CLI get account sys.txfee

$INFRA_CLI get account sys.identity
$INFRA_CLI push action infrasys updateauth '{"account":"sys.identity","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p sys.identity@owner
$INFRA_CLI push action infrasys updateauth '{"account":"sys.identity","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p sys.identity@active
$INFRA_CLI get account sys.identity

$INFRA_CLI get account yx.ntoken
$INFRA_CLI push action infrasys updateauth '{"account":"yx.ntoken","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.ntoken@owner
$INFRA_CLI push action infrasys updateauth '{"account":"yx.ntoken","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.ntoken@active
$INFRA_CLI get account yx.ntoken

$INFRA_CLI get account yx.token
$INFRA_CLI push action infrasys updateauth '{"account":"yx.token","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.token@owner
$INFRA_CLI push action infrasys updateauth '{"account":"yx.token","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.token@active
$INFRA_CLI get account yx.token

$INFRA_CLI get account yx.dcontract
$INFRA_CLI push action infrasys updateauth '{"account":"yx.dcontract","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.dcontract@owner
$INFRA_CLI push action infrasys updateauth '{"account":"yx.dcontract","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.dcontract@active
$INFRA_CLI get account yx.dcontract

$INFRA_CLI get account yx.nft
$INFRA_CLI push action infrasys updateauth '{"account":"yx.nft","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.nft@owner
$INFRA_CLI push action infrasys updateauth '{"account":"yx.nft","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.nft@active
$INFRA_CLI get account yx.nft

$INFRA_CLI get account yx.nftex
$INFRA_CLI push action infrasys updateauth '{"account":"yx.nftex","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.nftex@owner
$INFRA_CLI push action infrasys updateauth '{"account":"yx.nftex","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.nftex@active
$INFRA_CLI get account yx.nftex

$INFRA_CLI get account yx.escrow
$INFRA_CLI push action infrasys updateauth '{"account":"yx.escrow","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.escrow@owner
$INFRA_CLI push action infrasys updateauth '{"account":"yx.escrow","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"infrasys","permission":"active"}}]}}' -p yx.escrow@active
$INFRA_CLI get account yx.escrow
```

Unauthorizing Block Producer
---
```bash
$INFRA_CLI push action -j -d infrasys rmvproducer '["producer.e"]' -p infrasys@active
#$INFRA_CLI push action infrasys rmvproducer '["producer.e"]' -p infrasys@active
#$INFRA_CLI push action infrasys rmvproducer '["producer.d"]' -p infrasys@active
$INFRA_CLI get account sys.prods
```

Create New User Accounts
---

```bash
$INFRA_CLI create key --to-console
Private key: YPV_5KTdP4VyuhZ1ba3sX8JvHjKXgcTFUA75QHf92nrGYQX4coVJRAW
Public key: YOS7y8FkTHo58ZvrMa6yrTpo3D2Y2AA4dMAZzQ8aZY6R6geZSDZ75
$INFRA_CLI wallet import --private-key YPV_5KTdP4VyuhZ1ba3sX8JvHjKXgcTFUA75QHf92nrGYQX4coVJRAW

$INFRA_CLI create key --to-console
Private key: YPV_5KDKMejP2AZFGCMxLh67t7417coEgUu7PfQZk7ufuYA4wTBiNC4
Public key: YOS8ZFsmfpevP8pKPMXuQMNB7P9KJoKzZwwzu2uww9VrRx7E7NUQj
$INFRA_CLI wallet import --private-key YPV_5KDKMejP2AZFGCMxLh67t7417coEgUu7PfQZk7ufuYA4wTBiNC4

$INFRA_CLI create key --to-console
Private key: YPV_5Jj4ryu8TY1cVhiLvPkBHKiwMAXV18WqJM97h8hfsBUt5sMJE3r
Public key: YOS5ubmvsnjHviACtfc9SwGbY7SprqTu1P5GQeDfwfZmhCq6aR7GH
$INFRA_CLI wallet import --private-key YPV_5Jj4ryu8TY1cVhiLvPkBHKiwMAXV18WqJM97h8hfsBUt5sMJE3r

$INFRA_CLI create key --to-console
Private key: YPV_5KWoErpjc9yQ2HF6rLuijLFgmAqVEoDLGWmDQAT6NM5DCgcSgo1
Public key: YOS6vdcL347XXzpxSdnLUKpNwmA3KhYagcFqnZ3rNfKp96hzFxyit
$INFRA_CLI wallet import --private-key YPV_5KWoErpjc9yQ2HF6rLuijLFgmAqVEoDLGWmDQAT6NM5DCgcSgo1

$INFRA_CLI create key --to-console
Private key: YPV_5JP9oWWsMaTEBcw1Y7Jn8JzxQBq51FDcGVGqCi5gpMDyKqio26j
Public key: YOS6ibAshrW7QmXeM5gurexmw6ijwM9d1BYS1J6Y1kevAFf7PcLNh
$INFRA_CLI wallet import --private-key YPV_5JP9oWWsMaTEBcw1Y7Jn8JzxQBq51FDcGVGqCi5gpMDyKqio26j

// Identity Authority can create accounts having 12 chracters without paying 'tf.newacc' transaction fee
$INFRA_CLI create account idauth1 useraccount1 YOS7y8FkTHo58ZvrMa6yrTpo3D2Y2AA4dMAZzQ8aZY6R6geZSDZ75 -p idauth1@active
$INFRA_CLI create account idauth1 useraccount2 YOS8ZFsmfpevP8pKPMXuQMNB7P9KJoKzZwwzu2uww9VrRx7E7NUQj -p idauth1@active
$INFRA_CLI create account idauth1 useraccount3 YOS5ubmvsnjHviACtfc9SwGbY7SprqTu1P5GQeDfwfZmhCq6aR7GH -p idauth1@active

// Only system account can create accounts having less than 12 characters
$INFRA_CLI create account infrasys com YOS6vdcL347XXzpxSdnLUKpNwmA3KhYagcFqnZ3rNfKp96hzFxyit -p infrasys@active

// Only suffix account can create accounts having arbitrary prefix with the same suffix name
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"com\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"1f32i7t23\"}" -p idauth1@active
$INFRA_CLI push action yx.ntoken nissue '["com",{"amount":"5000000.00 DKRW","issuer":"sysdepo1"},"nissue com"]' -p sysdepo1@active
executed transaction: c9fb1c0cf6c6c9d73e377bcfd405802e129f85cf86a68a57bc15f91d252e1924  136 bytes  3565 us
#     yx.ntoken <= yx.ntoken::nissue            {"to":"com","token":{"amount":"5000000.00 DKRW","issuer":"sysdepo1"},"memo":"nissue com"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","token":{"amount":"5000000.00 DKRW","issuer":"sysdepo1"},"memo":"nis...
#      sysdepo1 <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","token":{"amount":"5000000.00 DKRW","issuer":"sysdepo1"},"memo":"nis...
#           com <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","token":{"amount":"5000000.00 DKRW","issuer":"sysdepo1"},"memo":"nis...
$INFRA_CLI create account com acquire.com YOS6ibAshrW7QmXeM5gurexmw6ijwM9d1BYS1J6Y1kevAFf7PcLNh -p com@active
executed transaction: 9268f4e3b7e44caaf1a7c3528ff44ce6e9cfdb8d6dfcf33aa2cc44d7a6c5604a  200 bytes  4900 us
#      infrasys <= infrasys::newaccount         {"creator":"com","name":"acquire.com","owner":{"threshold":1,"keys":[{"key":"YOS5UbAi7wTM1wpFNp81bgh...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"com","token":"1000.00 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"com","token":{"amount":"1000.00 DKRW","issuer":"sysdepo1"}}
#           com <= yx.ntoken::feetransfer       {"payer":"com","token":{"amount":"1000.00 DKRW","issuer":"sysdepo1"}}
#     sys.txfee <= yx.ntoken::feetransfer       {"payer":"com","token":{"amount":"1000.00 DKRW","issuer":"sysdepo1"}}

$INFRA_CLI get table yx.ntoken sysdepo1 ntaccounts
$INFRA_CLI get table yx.ntoken sysdepo1 ntstats
$INFRA_CLI get table yx.ntoken com ntaccounts
$INFRA_CLI get table yx.ntoken sys.txfee ntaccounts

$INFRA_CLI get table -L com -l 1 sys.identity sys.identity identity
``` 

Managing Account Identity Info (including KYC)
---
```bash
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"useraccount1\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 0111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth1@active
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"useraccount2\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"23uyiuye\"}" -p idauth1@active
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"useraccount3\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"vewv23r3\"}" -p idauth1@active

$INFRA_CLI get table -L useraccount1 -l 1 sys.identity sys.identity identity

// update user account type
$INFRA_CLI push action sys.identity settype "{\"account\":\"useraccount1\", \"type\":$(echo 'ibase=2; 11111111' | bc)}" -p idauth1@active

// update kyc status
$INFRA_CLI push action sys.identity setkyc "{\"account\":\"useraccount1\", \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p idauth1@active

// update account state (e.g. blacklist account)
$INFRA_CLI push action sys.identity setstate "{\"account\":\"useraccount1\", \"state\":$(echo 'ibase=2; 0001' | bc)}" -p idauth1@active

// update identity-authority specific data
$INFRA_CLI push action sys.identity setdata "{\"account\":\"useraccount1\", \"data\":\"23fiuygy3\"}" -p idauth1@active
```

Native Token Issue / Transfer
---

```bash

$INFRA_CLI push action yx.ntoken nissue '["useraccount2",{"amount":"100000.00 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active
executed transaction: 5d60dada4af867696f4bf43842c9820218da2fbb8166d2c3d83f32bd7c07ad89  136 bytes  3767 us
#     yx.ntoken <= yx.ntoken::nissue            {"to":"useraccount2","token":{"amount":"100000.00 DKRW","issuer":"sysdepo1"},"memo":"nissue test"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"useraccount2","token":{"amount":"100000.00 DKRW","issuer":"sysdepo1"},"me...
#      sysdepo1 <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"useraccount2","token":{"amount":"100000.00 DKRW","issuer":"sysdepo1"},"me...
#  useraccount2 <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"useraccount2","token":{"amount":"100000.00 DKRW","issuer":"sysdepo1"},"me..

$INFRA_CLI push action yx.ntoken nissue '["useraccount3",{"amount":"100000.00 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active

// unable to issue to blacklisted account
//$INFRA_CLI push action yx.ntoken nissue '["useraccount1",{"amount":"100000.00 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active


$INFRA_CLI push action yx.ntoken transfer '[ "useraccount2", "useraccount3", "10000.00 DKRW", "memo" ]' -p useraccount2
executed transaction: 0bf46f27f6689de95d875e2ad9f6f5428b139f377a5790313e618de371150dc5  136 bytes  7379 us
#     yx.ntoken <= yx.ntoken::transfer          {"from":"useraccount2","to":"useraccount3","amount":"10000.00 DKRW","memo":"memo"}
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"useraccount2","token":"100.00 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.00 DKRW","issuer":"sysdepo1"}}
#  useraccount2 <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.00 DKRW","issuer":"sysdepo1"}}
#     sys.txfee <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.00 DKRW","issuer":"sysdepo1"}}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.00 DKRW","issuer":"sysdepo1"},...
#  useraccount2 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.00 DKRW","issuer":"sysdepo1"},...
#  useraccount3 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.00 DKRW","issuer":"sysdepo1"},...

$INFRA_CLI push action yx.ntoken ntransfer '[ "useraccount2", "useraccount3", {"amount":"10000.00 DKRW","issuer":"sysdepo1"}, "memo" ]' -p useraccount2
executed transaction: f443593e42ff28e1515a26d87d9b0b476849954ceb77ef1057e8dd07a857cea8  144 bytes  4947 us
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.00 DKRW","issuer":"sysdepo1"},...
#  useraccount2 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.00 DKRW","issuer":"sysdepo1"},...
#  useraccount3 <= yx.ntoken::ntransfer         {"from":"useraccount2","to":"useraccount3","token":{"amount":"10000.00 DKRW","issuer":"sysdepo1"},...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"useraccount2","token":"200.00 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"200.00 DKRW","issuer":"sysdepo1"}}
#  useraccount2 <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"200.00 DKRW","issuer":"sysdepo1"}}
#     sys.txfee <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"200.00 DKRW","issuer":"sysdepo1"}}

$INFRA_CLI push action yx.ntoken wptransfer '[ "useraccount3", "useraccount2", "10000.00 DKRW", "useraccount2", "memo" ]' -p useraccount3 -p useraccount2
executed transaction: 8f7e234e837021535c8eb9302ed3052307e972ac9da001de7db35c80714f846f  168 bytes  7554 us
#     yx.ntoken <= yx.ntoken::wptransfer        {"from":"useraccount3","to":"useraccount2","amount":"10000.00 DKRW","payer":"useraccount2","memo":...
#     yx.ntoken <= yx.ntoken::payfee            {"payer":"useraccount2","token":"100.00 DKRW"}
#     yx.ntoken <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.00 DKRW","issuer":"sysdepo1"}}
#  useraccount2 <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.00 DKRW","issuer":"sysdepo1"}}
#     sys.txfee <= yx.ntoken::feetransfer       {"payer":"useraccount2","token":{"amount":"100.00 DKRW","issuer":"sysdepo1"}}
#     yx.ntoken <= yx.ntoken::wpntransfer       {"from":"useraccount3","to":"useraccount2","token":{"amount":"10000.00 DKRW","issuer":"sysdepo1"},...
#  useraccount3 <= yx.ntoken::wpntransfer       {"from":"useraccount3","to":"useraccount2","token":{"amount":"10000.00 DKRW","issuer":"sysdepo1"},...
#  useraccount2 <= yx.ntoken::wpntransfer       {"from":"useraccount3","to":"useraccount2","token":{"amount":"10000.00 DKRW","issuer":"sysdepo1"},...

$INFRA_CLI get table yx.ntoken sysdepo1 ntstats
$INFRA_CLI get table yx.ntoken useraccount1 ntaccounts
$INFRA_CLI get table yx.ntoken useraccount2 ntaccounts
$INFRA_CLI get table yx.ntoken useraccount3 ntaccounts
$INFRA_CLI get table yx.ntoken sys.txfee ntaccounts
$INFRA_CLI get table yx.ntoken sysdepo1 ntaccounts

```

Claim Producer Rewards
---
---
```bash
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"producer.c\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"bp\"}" -p idauth1@active

$INFRA_CLI push action infrasys claimrewards '["producer.c"]' -p producer.c@active
executed transaction: 96c1b0e367c4bbc85801242dc5a8ba33de274c4aee6ce898ac0b9008c8ca655c  104 bytes  4891 us
#      infrasys <= infrasys::claimrewards       {"owner":"producer.c"}
#     yx.ntoken <= yx.ntoken::transfer          {"from":"sys.txfee","to":"producer.c","amount":"247.82 DKRW","memo":""}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"sys.txfee","to":"producer.c","token":{"amount":"247.82 DKRW","issuer":"sysdepo1"},"memo":"...
#     sys.txfee <= yx.ntoken::ntransfer         {"from":"sys.txfee","to":"producer.c","token":{"amount":"247.82 DKRW","issuer":"sysdepo1"},"memo":"...
#    producer.c <= yx.ntoken::ntransfer         {"from":"sys.txfee","to":"producer.c","token":{"amount":"247.82 DKRW","issuer":"sysdepo1"},"memo":"...

$INFRA_CLI get table infrasys infrasys producers

$INFRA_CLI get table yx.ntoken sys.txfee ntaccounts
$INFRA_CLI get table yx.ntoken producer.c ntaccounts

```

Transaction Vote 
---

```bash
$INFRA_CLI push action --help
Push a transaction with a single action
Usage: infra-cli push action [OPTIONS] account action data

Options:
...
  -v,--trx-vote TEXT          transaction vote target account, Transaction-as-a-Vote(TaaV) for InfraBlockchain Proof-of-Transaction(PoT)
...
  
$INFRA_CLI push action yx.ntoken transfer '[ "useraccount3", "useraccount2", "10000.00 DKRW", "memo" ]' -p useraccount3 -v producer.f
$INFRA_CLI get table infrasys infrasys producers
```

Synchronize action data 
---
```bash
$INFRA_CLI_TESTNET get actions yx.ntoken 0 9
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#    0   2018-08-17T06:32:57.500         infrasys::setcode => infrasys      c5c43a79... {"account":"yx.ntoken","vmtype":0,"vmversion":0,"code":"0061...
#    1   2018-08-17T06:32:57.500          infrasys::setabi => infrasys      c5c43a79... {"account":"yx.ntoken","abi":"0e656f73696f3a3a6162692f312e30...
#    2   2018-08-17T06:32:57.500     yx.ntoken::setkycrule => yx.ntoken     9b066054... {"type":0,"kyc":15}...
#    3   2018-08-17T06:32:57.500     yx.ntoken::setkycrule => yx.ntoken     1ebd6b37... {"type":1,"kyc":15}...
#    4   2018-08-17T06:36:10.000      infrasys::updateauth => infrasys      dc14ff64... {"account":"yx.ntoken","permission":"owner","parent":"","aut...
#    5   2018-08-17T06:36:10.000      infrasys::updateauth => infrasys      d153eff4... {"account":"yx.ntoken","permission":"active","parent":"owner...
#    6   2018-08-17T06:36:11.500         yx.ntoken::nissue => yx.ntoken     eb2f8abc... {"to":"com","token":{"amount":"5000000.00 DKRW","issuer":"...
#    7   2018-08-17T06:36:11.500      yx.ntoken::ntransfer => yx.ntoken     eb2f8abc... {"from":"sysdepo1","to":"com","token":{"amount":"5000000.000...
#    8   2018-08-17T06:36:11.500         yx.ntoken::payfee => yx.ntoken     d59e98ad... {"payer":"com","token":{"amount":"1000.00 DKRW","issuer":"...
#    9   2018-08-17T06:36:14.000         yx.ntoken::nissue => yx.ntoken     e91aaf07... {"to":"useraccount2","token":{"amount":"100000.00 DKRW","i...

$INFRA_CLI_TESTNET get actions yx.ntoken 10 9
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#   10   2018-08-17T06:36:14.000      yx.ntoken::ntransfer => yx.ntoken     e91aaf07... {"from":"sysdepo1","to":"useraccount2","token":{"amount":"10...
#   11   2018-08-17T06:36:14.000         yx.ntoken::nissue => yx.ntoken     ad9e02d1... {"to":"useraccount3","token":{"amount":"100000.00 DKRW","i...
#   12   2018-08-17T06:36:14.000      yx.ntoken::ntransfer => yx.ntoken     ad9e02d1... {"from":"sysdepo1","to":"useraccount3","token":{"amount":"10...
#   13   2018-08-17T06:36:14.000       yx.ntoken::transfer => yx.ntoken     eadb558f... {"from":"useraccount2","to":"useraccount3","amount":"10000.0...
#   14   2018-08-17T06:36:14.000      yx.ntoken::ntransfer => yx.ntoken     eadb558f... {"from":"useraccount2","to":"useraccount3","token":{"amount"...
#   15   2018-08-17T06:36:14.000         yx.ntoken::payfee => yx.ntoken     eadb558f... {"payer":"useraccount2","token":{"amount":"100.00 DKRW","i...
#   16   2018-08-17T06:36:14.000      yx.ntoken::ntransfer => yx.ntoken     c15fabd8... {"from":"useraccount2","to":"useraccount3","token":{"amount"...
#   17   2018-08-17T06:36:14.000         yx.ntoken::payfee => yx.ntoken     c15fabd8... {"payer":"useraccount2","token":{"amount":"200.00 DKRW","i...
#   18   2018-08-17T06:36:14.000     yx.ntoken::wptransfer => yx.ntoken     12e702db... {"from":"useraccount3","to":"useraccount2","amount":"10000.0...
#   19   2018-08-17T06:36:14.000    yx.ntoken::wpntransfer => yx.ntoken     12e702db... {"from":"useraccount3","to":"useraccount2","token":{"amount"...


$INFRA_CLI_TESTNET get actions yx.ntoken -1 -1
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#  seq  when                              contract::action => receiver      trx id...   args
================================================================================================================
#   31   2018-08-17T06:36:19.500         yx.ntoken::payfee => yx.ntoken     a5b8539d... {"payer":"useraccount3","token":{"amount":"100.00 DKRW","i...
```

Get transaction data
---

```bash
$INFRA_CLI_TESTNET get transaction e91aaf079a73304060d98ca69f3e3010c26127212b7904cb9939d74964453296
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
            "YSG_K1_K8PtT9kUxbtkMLsj7NJDMRHA5vDTqjYLt3n6JztttQUAtFppvU3QCWTWiHLMHmecL8pS4WSAizcL1N2zkPwbwVyPv4Nru6"
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
              "amount": "100000.00 DKRW",
              "issuer": "sysdepo1"
            },
            "memo": "nissue test"
          },
          "hex_data": "20f2d414217315d600ca9a3b0000000004444b5257000000000000815695b0c70b6e69737375652074657374"
        }
      ],
      "transaction_extensions": [],
      "signatures": [
        "YSG_K1_K8PtT9kUxbtkMLsj7NJDMRHA5vDTqjYLt3n6JztttQUAtFppvU3QCWTWiHLMHmecL8pS4WSAizcL1N2zkPwbwVyPv4Nru6"
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
            "amount": "100000.00 DKRW",
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
                "infrasys",
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
                "actor": "infrasys",
                "permission": "active"
              }
            ],
            "data": {
              "from": "sysdepo1",
              "to": "useraccount2",
              "token": {
                "amount": "100000.00 DKRW",
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
                    "infrasys",
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
                    "actor": "infrasys",
                    "permission": "active"
                  }
                ],
                "data": {
                  "from": "sysdepo1",
                  "to": "useraccount2",
                  "token": {
                    "amount": "100000.00 DKRW",
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
                    "infrasys",
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
                    "actor": "infrasys",
                    "permission": "active"
                  }
                ],
                "data": {
                  "from": "sysdepo1",
                  "to": "useraccount2",
                  "token": {
                    "amount": "100000.00 DKRW",
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
            "infrasys",
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
            "actor": "infrasys",
            "permission": "active"
          }
        ],
        "data": {
          "from": "sysdepo1",
          "to": "useraccount2",
          "token": {
            "amount": "100000.00 DKRW",
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
                "infrasys",
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
                "actor": "infrasys",
                "permission": "active"
              }
            ],
            "data": {
              "from": "sysdepo1",
              "to": "useraccount2",
              "token": {
                "amount": "100000.00 DKRW",
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
                "infrasys",
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
                "actor": "infrasys",
                "permission": "active"
              }
            ],
            "data": {
              "from": "sysdepo1",
              "to": "useraccount2",
              "token": {
                "amount": "100000.00 DKRW",
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
            "infrasys",
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
            "actor": "infrasys",
            "permission": "active"
          }
        ],
        "data": {
          "from": "sysdepo1",
          "to": "useraccount2",
          "token": {
            "amount": "100000.00 DKRW",
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
            "infrasys",
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
            "actor": "infrasys",
            "permission": "active"
          }
        ],
        "data": {
          "from": "sysdepo1",
          "to": "useraccount2",
          "token": {
            "amount": "100000.00 DKRW",
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