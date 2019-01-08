#!/usr/bin/env bash
# chmod +x ./reset_std_token_yosemite_testnet_on_server_side.sh

YOSEMITE_NODE_BIN_NAME=yosemite
YOSEMITE_CLI_BIN_NAME=clyos
YOSEMITE_KEYD_BIN_NAME=keyos
YOSEMITE_HOME=/mnt/yosemite-public-blockchain-git
YOSEMITE_NODE=$YOSEMITE_HOME/build/programs/$YOSEMITE_NODE_BIN_NAME/$YOSEMITE_NODE_BIN_NAME
YOSEMITE_NODE_LOG_FILE=/mnt/$YOSEMITE_NODE_BIN_NAME.log
YOSEMITE_CLI="$YOSEMITE_HOME/build/programs/$YOSEMITE_CLI_BIN_NAME/$YOSEMITE_CLI_BIN_NAME --wallet-url http://127.0.0.1:8900/"
YOSEMITE_CLI_TESTNET="$YOSEMITE_CLI -u http://testnet-sentinel.yosemitelabs.org:8888"
YOSEMITE_CLI_TESTNET_V1="$YOSEMITE_CLI -u http://testnet.yosemitelabs.org:8888"
YOSEMITE_KEYD=$YOSEMITE_HOME/build/programs/$YOSEMITE_KEYD_BIN_NAME/$YOSEMITE_KEYD_BIN_NAME
YOSEMITE_KEYD_LOG_FILE=/mnt/$YOSEMITE_KEYD_BIN_NAME.log
YOSEMITE_KEYD_WALLET_PASSWORD=PW5Jrpn9S5ygoA9r2bv47rv9gH2jVjntdyHWKL4QdoKVFRz6U17EM
YOSEMITE_NODE_CONFIG=$YOSEMITE_HOME/yosemite_config/config_yosemite_testnet_boot.ini
YOSEMITE_NODE_GENESIS_JSON=$YOSEMITE_HOME/yosemite_config/genesis_yosemite_testnet.json
YOSEMITE_NODE_DATA_DIR=/mnt/yosemite_node_data
YOSEMITE_DEV_WALLET_DIR=/mnt/yosemite_dev_wallet
YOSEMITE_CONTRACTS_DIR=$YOSEMITE_HOME/build/contracts
YOSEMITE_MONGOD=/home/ubuntu/opt/mongodb/bin/mongod
YOSEMITE_MONGODB_CONFIG=/home/ubuntu/opt/mongodb/mongod.conf
YOSEMITE_MONGODB_DATA_DIR=/mnt/mongodb

{ set +x; } 2>/dev/null
red=`tput setaf 1`
green=`tput setaf 2`
magenta=`tput setaf 6`
reset=`tput sgr0`
set -x

{ set +x; } 2>/dev/null
echo "${red}[Resetting YOSEMITE Testnet]${reset}"
echo
echo "${green}YOSEMITE_HOME${reset}=${red}$YOSEMITE_HOME${reset}"
echo "${green}YOSEMITE_NODE${reset}=${red}$YOSEMITE_NODE${reset}"
echo "${green}YOSEMITE_NODE_LOG_FILE${reset}=${red}$YOSEMITE_NODE_LOG_FILE${reset}"
echo "${green}YOSEMITE_CLI${reset}=${red}$YOSEMITE_CLI${reset}"
echo "${green}YOSEMITE_CLI_TESTNET${reset}=${red}$YOSEMITE_CLI_TESTNET${reset}"
echo "${green}YOSEMITE_KEYD${reset}=${red}$YOSEMITE_KEYD${reset}"
echo "${green}YOSEMITE_KEYD_LOG_FILE${reset}=${red}$YOSEMITE_KEYD_LOG_FILE${reset}"
echo "${green}YOSEMITE_KEYD_WALLET_PASSWORD${reset}=${red}$YOSEMITE_KEYD_WALLET_PASSWORD${reset}"
echo "${green}YOSEMITE_NODE_CONFIG${reset}=${red}$YOSEMITE_NODE_CONFIG${reset}"
echo "${green}YOSEMITE_NODE_GENESIS_JSON${reset}=${red}$YOSEMITE_NODE_GENESIS_JSON${reset}"
echo "${green}YOSEMITE_NODE_DATA_DIR${reset}=${red}$YOSEMITE_NODE_DATA_DIR${reset}"
echo "${green}YOSEMITE_DEV_WALLET_DIR${reset}=${red}$YOSEMITE_DEV_WALLET_DIR${reset}"
echo "${green}YOSEMITE_CONTRACTS_DIR${reset}=${red}$YOSEMITE_CONTRACTS_DIR${reset}"
echo "${green}YOSEMITE_MONGOD${reset}=${red}$YOSEMITE_MONGOD${reset}"
echo "${green}YOSEMITE_MONGODB_CONFIG${reset}=${red}$YOSEMITE_MONGODB_CONFIG${reset}"
echo "${green}YOSEMITE_MONGODB_DATA_DIR${reset}=${red}$YOSEMITE_MONGODB_DATA_DIR${reset}"
echo
set -x

{ set +x; } 2>/dev/null
echo "${red}Really want to reset Testnet node data?${reset}"
echo "${red}[WARNING] all data including yosemite node data and mongo db data will be deleted permanately.${reset}"
echo "write YES to proceed reset process."
read USER_CONFIRM_TO_PROCEED
if [ "$USER_CONFIRM_TO_PROCEED" != "YES" ]; then
  exit 1
fi
set -x

print_section_title() {
  { set +x; } 2>/dev/null
  echo
  echo "${green}[$1]${reset}"
  echo
  set -x
}

{ print_section_title "Reset yosemite node data"; } 2>/dev/null

pgrep $YOSEMITE_NODE_BIN_NAME
pkill -SIGINT $YOSEMITE_NODE_BIN_NAME
sleep 5

du -sh $YOSEMITE_NODE_DATA_DIR
rm -rf $YOSEMITE_NODE_DATA_DIR
mkdir $YOSEMITE_NODE_DATA_DIR


{ print_section_title "Reset mongo db data"; } 2>/dev/null

pgrep mongod
pkill -SIGINT mongod
sleep 5

