#!/usr/bin/env bash
# chmod +x ./reset_std_token_yosemite_testnet_on_server_side.sh

YOSEMITE_NODE_BIN_NAME=yosemite
YOSEMITE_CLI_BIN_NAME=clyos
YOSEMITE_KEYD_BIN_NAME=keyos
YOSEMITE_HOME=/mnt/yosemite-public-blockchain-git
YOSEMITE_NODE=$YOSEMITE_HOME/build/programs/$YOSEMITE_NODE_BIN_NAME/$YOSEMITE_NODE_BIN_NAME
YOSEMITE_NODE_LOG_FILE=/mnt/$YOSEMITE_NODE_BIN_NAME.log
YOSEMITE_CLI="$YOSEMITE_HOME/build/programs/$YOSEMITE_CLI_BIN_NAME/$YOSEMITE_CLI_BIN_NAME --wallet-url http://127.0.0.1:8900/"
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
$YOSEMITE_CLI push action yosemite setpriv '["yx.msig",1]' -p yosemite@active --txfee-payer yosemite
sleep 2


{ print_section_title "Install Identity System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.identity $YOSEMITE_CONTRACTS_DIR/yx.identity/ -p yx.identity@active --txfee-payer yosemite
sleep 2


{ print_section_title "Register Initial Identity Authorities"; } 2>/dev/null

$YOSEMITE_CLI push action yosemite regidauth '["idauth.a","http://idauth-a.org",1]' -p idauth.a@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authidauth '["idauth.a"]' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite regidauth '["idauth.b","http://idauth-b.org",1]' -p idauth.b@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authidauth '["idauth.b"]' -p yosemite@active --txfee-payer yosemite

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

$YOSEMITE_CLI push action yosemite regproducer '["producer.a","YOS5Audoa4mpZaYhp7vwYVCUnsQCUVifftdPipvkfZ9qVggoYoHUn","http://producera.io",1]' -p producer.a@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '["producer.b","YOS5aw9PzjxJCTi23FWtcB6Q8feMhfLg7Toh7PwGoWge4K4xNWQdm","http://producerb.io",1]' -p producer.b@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite authproducer '["producer.a"]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '["producer.b"]' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite regproducer '["producer.c","YOS8cvC5FJozTTVfUVXZ4E4kz1eNsKoBsnG7J76Fw1gX1wstGoUWo","http://producerc.io",1]' -p producer.c@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '["producer.d","YOS6ig1G6hpk1Tzj1Ko8zfysATY4eqpb9znyEnx25zbkHscV6qHvy","http://producerd.io",1]' -p producer.d@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '["producer.e","YOS72LDKqDc2KvyN1XeEYhv7AbkMUYB8B3fJ55yMn4ZqLzeqxz3w1","http://producere.io",1]' -p producer.e@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite authproducer '["producer.c"]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '["producer.d"]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '["producer.e"]' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite regproducer '["producer.f","YOS5SzGAGCMznawLLY9xkpa4ta62CfTKu6di9AjZ9bWCBJ1pFu641","http://producerf.io",1]' -p producer.f@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '["producer.g","YOS5t1fHFunR2rWq5z8NHPrxj1H4xG5Vq4bGKcH33yg1eZMCVPQRq","http://producerg.io",1]' -p producer.g@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '["producer.h","YOS5unphoov9UCG8AidDYv14fMJVRnHuihScASHbdFXHuDeDAS2s6","http://producerh.io",1]' -p producer.h@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '["producer.i","YOS5KM5t7td26VGDSEHXPHoUHB1Md1NnGRbT9EkXDaBf5nyhw18is","http://produceri.io",1]' -p producer.i@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite regproducer '["producer.j","YOS5oduMFs5Lrbb8ZEc11KtyoVqfUjvaRxbUsQGgTqsEq18p1KqoC","http://producerj.io",1]' -p producer.j@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite authproducer '["producer.f"]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '["producer.g"]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '["producer.h"]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '["producer.i"]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authproducer '["producer.j"]' -p yosemite@active --txfee-payer yosemite

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

#$YOSEMITE_CLI get account yx.dcontract
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.dcontract","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.dcontract@owner --txfee-payer yosemite
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.dcontract","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.dcontract@active --txfee-payer yosemite
#$YOSEMITE_CLI get account yx.dcontract

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

$YOSEMITE_CLI push action systoken.a settokenmeta '["4,DUSD","http://sysdepo-a.org","system depository a"]' -p systoken.a@active --txfee-payer yosemite

$YOSEMITE_CLI push action systoken.a issue '["systoken.a","systoken.a","1000.0000 DUSD","issue systoken.a"]' -p systoken.a@active --txfee-payer yosemite
sleep 1

$YOSEMITE_CLI get token info systoken.a
$YOSEMITE_CLI get token balance systoken.a systoken.a
$YOSEMITE_CLI get token balance systoken.a idauth.a
$YOSEMITE_CLI get token balance systoken.a idauth.b

$YOSEMITE_CLI push action systoken.b settokenmeta '["4,DUSDB","http://sysdepo-b.org","system depository b"]' -p systoken.b@active --txfee-payer yosemite

$YOSEMITE_CLI push action systoken.b issue '["systoken.b","systoken.b","2000.0000 DUSDB","issue systoken.b"]' -p systoken.b@active --txfee-payer yosemite

$YOSEMITE_CLI get token info systoken.b
$YOSEMITE_CLI get token balance systoken.b systoken.b

$YOSEMITE_CLI push action yosemite addsystoken '["systoken.a",10000]' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite addsystoken '["systoken.b",10000]' -p yosemite@active --txfee-payer yosemite
sleep 2

$YOSEMITE_CLI get systoken list

$YOSEMITE_CLI get systoken balance systoken.a
$YOSEMITE_CLI get systoken balance systoken.b

$YOSEMITE_CLI push action systoken.a issue '["systoken.a","idauth.a","5000.0000 DUSD","issue systoken.a to idauth.a"]' -p systoken.a@active --txfee-payer systoken.a
$YOSEMITE_CLI push action systoken.a issue '["systoken.a","idauth.b","5000.0000 DUSD","issue systoken.a to idauth.b"]' -p systoken.a@active --txfee-payer systoken.a


{ print_section_title "Setup Initial Transaction Fees for Built-in Actions"; } 2>/dev/null

$YOSEMITE_CLI push action yosemite settxfee '["","",1000,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["","issue",50,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["","transfer",100,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["","redeem",200,1]' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '["yosemite","newaccount",500,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["yosemite","updateauth",1000,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["yosemite","linkauth",1000,1]' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '["yx.identity","setidinfo",50,1]' -p yosemite@active --txfee-payer yosemite
sleep 1
$YOSEMITE_CLI get txfee list -L "yx.identity" -U "yx.identity"
$YOSEMITE_CLI push action yosemite unsettxfee '["yx.identity","setidinfo"]' -p yosemite@active --txfee-payer yosemite
sleep 1
$YOSEMITE_CLI get txfee list -L "yx.identity" -U "yx.identity"
$YOSEMITE_CLI push action yosemite settxfee '["yx.identity","setidinfo",0,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["yx.identity","settype",0,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["yx.identity","setkyc",0,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["yx.identity","setstate",0,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["yx.identity","setdata",0,1]' -p yosemite@active --txfee-payer yosemite
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
$YOSEMITE_CLI push action systoken.a issue '["systoken.a","com","3000.0000 DUSD","issue systoken.a to com"]' -p systoken.a@active --txfee-payer systoken.a
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

$YOSEMITE_CLI push action systoken.a issue '["systoken.a","useraccount2","1000.0000 DUSD","issue systoken.a"]' -p systoken.a@active --txfee-payer systoken.a
$YOSEMITE_CLI push action systoken.a issue '["systoken.a","useraccount3","1000.0000 DUSD","issue systoken.a"]' -p systoken.a@active --txfee-payer systoken.a

$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","useraccount2","useraccount3","100.0000 DUSD","transfer memo"]' -p useraccount2@active --txfee-payer useraccount2
$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","useraccount2","useraccount3","100.0000 DUSD","transfer memo 2"]' -p useraccount2@active --txfee-payer useraccount3

$YOSEMITE_CLI get systoken balance useraccount1
$YOSEMITE_CLI get systoken balance useraccount2
$YOSEMITE_CLI get systoken balance useraccount3
$YOSEMITE_CLI get systoken balance sysdepo1
$YOSEMITE_CLI get systoken balance yx.txfee

sleep 1


{ print_section_title "Transaction Votes"; } 2>/dev/null

$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","useraccount3","useraccount2","50.0000 DUSD","tag1"]' -p useraccount3@active --txfee-payer useraccount2 -v producer.f
$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","useraccount3","useraccount2","50.0000 DUSD","tag2"]' -p useraccount3@active --txfee-payer useraccount2 -v producer.g
$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","useraccount3","useraccount2","50.0000 DUSD","tag3"]' -p useraccount3@active --txfee-payer useraccount2 -v producer.g
sleep 2

$YOSEMITE_CLI get table yosemite yosemite producers
sleep 1


{ print_section_title "Deploy YOSEMITE Fiat Stable Coin as a System Token"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5K9aJSJyMhNLWvAtYm6x3ZYL13eaToVeT6JjQ1ABkQd383gXHFa
$YOSEMITE_CLI create account yosemite ysmt.dusd.a YOS568YZYS8i2Gr6gbJE9CEEsyaZFfRXzVS9BNw9FfCLr9Yccpg7o -p yosemite@active --txfee-payer yosemite

sleep 2
$YOSEMITE_CLI set contract ysmt.dusd.a $YOSEMITE_CONTRACTS_DIR/yosemitex.fiat.stable.token/ -p ysmt.dusd.a@active --txfee-payer yosemite
sleep 2

$YOSEMITE_CLI push action ysmt.dusd.a settokenmeta '["4,DUSD","https://yosemitex.com","Yosemite X Digital USD Token"]' -p ysmt.dusd.a@active --txfee-payer yosemite

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"ysmt.dusd.a\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"ysmt.dusd.a\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action ysmt.dusd.a issue '["ysmt.dusd.a","ysmt.dusd.a","100000.0000 DUSD",""]' -p ysmt.dusd.a@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '["ysmt.dusd.a","issue",0,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["ysmt.dusd.a","redeem",50,1]' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite addsystoken '["ysmt.dusd.a",10000]' -p yosemite@active --txfee-payer yosemite

sleep 2


{ print_section_title "YOSEMITE Standard Token Test"; } 2>/dev/null

$YOSEMITE_CLI push action systoken.a issue '["systoken.a","idauth.a","10000.0000 DUSD",""]' -p systoken.a@active --txfee-payer systoken.a

$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","idauth.a","idauth.b","50.0000 DUSD","transfer tag"]' -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","idauth.a","idauth.b","30.0000 DUSD","transfer tag 2"]' -p idauth.a@active --txfee-payer idauth.b

$YOSEMITE_CLI get token balance systoken.a idauth.a
$YOSEMITE_CLI get token balance systoken.a idauth.b

$YOSEMITE_CLI get token balance systoken.a systoken.a
$YOSEMITE_CLI push action systoken.a redeem '["500.0000 DUSD","redeem tag"]' -p systoken.a@active --txfee-payer systoken.a

$YOSEMITE_CLI wallet import --private-key YPV_5JcSa5y1YQEEbxjZ7XjW2zApxDUqcuNLxA1nKji8E7Kif8b5wA4
$YOSEMITE_CLI wallet import --private-key YPV_5J7kM4HyzZuuJ5HcoywncQScmWfMErE4sz8gMustSmoEPKBYywk
$YOSEMITE_CLI wallet import --private-key YPV_5J6c9EwsTkmrcLrE5y6No2TgdcBB2X9dbx6Rs6ESF9TAtacDk1f

$YOSEMITE_CLI create account idauth.a useraccounta YOS5Thieg3kFy7rPu6hNpVtV8ijAUe89xaGFVA7X2KwqMXYEwJgDJ -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI create account idauth.a useraccountb YOS4tdykSjH8R87BeHf76CHKP2ZLSwctz7CLshLYzfheR6xRVzgLy -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI create account idauth.a useraccountc YOS6T82Va2G3oYhLAxoe5c9q2PDScLD2YZP9Yg8f9KGJcNdtcUJh6 -p idauth.a@active --txfee-payer idauth.a

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccounta\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"qwefewff2f\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccountb\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"wef2ouih23\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccountc\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 0111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"kh23r23ruy\"}" -p idauth.a@active --txfee-payer idauth.a

$YOSEMITE_CLI push action ysmt.dusd.a issue '["ysmt.dusd.a","useraccounta","50000.0000 DUSD",""]' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a
$YOSEMITE_CLI push action ysmt.dusd.a issue '["ysmt.dusd.a","useraccountb","20000.0000 DUSD",""]' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a
#$YOSEMITE_CLI push action ysmt.dusd.a issue '["ysmt.dusd.a","useraccountc","10000.0000 DUSD",""]' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a

$YOSEMITE_CLI get token balance ysmt.dusd.a useraccounta
$YOSEMITE_CLI get token balance ysmt.dusd.a useraccountb

$YOSEMITE_CLI push action ysmt.dusd.a transfer '["ysmt.dusd.a","useraccounta","useraccountb","100.0000 DUSD","transfer tag"]' -p useraccounta@active --txfee-payer useraccounta -v producer.d


$YOSEMITE_CLI push action systoken.a issue '["systoken.a","useraccounta","1000.0000 DUSD",""]' -p systoken.a@active --txfee-payer systoken.a
$YOSEMITE_CLI push action systoken.b issue '["systoken.b","useraccounta","1000.0000 DUSDB",""]' -p systoken.b@active --txfee-payer systoken.b

$YOSEMITE_CLI push action systoken.a issue '["systoken.a","useraccountb","2000.0000 DUSD",""]' -p systoken.a@active --txfee-payer systoken.a
$YOSEMITE_CLI push action systoken.a issue '["systoken.a","useraccountc","3000.0000 DUSD",""]' -p systoken.a@active --txfee-payer systoken.a

$YOSEMITE_CLI get token balance systoken.a useraccounta
$YOSEMITE_CLI get token balance systoken.b useraccounta
$YOSEMITE_CLI get systoken balance useraccounta

$YOSEMITE_CLI get token balance systoken.a useraccountb
$YOSEMITE_CLI get token balance systoken.a useraccountc

$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","useraccounta","useraccountb","999.5000 DUSD","transfer tag"]' -p useraccounta@active --txfee-payer useraccounta -v producer.a

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

$YOSEMITE_CLI push action ycard.cusd.a settokenmeta '["4,CUSD","https://yosemitecardx.com","Yosemite Card X USD Credit Token"]' -p ycard.cusd.a@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '["ycard.cusd.a","creditlimit",100,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["ycard.cusd.a","creditissue",300,1]' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["ycard.cusd.a","creditredeem",500,1]' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"ycard.cusd.a\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"if3fhu3eu\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action ysmt.dusd.a issue '["ysmt.dusd.a","ycard.cusd.a","70000.0000 DUSD",""]' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.a

$YOSEMITE_CLI get token balance ysmt.dusd.a ycard.cusd.a

sleep 2

## issuing credit card to useraccounta by offering credit limit
$YOSEMITE_CLI push action ycard.cusd.a creditlimit '["useraccounta","500.0000 CUSD","vhw93rh23r"]' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.b

$YOSEMITE_CLI get table -L useraccounta -l 1 ycard.cusd.a ycard.cusd.a creditoffer

sleep 2

## useraccounta issues credit tokens and deposits to ycard service
$YOSEMITE_CLI push action yosemite updateauth '{"account":"useraccounta","permission":"creditissue","parent":"active","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"ycard.cusd.a","permission":"active"}}]}}' -p useraccounta@active --txfee-payer ycard.cusd.a -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '["useraccounta","ycard.cusd.a","creditissue","creditissue"]' -p useraccounta@active --txfee-payer ycard.cusd.a -v producer.c

$YOSEMITE_CLI push action ycard.cusd.a creditissue '["useraccounta","ycard.cusd.a","500.0000 CUSD","issue credit token by user, and deposit to credit service"]' -p useraccounta@creditissue --txfee-payer ycard.cusd.a -v producer.c
#$YOSEMITE_CLI push action ycard.cusd.a transfer '["ycard.cusd.a","useraccounta","ycard.cusd.a","500.0000 CUSD","deposit credit to service account for anonymous tx"]' -p useraccounta@active --txfee-payer ycard.cusd.a -v producer.c

$YOSEMITE_CLI get table -L useraccounta -l 1 ycard.cusd.a ycard.cusd.a creditoffer
$YOSEMITE_CLI get token balance ycard.cusd.a useraccounta
$YOSEMITE_CLI get token balance ycard.cusd.a ycard.cusd.a

sleep 2

## anonymous payment transaction to merchant useraccountb
$YOSEMITE_CLI push action ycard.cusd.a transfer '["ycard.cusd.a","ycard.cusd.a","useraccountb","100.0000 CUSD","payment tx 1"]' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.d
$YOSEMITE_CLI push action ycard.cusd.a transfer '["ycard.cusd.a","ycard.cusd.a","useraccountb","100.0000 CUSD","payment tx 2"]' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.d

$YOSEMITE_CLI get token balance ycard.cusd.a useraccountb

sleep 2

## monthly user credit balance settlement
$YOSEMITE_CLI push action ysmt.dusd.a issue '["ysmt.dusd.a","ycard.cusd.a","200.0000 DUSD","useraccounta credit settlement"]' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.g

$YOSEMITE_CLI push action ycard.cusd.a redeem '["300.0000 CUSD","burn unused useraccounta credit"]' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.g

$YOSEMITE_CLI push action ycard.cusd.a creditredeem '["useraccounta","500.0000 CUSD","reset useraccounta credit offering"]' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.e

$YOSEMITE_CLI push action ycard.cusd.a creditissue '["useraccounta","ycard.cusd.a","500.0000 CUSD","monthly issue credit token by user, and deposit to credit service"]' -p useraccounta@creditissue --txfee-payer ycard.cusd.a -v producer.e
#$YOSEMITE_CLI push action ycard.cusd.a transfer '["ycard.cusd.a","useraccounta","ycard.cusd.a","500.0000 CUSD","deposit credit to service account for anonymous tx"]' -p useraccounta@active --txfee-payer ycard.cusd.a -v producer.e

sleep 2

## merchant(useraccountb) redeems credit tokens and withdraws fiat
$YOSEMITE_CLI push action ycard.cusd.a transfer '["ycard.cusd.a","useraccountb","ycard.cusd.a","200.0000 CUSD","request redeeming credit token"]' -p useraccountb@active --txfee-payer ycard.cusd.a -v producer.h

$YOSEMITE_CLI push action ysmt.dusd.a transfer '["ysmt.dusd.a","ycard.cusd.a","useraccountb","200.0000 DUSD","redeeming CUSD to DUSD"]' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.h

$YOSEMITE_CLI push action ycard.cusd.a redeem '["200.0000 CUSD","burn redeemed credit tokens"]' -p ycard.cusd.a@active --txfee-payer ycard.cusd.a -v producer.h

sleep 2

$YOSEMITE_CLI get token balance ycard.cusd.a useraccountb
$YOSEMITE_CLI get token balance ysmt.dusd.a useraccountb

$YOSEMITE_CLI push action ysmt.dusd.a transfer '["ysmt.dusd.a","useraccountb","ysmt.dusd.a","200.0000 DUSD","request redeeming DUSD to fiat"]' -p useraccountb@active --txfee-payer ycard.cusd.a -v producer.i
$YOSEMITE_CLI push action ysmt.dusd.a redeem '["200.0000 DUSD","redeem DUSD to fiat"]' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.i