du -sh $YOSEMITE_MONGODB_DATA_DIR
rm -rf $YOSEMITE_MONGODB_DATA_DIR
mkdir $YOSEMITE_MONGODB_DATA_DIR
mkdir $YOSEMITE_MONGODB_DATA_DIR/data
mkdir $YOSEMITE_MONGODB_DATA_DIR/log

$YOSEMITE_MONGOD -f $YOSEMITE_MONGODB_CONFIG --logpath $YOSEMITE_MONGODB_DATA_DIR/log/mongodb.log --dbpath $YOSEMITE_MONGODB_DATA_DIR/data --bind_ip 127.0.0.1 --port 27017 --fork

sleep 5


{ print_section_title "Restart key daemon"; } 2>/dev/null

pgrep $YOSEMITE_KEYD_BIN_NAME
pkill -SIGINT $YOSEMITE_KEYD_BIN_NAME
sleep 5
nohup $YOSEMITE_KEYD --unlock-timeout 999999999 --http-server-address 127.0.0.1:8900 --wallet-dir $YOSEMITE_DEV_WALLET_DIR > $YOSEMITE_KEYD_LOG_FILE 2>&1&
sleep 10
tail -n 300 $YOSEMITE_KEYD_LOG_FILE

#$YOSEMITE_CLI wallet create --to-console
#Creating wallet: default
#Save password to use in the future to unlock this wallet.
#Without password imported keys will not be retrievable.
#"PW5Jrpn9S5ygoA9r2bv47rv9gH2jVjntdyHWKL4QdoKVFRz6U17EM"
$YOSEMITE_CLI wallet open
$YOSEMITE_CLI wallet unlock --password $YOSEMITE_KEYD_WALLET_PASSWORD

# yosemite initial key
#$YOSEMITE_CLI create key --to-console
$YOSEMITE_CLI wallet import --private-key YPV_5J7MF5bWVzjLczmVFMwiRjD5TVBG1o8UELy4jkaikXETbaMHZSb


{ print_section_title "Start yosemite node from genesis"; } 2>/dev/null

nohup $YOSEMITE_NODE --config $YOSEMITE_NODE_CONFIG --genesis-json $YOSEMITE_NODE_GENESIS_JSON --data-dir $YOSEMITE_NODE_DATA_DIR > $YOSEMITE_NODE_LOG_FILE 2>&1&
sleep 10
tail -n 300 $YOSEMITE_NODE_LOG_FILE
pgrep $YOSEMITE_NODE_BIN_NAME
pkill -SIGINT $YOSEMITE_NODE_BIN_NAME
sleep 5
$YOSEMITE_NODE --print-genesis-json --config $YOSEMITE_NODE_CONFIG --data-dir $YOSEMITE_NODE_DATA_DIR

nohup $YOSEMITE_NODE --config $YOSEMITE_NODE_CONFIG --data-dir $YOSEMITE_NODE_DATA_DIR > $YOSEMITE_NODE_LOG_FILE 2>&1&
sleep 10
tail -n 300 $YOSEMITE_NODE_LOG_FILE


{ print_section_title "Create System Accounts"; } 2>/dev/null

$YOSEMITE_CLI create account yosemite yx.tokenabi YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3 -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite yx.txfee YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3 -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite yx.msig YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3 -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite yx.identity YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3 -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite yx.dcontract YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3 -p yosemite@active --txfee-payer yosemite

{ print_section_title "Create initial Identity Authority Account"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5K5Qe2ndgcfhWqdUjHnfhvCKVqCE82Rih6JrNQuuVJBcathAx9m
$YOSEMITE_CLI create account yosemite idauth.a YOS6EfGUaA5MNLH1GiHd64DcDr3HMgY1AM3WR1vdHKaah9Z4cWPZq -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI wallet import --private-key YPV_5JRzPhyg9jY4thDJ1rLsSr4zdLLoA7UEMQRw3vJFGmGKTZF5Kbx
$YOSEMITE_CLI create account yosemite idauth.b YOS5VvWLCqXL3AQyuXZfUTnEiWuhJNV6Nq3YbbS7Z98cN7GiRo9LR -p yosemite@active --txfee-payer yosemite



{ print_section_title "Create initial System Token Account"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5J8WZRwfYVzsH8i3LaRQLGorVD788RFYqALEsLmeK8a6Kq6re2W
$YOSEMITE_CLI create account yosemite systoken.a YOS6o8hwGio5V2LgBxFMZi4bdVG5AvcHeFiMpaREKuZ29BwXpNTpH -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI wallet import --private-key YPV_5K5MBizJ6HuG1kdvQTeMnpKjpFSZHm64Nj3X6MvJR6AnTF8mHkx
$YOSEMITE_CLI create account yosemite systoken.b YOS61njg9Zs9oQmAmwLnrcVBrniWKshKAhoQJJqNVVnYXCPSRKLTE -p yosemite@active --txfee-payer yosemite


{ print_section_title "Install YOSEMITE System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yosemite $YOSEMITE_CONTRACTS_DIR/yx.system/ -p yosemite@active --txfee-payer yosemite
sleep 2


{ print_section_title "Install Standard Token ABI Interface System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.tokenabi $YOSEMITE_CONTRACTS_DIR/yx.tokenabi/ -p yx.tokenabi@active --txfee-payer yosemite
sleep 2


{ print_section_title "Install Multi-sig System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.msig $YOSEMITE_CONTRACTS_DIR/eosio.msig/ -p yx.msig@active --txfee-payer yosemite
sleep 2
$YOSEMITE_CLI push action yosemite setpriv '{"account":"yx.msig","is_priv":1}' -p yosemite@active --txfee-payer yosemite
sleep 2


{ print_section_title "Install Identity System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.identity $YOSEMITE_CONTRACTS_DIR/yx.identity/ -p yx.identity@active --txfee-payer yosemite
sleep 2

{ print_section_title "Install DigitalContract Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.dcontract $YOSEMITE_CONTRACTS_DIR/yx.dcontract/ -p yx.dcontract@active --txfee-payer yosemite
sleep 2

{ print_section_title "Register Initial Identity Authorities"; } 2>/dev/null

$YOSEMITE_CLI push action yosemite regidauth '{"identity_authority":"idauth.a","url":"http://idauth-a.org","location":1}' -p idauth.a@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authidauth '{"identity_authority":"idauth.a"}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite regidauth '{"identity_authority":"idauth.b","url":"http://idauth-b.org","location":1}' -p idauth.b@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authidauth '{"identity_authority":"idauth.b"}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI get table yosemite yosemite idauthority

sleep 2


{ print_section_title "Querying the status of Identity Authorities and Block Producers"; } 2>/dev/null

$YOSEMITE_CLI get table yosemite yosemite global
$YOSEMITE_CLI get table yosemite yosemite idauthority
$YOSEMITE_CLI get table yosemite yosemite producers


{ print_section_title "Initial Block Producer Setup"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5HsXETBYTpZbgu99EDVmdihxCkVm3i1DU6PL6BQRXAxJnacsooh
$YOSEMITE_CLI wallet import --private-key YPV_5JpFP6dYxrNDGnGKuj6kGNovk5a2QK8V7c1pyp5b93DSjE2WDA3
$YOSEMITE_CLI wallet import --private-key YPV_5KcHTPMwDxJ9yr14f8PZXgsoVbNnez1e85d3UfvWjvNFR817M7d
$YOSEMITE_CLI wallet import --private-key YPV_5J93otpSC4UZXakAT1FFXndc2tUyPPYth7hkuFZ9UQpvtS9vXtZ
$YOSEMITE_CLI wallet import --private-key YPV_5KD33jEkkZ5K7GK4Sbp26YfDWT1vxyY4K931QFdoyBb2JXV4qSd
$YOSEMITE_CLI wallet import --private-key YPV_5JpAqiPZ5RrcKrTxXuGzEtSvyV4sC2E1tsxJGtCyXAREfoXLtVt
$YOSEMITE_CLI wallet import --private-key YPV_5JW4NzsCak4hJCkoq85WJi9gdf1upGnY4XNAaYtxTnw4za7f5Kg
$YOSEMITE_CLI wallet import --private-key YPV_5JZ3EDx6PmSYXRs8gLvxhr6ZHvMJnJdnW6jD4AB5KnKguWmomjh
$YOSEMITE_CLI wallet import --private-key YPV_5KQvzWLVeKxgYY6oNGcYDg2QRViTA255nBuqHvoC2yPwaxqxhWh
$YOSEMITE_CLI wallet import --private-key YPV_5Kgq6gkf2ybW86J7pk3HhFZvtGo9nU1AxukPptxYHLohpBGpiDm
sleep 1

$YOSEMITE_CLI create account yosemite producer.a YOS5Audoa4mpZaYhp7vwYVCUnsQCUVifftdPipvkfZ9qVggoYoHUn -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite producer.b YOS5aw9PzjxJCTi23FWtcB6Q8feMhfLg7Toh7PwGoWge4K4xNWQdm -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite producer.c YOS8cvC5FJozTTVfUVXZ4E4kz1eNsKoBsnG7J76Fw1gX1wstGoUWo -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite producer.d YOS6ig1G6hpk1Tzj1Ko8zfysATY4eqpb9znyEnx25zbkHscV6qHvy -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite producer.e YOS72LDKqDc2KvyN1XeEYhv7AbkMUYB8B3fJ55yMn4ZqLzeqxz3w1 -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite producer.f YOS5SzGAGCMznawLLY9xkpa4ta62CfTKu6di9AjZ9bWCBJ1pFu641 -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite producer.g YOS5t1fHFunR2rWq5z8NHPrxj1H4xG5Vq4bGKcH33yg1eZMCVPQRq -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite producer.h YOS5unphoov9UCG8AidDYv14fMJVRnHuihScASHbdFXHuDeDAS2s6 -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite producer.i YOS5KM5t7td26VGDSEHXPHoUHB1Md1NnGRbT9EkXDaBf5nyhw18is -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI create account yosemite producer.j YOS5oduMFs5Lrbb8ZEc11KtyoVqfUjvaRxbUsQGgTqsEq18p1KqoC -p yosemite@active --txfee-payer yosemite
sleep 1

$YOSEMITE_CLI push action yosemite regproducer '{"producer":"producer.a","producer_key":"YOS5Audoa4mpZaYhp7vwYVCUnsQCUVifftdPipvkfZ9qVggoYoHUn","url":"http://producera.io","location":1}' -p producer.a@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '{"producer":"producer.b","producer_key":"YOS5aw9PzjxJCTi23FWtcB6Q8feMhfLg7Toh7PwGoWge4K4xNWQdm","url":"http://producerb.io","location":1}' -p producer.b@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite authproducer '{"producer":"producer.a"}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '{"producer":"producer.b"}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite regproducer '{"producer":"producer.c","producer_key":"YOS8cvC5FJozTTVfUVXZ4E4kz1eNsKoBsnG7J76Fw1gX1wstGoUWo","url":"http://producerc.io","location":1}' -p producer.c@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '{"producer":"producer.d","producer_key":"YOS6ig1G6hpk1Tzj1Ko8zfysATY4eqpb9znyEnx25zbkHscV6qHvy","url":"http://producerd.io","location":1}' -p producer.d@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '{"producer":"producer.e","producer_key":"YOS72LDKqDc2KvyN1XeEYhv7AbkMUYB8B3fJ55yMn4ZqLzeqxz3w1","url":"http://producere.io","location":1}' -p producer.e@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite authproducer '{"producer":"producer.c"}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '{"producer":"producer.d"}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '{"producer":"producer.e"}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite regproducer '{"producer":"producer.f","producer_key":"YOS5SzGAGCMznawLLY9xkpa4ta62CfTKu6di9AjZ9bWCBJ1pFu641","url":"http://producerf.io","location":1}' -p producer.f@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '{"producer":"producer.g","producer_key":"YOS5t1fHFunR2rWq5z8NHPrxj1H4xG5Vq4bGKcH33yg1eZMCVPQRq","url":"http://producerg.io","location":1}' -p producer.g@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '{"producer":"producer.h","producer_key":"YOS5unphoov9UCG8AidDYv14fMJVRnHuihScASHbdFXHuDeDAS2s6","url":"http://producerh.io","location":1}' -p producer.h@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '{"producer":"producer.i","producer_key":"YOS5KM5t7td26VGDSEHXPHoUHB1Md1NnGRbT9EkXDaBf5nyhw18is","url":"http://produceri.io","location":1}' -p producer.i@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '{"producer":"producer.j","producer_key":"YOS5oduMFs5Lrbb8ZEc11KtyoVqfUjvaRxbUsQGgTqsEq18p1KqoC","url":"http://producerj.io","location":1}' -p producer.j@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite authproducer '{"producer":"producer.f"}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '{"producer":"producer.g"}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '{"producer":"producer.h"}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '{"producer":"producer.i"}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '{"producer":"producer.j"}' -p yosemite@active --txfee-payer yosemite

sleep 125
tail -n 150 $YOSEMITE_NODE_LOG_FILE

{ print_section_title "Resign \"yosemite\" delegating authority to \"yx.prods\""; } 2>/dev/null

$YOSEMITE_CLI get account yosemite
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yosemite","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yx.prods","permission":"active"}}]}}' -p yosemite@owner --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yosemite","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yx.prods","permission":"active"}}]}}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI get account yosemite
$YOSEMITE_CLI get account yx.prods

sleep 1

{ print_section_title "Resign \"yx.msig\" delegating authority to \"yosemite\""; } 2>/dev/null

$YOSEMITE_CLI get account yx.msig
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.msig","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.msig@owner --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.msig","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.msig@active --txfee-payer yosemite
$YOSEMITE_CLI get account yx.msig

sleep 1

{ print_section_title "Resign Yosemite Contract Accounts delegating authority to \"yosemite\""; } 2>/dev/null

$YOSEMITE_CLI get account yx.identity
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.identity","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.identity@owner --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.identity","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.identity@active --txfee-payer yosemite
$YOSEMITE_CLI get account yx.identity

$YOSEMITE_CLI get account yx.dcontract
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.dcontract","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.dcontract@owner --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.dcontract","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.dcontract@active --txfee-payer yosemite
$YOSEMITE_CLI get account yx.dcontract

#$YOSEMITE_CLI get account yx.nft
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.nft","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.nft@owner --txfee-payer yosemite
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.nft","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.nft@active --txfee-payer yosemite
#$YOSEMITE_CLI get account yx.nft

#$YOSEMITE_CLI get account yx.nftex
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.nftex","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.nftex@owner --txfee-payer yosemite
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.nftex","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.nftex@active --txfee-payer yosemite
#$YOSEMITE_CLI get account yx.nftex

#$YOSEMITE_CLI get account yx.escrow
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.escrow","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.escrow@owner --txfee-payer yosemite
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.escrow","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.escrow@active --txfee-payer yosemite
#$YOSEMITE_CLI get account yx.escrow

sleep 1

{ print_section_title "Setup Initial System Tokens"; } 2>/dev/null

$YOSEMITE_CLI push action systoken.a settokenmeta '{"sym":"4,DUSD","url":"http://sysdepo-a.org","desc":"system depository a"}' -p systoken.a@active --txfee-payer yosemite

$YOSEMITE_CLI push action systoken.a issue '{"t":"systoken.a","to":"systoken.a","qty":"1000.0000 DUSD","tag":"issue systoken.a"}' -p systoken.a@active --txfee-payer yosemite
sleep 1

$YOSEMITE_CLI get token info systoken.a
$YOSEMITE_CLI get token balance systoken.a systoken.a
$YOSEMITE_CLI get token balance systoken.a idauth.a
$YOSEMITE_CLI get token balance systoken.a idauth.b

$YOSEMITE_CLI push action systoken.b settokenmeta '{"sym":"4,DUSDB","url":"http://sysdepo-b.org","desc":"system depository b"}' -p systoken.b@active --txfee-payer yosemite

$YOSEMITE_CLI push action systoken.b issue '{"t":"systoken.b","to":"systoken.b","qty":"2000.0000 DUSDB","tag":"issue systoken.b"}' -p systoken.b@active --txfee-payer yosemite

$YOSEMITE_CLI get token info systoken.b
$YOSEMITE_CLI get token balance systoken.b systoken.b

$YOSEMITE_CLI push action yosemite addsystoken '{"token":"systoken.a","weight":10000}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite addsystoken '{"token":"systoken.b","weight":10000}' -p yosemite@active --txfee-payer yosemite
sleep 2

$YOSEMITE_CLI get systoken list

$YOSEMITE_CLI get systoken balance systoken.a
$YOSEMITE_CLI get systoken balance systoken.b

$YOSEMITE_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.a","qty":"5000.0000 DUSD","tag":"issue systoken.a to idauth.a"}' -p systoken.a@active --txfee-payer systoken.a
$YOSEMITE_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"5000.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a


{ print_section_title "Setup Initial Transaction Fees for Built-in Actions"; } 2>/dev/null

$YOSEMITE_CLI push action yosemite settxfee '{"code":"","action":"","value":1000,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"","action":"issue","value":50,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"","action":"transfer","value":100,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"","action":"redeem","value":200,"feetype":1}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '{"code":"yosemite","action":"newaccount","value":500,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"yosemite","action":"updateauth","value":1000,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"yosemite","action":"linkauth","value":1000,"feetype":1}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '{"code":"yx.identity","action":"setidinfo","value":50,"feetype":1}' -p yosemite@active --txfee-payer yosemite
sleep 1
$YOSEMITE_CLI get txfee list -L "yx.identity" -U "yx.identity"
$YOSEMITE_CLI push action yosemite unsettxfee '{"code":"yx.identity","action":"setidinfo"}' -p yosemite@active --txfee-payer yosemite
sleep 1
$YOSEMITE_CLI get txfee list -L "yx.identity" -U "yx.identity"
$YOSEMITE_CLI push action yosemite settxfee '{"code":"yx.identity","action":"setidinfo","value":0,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"yx.identity","action":"settype","value":0,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"yx.identity","action":"setkyc","value":0,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"yx.identity","action":"setstate","value":0,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"yx.identity","action":"setdata","value":0,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI get txfee list -L "yx.identity" -U "yx.identity"

$YOSEMITE_CLI get txfee item yosemite newaccount
$YOSEMITE_CLI get txfee item "" transfer
$YOSEMITE_CLI get txfee item "" ""
$YOSEMITE_CLI get txfee list -L "" -U 1
$YOSEMITE_CLI get txfee list -L yosemite -U yosemite -l 50
$YOSEMITE_CLI get txfee list

sleep 1

{ print_section_title "Create New User Accounts"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5KTdP4VyuhZ1ba3sX8JvHjKXgcTFUA75QHf92nrGYQX4coVJRAW
$YOSEMITE_CLI wallet import --private-key YPV_5KDKMejP2AZFGCMxLh67t7417coEgUu7PfQZk7ufuYA4wTBiNC4
$YOSEMITE_CLI wallet import --private-key YPV_5Jj4ryu8TY1cVhiLvPkBHKiwMAXV18WqJM97h8hfsBUt5sMJE3r
$YOSEMITE_CLI wallet import --private-key YPV_5KWoErpjc9yQ2HF6rLuijLFgmAqVEoDLGWmDQAT6NM5DCgcSgo1
$YOSEMITE_CLI wallet import --private-key YPV_5JP9oWWsMaTEBcw1Y7Jn8JzxQBq51FDcGVGqCi5gpMDyKqio26j

$YOSEMITE_CLI create account idauth.a useraccount1 YOS7y8FkTHo58ZvrMa6yrTpo3D2Y2AA4dMAZzQ8aZY6R6geZSDZ75 -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI create account idauth.a useraccount2 YOS8ZFsmfpevP8pKPMXuQMNB7P9KJoKzZwwzu2uww9VrRx7E7NUQj -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI create account idauth.a useraccount3 YOS5ubmvsnjHviACtfc9SwGbY7SprqTu1P5GQeDfwfZmhCq6aR7GH -p idauth.a@active --txfee-payer idauth.a

$YOSEMITE_CLI create account yosemite com YOS6vdcL347XXzpxSdnLUKpNwmA3KhYagcFqnZ3rNfKp96hzFxyit -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"com\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"1f32i7t23\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action systoken.a issue '{"t":"systoken.a","to":"com","qty":"3000.0000 DUSD","tag":"issue systoken.a to com"}' -p systoken.a@active --txfee-payer systoken.a
$YOSEMITE_CLI create account com acquire.com YOS6ibAshrW7QmXeM5gurexmw6ijwM9d1BYS1J6Y1kevAFf7PcLNh -p com@active --txfee-payer com

$YOSEMITE_CLI get systoken balance com
$YOSEMITE_CLI get systoken balance acquire.com
$YOSEMITE_CLI get systoken balance yx.txfee

$YOSEMITE_CLI get table -L com -l 1 yx.identity yx.identity identity

sleep 1

{ print_section_title "Managing Account Identity Info (KYC, Account Type/State/Data)"; } 2>/dev/null

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccount1\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 0111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccount2\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"23uyiuye\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccount3\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"vewv23r3\"}" -p idauth.a@active --txfee-payer idauth.a

$YOSEMITE_CLI get table -L useraccount1 -l 1 yx.identity yx.identity identity

$YOSEMITE_CLI push action yx.identity settype "{\"account\":\"useraccount1\", \"type\":$(echo 'ibase=2; 1000000000000000' | bc)}" -p idauth.a@active --txfee-payer idauth.a

$YOSEMITE_CLI push action yx.identity settype "{\"account\":\"useraccount1\", \"type\":$(echo 'ibase=2; 0' | bc)}" -p idauth.a@active --txfee-payer idauth.a

$YOSEMITE_CLI push action yx.identity setkyc "{\"account\":\"useraccount1\", \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p idauth.a@active --txfee-payer idauth.a

$YOSEMITE_CLI push action yx.identity setstate "{\"account\":\"useraccount1\", \"state\":$(echo 'ibase=2; 0001' | bc)}" -p idauth.a@active --txfee-payer idauth.a

$YOSEMITE_CLI push action yx.identity setdata "{\"account\":\"useraccount1\", \"data\":\"23fiuygy3\"}" -p idauth.a@active --txfee-payer idauth.a

sleep 1

{ print_section_title "System Token Issue / Transfer"; } 2>/dev/null

$YOSEMITE_CLI push action systoken.a issue '{"t":"systoken.a","to":"useraccount2","qty":"1000.0000 DUSD","tag":"issue systoken.a"}' -p systoken.a@active --txfee-payer systoken.a
$YOSEMITE_CLI push action systoken.a issue '{"t":"systoken.a","to":"useraccount3","qty":"1000.0000 DUSD","tag":"issue systoken.a"}' -p systoken.a@active --txfee-payer systoken.a

$YOSEMITE_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccount2","to":"useraccount3","qty":"100.0000 DUSD","tag":"transfer memo"}' -p useraccount2@active --txfee-payer useraccount2
$YOSEMITE_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccount2","to":"useraccount3","qty":"100.0000 DUSD","tag":"transfer memo 2"}' -p useraccount2@active --txfee-payer useraccount3

$YOSEMITE_CLI get systoken balance useraccount1
$YOSEMITE_CLI get systoken balance useraccount2
$YOSEMITE_CLI get systoken balance useraccount3
$YOSEMITE_CLI get systoken balance sysdepo1
$YOSEMITE_CLI get systoken balance yx.txfee

sleep 1


{ print_section_title "Transaction Votes"; } 2>/dev/null

$YOSEMITE_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccount3","to":"useraccount2","qty":"50.0000 DUSD","tag":"tag1"}' -p useraccount3@active --txfee-payer useraccount2 -v producer.f
$YOSEMITE_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccount3","to":"useraccount2","qty":"50.0000 DUSD","tag":"tag2"}' -p useraccount3@active --txfee-payer useraccount2 -v producer.g
$YOSEMITE_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccount3","to":"useraccount2","qty":"50.0000 DUSD","tag":"tag3"}' -p useraccount3@active --txfee-payer useraccount2 -v producer.g
sleep 2

$YOSEMITE_CLI get table yosemite yosemite producers
sleep 1


{ print_section_title "Account Recovery"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5KHCnUyZWqrB73iJp8UUiWfr4Q1K8UjBqgvSJzGgA9Z621u8E8q
$YOSEMITE_CLI wallet import --private-key YPV_5KVs1TdVEHE6FR2VXojhA71h667X2ZYfLNbi8u9hKYuVvDm3j49
$YOSEMITE_CLI wallet import --private-key YPV_5JbyqQwnxjByH8Aw8vrhiBf6Ze4NPj7ctw91KiqD1x53Fwz2uSF
$YOSEMITE_CLI wallet import --private-key YPV_5HvGPGhVFi9S1FmKF2wiYP3ZfQEiDvj6Q3i8X45Jhyh4xBSjXXj
$YOSEMITE_CLI wallet import --private-key YPV_5JKgY86UPP7i6cqxoHSpXHBGnAgyUhoawRY7t69G1eJs9WBLXEo

$YOSEMITE_CLI push action yosemite newaccount '{"creator":"idauth.a","name":"useraccountz","owner":{"threshold":1,"keys":[{"key":"YOS8M8gXkDHyffjeHTgmCqMapMN7LHzz8PnapVAY6GTcCChH4RGbr","weight":1}],"accounts":[{"permission":{"actor":"idauth.a","permission":"active"},"weight":1}],"waits":[]},"active":{"threshold":1,"keys":[{"key":"YOS5LA5GcJ8xYDXD2u9wQ7A79fwBzfvU59gVw21utaWgKgcB4yWeb","weight":1}],"accounts":[],"waits":[]}}' -p idauth.a@active --txfee-payer idauth.a -v producer.c
$YOSEMITE_CLI get account useraccountz
sleep 1
$YOSEMITE_CLI push action yosemite updateauth '{"account":"useraccountz","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key":"YOS6MWWFnregfdEB7txoK8enXh5jCd7967jJVSEhuhqA5xJWDhe7p","weight":1}],"waits":[],"accounts":[]}}' -p useraccountz@owner --txfee-payer idauth.a
$YOSEMITE_CLI get account useraccountz
sleep 1
$YOSEMITE_CLI push action yosemite updateauth '{"account":"useraccountz","permission":"owner","parent":"","auth":{"threshold":2,"keys":[{"key":"YOS7g4MjVQEpDPjNxNEBtNz11GJG7tzJowwNMFLsjqS4TddaC6zeU","weight":2}],"accounts":[{"permission":{"actor":"idauth.a","permission":"active"},"weight":1},{"permission":{"actor":"systoken.a","permission":"active"},"weight":1}],"waits":[]}}' -p useraccountz@owner --txfee-payer idauth.a
$YOSEMITE_CLI get account useraccountz
sleep 1
$YOSEMITE_CLI push action yosemite updateauth '{"account":"useraccountz","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key":"YOS7j467RUtMnQ6JWkzn9L6t2TKq9uJJbDNv7oPePbn7owRGJMVZw","weight":1}],"waits":[],"accounts":[]}}' -p useraccountz@owner --txfee-payer idauth.a
$YOSEMITE_CLI get account useraccountz
sleep 1
$YOSEMITE_CLI push action yosemite newaccount '{"creator":"idauth.a","name":"useraccountx","owner":{"threshold":2,"keys":[{"key":"YOS7g4MjVQEpDPjNxNEBtNz11GJG7tzJowwNMFLsjqS4TddaC6zeU","weight":2}],"accounts":[{"permission":{"actor":"idauth.a","permission":"active"},"weight":1},{"permission":{"actor":"systoken.a","permission":"active"},"weight":1}],"waits":[]},"active":{"threshold":1,"keys":[{"key":"YOS7j467RUtMnQ6JWkzn9L6t2TKq9uJJbDNv7oPePbn7owRGJMVZw","weight":1}],"accounts":[],"waits":[]}}' -p idauth.a@active --txfee-payer idauth.a -v producer.c
$YOSEMITE_CLI get account useraccountx
sleep 1
$YOSEMITE_CLI push action yosemite updateauth '{"account":"useraccountx","permission":"owner","parent":"","auth":{"threshold":2,"keys":[{"key":"YOS6MWWFnregfdEB7txoK8enXh5jCd7967jJVSEhuhqA5xJWDhe7p","weight":1}],"accounts":[{"permission":{"actor":"idauth.a","permission":"active"},"weight":1},{"permission":{"actor":"systoken.a","permission":"active"},"weight":1}],"waits":[]}}' -p useraccountx@owner --txfee-payer idauth.a
$YOSEMITE_CLI push action yosemite updateauth '{"account":"useraccountx","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key":"YOS6MWWFnregfdEB7txoK8enXh5jCd7967jJVSEhuhqA5xJWDhe7p","weight":1}],"accounts":[],"waits":[]}}' -p useraccountx@owner --txfee-payer idauth.a
$YOSEMITE_CLI get account useraccountx
sleep 1
$YOSEMITE_CLI push action yosemite newaccount '{"creator":"idauth.a","name":"useraccounty","owner":{"threshold":3,"keys":[{"key":"YOS7g4MjVQEpDPjNxNEBtNz11GJG7tzJowwNMFLsjqS4TddaC6zeU","weight":2}],"accounts":[{"permission":{"actor":"idauth.a","permission":"active"},"weight":2},{"permission":{"actor":"systoken.a","permission":"active"},"weight":1}],"waits":[{"wait_sec":604800,"weight":1}]},"active":{"threshold":1,"keys":[{"key":"YOS7j467RUtMnQ6JWkzn9L6t2TKq9uJJbDNv7oPePbn7owRGJMVZw","weight":1}],"accounts":[],"waits":[]}}' -p idauth.a@active --txfee-payer idauth.a -v producer.c
$YOSEMITE_CLI get account useraccounty
sleep 1

{ print_section_title "Deploy YOSEMITE Fiat Stable Coin as a System Token"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5K9aJSJyMhNLWvAtYm6x3ZYL13eaToVeT6JjQ1ABkQd383gXHFa
$YOSEMITE_CLI create account yosemite ysmt.dusd.a YOS568YZYS8i2Gr6gbJE9CEEsyaZFfRXzVS9BNw9FfCLr9Yccpg7o -p yosemite@active --txfee-payer yosemite

sleep 2
$YOSEMITE_CLI set contract ysmt.dusd.a $YOSEMITE_CONTRACTS_DIR/yosemitex.fiat.stable.token/ -p ysmt.dusd.a@active --txfee-payer yosemite
sleep 2

$YOSEMITE_CLI push action ysmt.dusd.a settokenmeta '{"sym":"4,DUSD","url":"https://yosemitex.com","desc":"Yosemite X Digital USD Token"}' -p ysmt.dusd.a@active --txfee-payer yosemite

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"ysmt.dusd.a\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"ysmt.dusd.a\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"ysmt.dusd.a","qty":"100000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '{"code":"ysmt.dusd.a","action":"issue","value":0,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ysmt.dusd.a","action":"redeem","value":50,"feetype":1}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite addsystoken '{"token":"ysmt.dusd.a","weight":10000}' -p yosemite@active --txfee-payer yosemite

sleep 2


{ print_section_title "YOSEMITE Standard Token Test"; } 2>/dev/null

$YOSEMITE_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.a","qty":"10000.0000 DUSD","tag":""}' -p systoken.a@active --txfee-payer systoken.a

$YOSEMITE_CLI push action systoken.a transfer '{"t":"systoken.a","from":"idauth.a","to":"idauth.b","qty":"50.0000 DUSD","tag":"transfer tag"}' -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action systoken.a transfer '{"t":"systoken.a","from":"idauth.a","to":"idauth.b","qty":"30.0000 DUSD","tag":"transfer tag 2"}' -p idauth.a@active --txfee-payer idauth.b

$YOSEMITE_CLI get token balance systoken.a idauth.a
$YOSEMITE_CLI get token balance systoken.a idauth.b

$YOSEMITE_CLI get token balance systoken.a systoken.a
$YOSEMITE_CLI push action systoken.a redeem '{"qty":"500.0000 DUSD","tag":"redeem tag"}' -p systoken.a@active --txfee-payer systoken.a

$YOSEMITE_CLI wallet import --private-key YPV_5JcSa5y1YQEEbxjZ7XjW2zApxDUqcuNLxA1nKji8E7Kif8b5wA4
$YOSEMITE_CLI wallet import --private-key YPV_5J7kM4HyzZuuJ5HcoywncQScmWfMErE4sz8gMustSmoEPKBYywk
$YOSEMITE_CLI wallet import --private-key YPV_5J6c9EwsTkmrcLrE5y6No2TgdcBB2X9dbx6Rs6ESF9TAtacDk1f

$YOSEMITE_CLI create account idauth.a useraccounta YOS5Thieg3kFy7rPu6hNpVtV8ijAUe89xaGFVA7X2KwqMXYEwJgDJ -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI create account idauth.a useraccountb YOS4tdykSjH8R87BeHf76CHKP2ZLSwctz7CLshLYzfheR6xRVzgLy -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI create account idauth.a useraccountc YOS6T82Va2G3oYhLAxoe5c9q2PDScLD2YZP9Yg8f9KGJcNdtcUJh6 -p idauth.a@active --txfee-payer idauth.a

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccounta\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"qwefewff2f\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccountb\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"wef2ouih23\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccountc\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 0111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"kh23r23ruy\"}" -p idauth.a@active --txfee-payer idauth.a

$YOSEMITE_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"useraccounta","qty":"50000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a
$YOSEMITE_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"useraccountb","qty":"20000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a
#$YOSEMITE_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"useraccountc","qty":"10000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a

$YOSEMITE_CLI get token balance ysmt.dusd.a useraccounta
$YOSEMITE_CLI get token balance ysmt.dusd.a useraccountb

$YOSEMITE_CLI push action ysmt.dusd.a transfer '{"t":"ysmt.dusd.a","from":"useraccounta","to":"useraccountb","qty":"100.0000 DUSD","tag":"transfer tag"}' -p useraccounta@active --txfee-payer useraccounta -v producer.d


$YOSEMITE_CLI push action systoken.a issue '{"t":"systoken.a","to":"useraccounta","qty":"1000.0000 DUSD","tag":""}' -p systoken.a@active --txfee-payer systoken.a
$YOSEMITE_CLI push action systoken.b issue '{"t":"systoken.b","to":"useraccounta","qty":"1000.0000 DUSDB","tag":""}' -p systoken.b@active --txfee-payer systoken.b

$YOSEMITE_CLI push action systoken.a issue '{"t":"systoken.a","to":"useraccountb","qty":"2000.0000 DUSD","tag":""}' -p systoken.a@active --txfee-payer systoken.a
$YOSEMITE_CLI push action systoken.a issue '{"t":"systoken.a","to":"useraccountc","qty":"3000.0000 DUSD","tag":""}' -p systoken.a@active --txfee-payer systoken.a

$YOSEMITE_CLI get token balance systoken.a useraccounta
$YOSEMITE_CLI get token balance systoken.b useraccounta
$YOSEMITE_CLI get systoken balance useraccounta

$YOSEMITE_CLI get token balance systoken.a useraccountb
$YOSEMITE_CLI get token balance systoken.a useraccountc

$YOSEMITE_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccounta","to":"useraccountb","qty":"999.5000 DUSD","tag":"transfer tag"}' -p useraccounta@active --txfee-payer useraccounta -v producer.a

$YOSEMITE_CLI get systoken balance useraccounta
$YOSEMITE_CLI get systoken balance useraccountb
$YOSEMITE_CLI get systoken balance useraccountc

sleep 2

{ print_section_title "YOSEMITE Credit Token Test"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5KKMcPu86p6vbsuAozTT9vLxUT4GeZFqSWrbgok1LjpthJEB5Qe
$YOSEMITE_CLI create account yosemite ycard.cusd.a YOS7mK7vvTTCDzEr8gtMjrMsbWA25UsS3WmV8WRoqC5gSMh9JLcQM -p yosemite@active --txfee-payer yosemite

sleep 2
$YOSEMITE_CLI set contract ycard.cusd.a $YOSEMITE_CONTRACTS_DIR/yosemitex.credit.token/ -p ycard.cusd.a@active --txfee-payer yosemite
sleep 2

$YOSEMITE_CLI push action ycard.cusd.a settokenmeta '{"sym":"4,CUSD","url":"https://yosemitecardx.com","desc":"Yosemite Card X USD Credit Token"}' -p ycard.cusd.a@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.cusd.a","action":"creditlimit","value":100,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.cusd.a","action":"creditissue","value":300,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.cusd.a","action":"creditredeem","value":500,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.cusd.a","action":"offtransfer","value":50,"feetype":1}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"ycard.cusd.a\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"if3fhu3eu\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"ycard.cusd.a","qty":"70000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.a

$YOSEMITE_CLI get token balance ysmt.dusd.a ycard.cusd.a

sleep 2

## issuing credit card to useraccounta by offering credit limit
$YOSEMITE_CLI push action ycard.cusd.a creditlimit '{"account":"useraccounta","credit_limit":"500.0000 CUSD","tag":"vhw93rh23r"}' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.b

$YOSEMITE_CLI get table -L useraccounta -l 1 ycard.cusd.a ycard.cusd.a creditoffer

sleep 2

## useraccounta issues credit tokens and deposits to ycard service
$YOSEMITE_CLI push action yosemite updateauth '{"account":"useraccounta","permission":"creditissue","parent":"active","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"ycard.cusd.a","permission":"active"}}]}}' -p useraccounta@active --txfee-payer ycard.cusd.a -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"useraccounta","code":"ycard.cusd.a","type":"creditissue","requirement":"creditissue"}' -p useraccounta@active --txfee-payer ycard.cusd.a -v producer.c

$YOSEMITE_CLI push action ycard.cusd.a creditissue '{"issuer":"useraccounta","to":"ycard.cusd.a","qty":"500.0000 CUSD","tag":"issue credit token by user, and deposit to credit service"}' -p useraccounta@creditissue --txfee-payer ycard.cusd.a -v producer.c
#$YOSEMITE_CLI push action ycard.cusd.a transfer '{"t":"ycard.cusd.a","from":"useraccounta","to":"ycard.cusd.a","qty":"500.0000 CUSD","tag":"deposit credit to service account for anonymous tx"}' -p useraccounta@active --txfee-payer ycard.cusd.a -v producer.c

$YOSEMITE_CLI get table -L useraccounta -l 1 ycard.cusd.a ycard.cusd.a creditoffer
$YOSEMITE_CLI get token balance ycard.cusd.a useraccounta
$YOSEMITE_CLI get token balance ycard.cusd.a ycard.cusd.a

sleep 2

## anonymous payment transaction to merchant useraccountb
$YOSEMITE_CLI push action ycard.cusd.a transfer '{"t":"ycard.cusd.a","from":"ycard.cusd.a","to":"useraccountb","qty":"100.0000 CUSD","tag":"payment tx 1"}' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.d
$YOSEMITE_CLI push action ycard.cusd.a transfer '{"t":"ycard.cusd.a","from":"ycard.cusd.a","to":"useraccountb","qty":"100.0000 CUSD","tag":"payment tx 2"}' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.d

$YOSEMITE_CLI get token balance ycard.cusd.a useraccountb

## anonymous off-chain token transfer, logging just an evidence of off-chain token transfer (hash of transfer data)
$YOSEMITE_CLI push action ycard.cusd.a offtransfer '{"t":"ycard.cusd.a","to":"useraccountc","tag":"QmT4AeWE9Q9EaoyLJiqaZuYQ8mJeq4ZBncjjFH9dQ9uDVA"}' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.e

sleep 2

## monthly user credit balance settlement
$YOSEMITE_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"ycard.cusd.a","qty":"200.0000 DUSD","tag":"useraccounta credit settlement"}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.g

$YOSEMITE_CLI push action ycard.cusd.a redeem '{"qty":"300.0000 CUSD","tag":"burn unused useraccounta credit"}' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.g

$YOSEMITE_CLI push action ycard.cusd.a creditredeem '{"account":"useraccounta","qty":"500.0000 CUSD","tag":"reset useraccounta credit offering"}' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.e

$YOSEMITE_CLI push action ycard.cusd.a creditissue '{"issuer":"useraccounta","to":"ycard.cusd.a","qty":"500.0000 CUSD","tag":"monthly issue credit token by user, and deposit to credit service"}' -p useraccounta@creditissue --txfee-payer ycard.cusd.a -v producer.e
#$YOSEMITE_CLI push action ycard.cusd.a transfer '{"t":"ycard.cusd.a","from":"useraccounta","to":"ycard.cusd.a","qty":"500.0000 CUSD","tag":"deposit credit to service account for anonymous tx"}' -p useraccounta@active --txfee-payer ycard.cusd.a -v producer.e

sleep 2

## merchant(useraccountb) redeems credit tokens and withdraws fiat
$YOSEMITE_CLI push action ycard.cusd.a transfer '{"t":"ycard.cusd.a","from":"useraccountb","to":"ycard.cusd.a","qty":"200.0000 CUSD","tag":"request redeeming credit token"}' -p useraccountb@active --txfee-payer ycard.cusd.a -v producer.h

$YOSEMITE_CLI push action ysmt.dusd.a transfer '{"t":"ysmt.dusd.a","from":"ycard.cusd.a","to":"useraccountb","qty":"200.0000 DUSD","tag":"redeeming CUSD to DUSD"}' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.h

$YOSEMITE_CLI push action ycard.cusd.a redeem '{"qty":"200.0000 CUSD","tag":"burn redeemed credit tokens"}' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.h

sleep 2

$YOSEMITE_CLI get token balance ycard.cusd.a useraccountb
$YOSEMITE_CLI get token balance ysmt.dusd.a useraccountb

$YOSEMITE_CLI push action ysmt.dusd.a transfer '{"t":"ysmt.dusd.a","from":"useraccountb","to":"ysmt.dusd.a","qty":"200.0000 DUSD","tag":"request redeeming DUSD to fiat"}' -p useraccountb@active --txfee-payer ycard.cusd.a -v producer.i
$YOSEMITE_CLI push action ysmt.dusd.a redeem '{"qty":"200.0000 DUSD","tag":"redeem DUSD to fiat"}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.i
