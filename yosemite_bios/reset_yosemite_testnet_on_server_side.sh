#!/usr/bin/env bash
# chmod +x ./reset_yosemite_testnet_on_server_side.sh

#YSMT_TESTNET_SSH_KEY_FILE=~/Documents/__HalfDomeChain__/AWS/ssh_key/ysmt_testnet_dev_server_ap_northeast_seoul.pem
#YSMT_TESTNET_NODE_USER_HOST=ubuntu@ec2-13-124-23-23.ap-northeast-2.compute.amazonaws.com
#scp -i ${YSMT_TESTNET_SSH_KEY_FILE} ./reset_yosemite_testnet_on_server_side.sh ${YSMT_TESTNET_NODE_USER_HOST}:/mnt/yosemite_testnet_mgmt/reset_yosemite_testnet_on_server_side.sh

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

$YOSEMITE_CLI create account yosemite yx.msig YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$YOSEMITE_CLI create account yosemite yx.txfee YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$YOSEMITE_CLI create account yosemite yx.identity YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$YOSEMITE_CLI create account yosemite yx.ntoken YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$YOSEMITE_CLI create account yosemite yx.token YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3
$YOSEMITE_CLI create account yosemite yx.dcontract YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3


{ print_section_title "Create initial Identity Authority Account"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5K5Qe2ndgcfhWqdUjHnfhvCKVqCE82Rih6JrNQuuVJBcathAx9m
$YOSEMITE_CLI create account yosemite idauth1 YOS6EfGUaA5MNLH1GiHd64DcDr3HMgY1AM3WR1vdHKaah9Z4cWPZq

$YOSEMITE_CLI wallet import --private-key YPV_5JRzPhyg9jY4thDJ1rLsSr4zdLLoA7UEMQRw3vJFGmGKTZF5Kbx
$YOSEMITE_CLI create account yosemite idauth2 YOS5VvWLCqXL3AQyuXZfUTnEiWuhJNV6Nq3YbbS7Z98cN7GiRo9LR


{ print_section_title "Create initial System Depository Account"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5J8WZRwfYVzsH8i3LaRQLGorVD788RFYqALEsLmeK8a6Kq6re2W
$YOSEMITE_CLI create account yosemite sysdepo1 YOS6o8hwGio5V2LgBxFMZi4bdVG5AvcHeFiMpaREKuZ29BwXpNTpH

$YOSEMITE_CLI wallet import --private-key YPV_5K5MBizJ6HuG1kdvQTeMnpKjpFSZHm64Nj3X6MvJR6AnTF8mHkx
$YOSEMITE_CLI create account yosemite sysdepo2 YOS61njg9Zs9oQmAmwLnrcVBrniWKshKAhoQJJqNVVnYXCPSRKLTE


{ print_section_title "Install Yosemite System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yosemite $YOSEMITE_CONTRACTS_DIR/yx.system/ -p yosemite@active
sleep 2


{ print_section_title "Install Multi-sig System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.msig $YOSEMITE_CONTRACTS_DIR/eosio.msig/ -p yx.msig@active
sleep 2
$YOSEMITE_CLI push action yosemite setpriv '["yx.msig",1]' -p yosemite@active
sleep 2


{ print_section_title "Install Transaction Fee System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.txfee $YOSEMITE_CONTRACTS_DIR/yx.txfee/ -p yx.txfee@active
sleep 2


{ print_section_title "Install Identity System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.identity $YOSEMITE_CONTRACTS_DIR/yx.identity/ -p yx.identity@active
sleep 2


{ print_section_title "Register Initial Identity Authority"; } 2>/dev/null

$YOSEMITE_CLI push action yosemite regidauth '["idauth1","http://idauth1.org",1]' -p idauth1@active -p yosemite@active
$YOSEMITE_CLI push action yosemite authidauth '["idauth1"]' -p yosemite@active

$YOSEMITE_CLI push action yosemite regidauth '["idauth2","http://idauth2.org",1]' -p idauth2@active -p yosemite@active
$YOSEMITE_CLI push action yosemite authidauth '["idauth2"]' -p yosemite@active

$YOSEMITE_CLI get table yosemite yosemite idauthority

sleep 2


{ print_section_title "Register Initial System Depository"; } 2>/dev/null

$YOSEMITE_CLI push action yosemite regsysdepo '["sysdepo1","http://sysdepo1.org",1]' -p sysdepo1@active -p yosemite@active
$YOSEMITE_CLI push action yosemite authsysdepo '["sysdepo1"]' -p yosemite@active

$YOSEMITE_CLI push action yosemite regsysdepo '["sysdepo2","http://sysdepo2.org",1]' -p sysdepo2@active -p yosemite@active
$YOSEMITE_CLI push action yosemite authsysdepo '["sysdepo2"]' -p yosemite@active

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"sysdepo1\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"sysdepo\"}" -p idauth1@active
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"sysdepo2\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"sysdepo\"}" -p idauth1@active
$YOSEMITE_CLI get table -L sysdepo1 -l 1 yx.identity yx.identity identity
$YOSEMITE_CLI get table -L sysdepo2 -l 1 yx.identity yx.identity identity

$YOSEMITE_CLI get table yosemite yosemite sysdepos

sleep 2

{ print_section_title "Install Native Token Contract"; } 2>/dev/null

slee 2
$YOSEMITE_CLI set contract yx.ntoken $YOSEMITE_CONTRACTS_DIR/yx.ntoken/
slee 2
$YOSEMITE_CLI push action yosemite setpriv '["yx.ntoken",1]' -p yosemite@active
slee 2

$YOSEMITE_CLI push action yx.ntoken setkycrule "{\"type\":0, \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p yosemite@active
$YOSEMITE_CLI push action yx.ntoken setkycrule "{\"type\":1, \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p yosemite@active


{ print_section_title "Install User Token Contract"; } 2>/dev/null

slee 2
$YOSEMITE_CLI set contract yx.token $YOSEMITE_CONTRACTS_DIR/yx.token/
slee 2
$YOSEMITE_CLI push action yosemite setpriv '["yx.token",1]' -p yosemite@active
slee 2


{ print_section_title "Install DContract Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.dcontract $YOSEMITE_CONTRACTS_DIR/yx.dcontract/
sleep 2
$YOSEMITE_CLI push action yosemite setpriv '["yx.dcontract",1]' -p yosemite@active
sleep 2


{ print_section_title "Querying the status of System Depositories, Identity Authorities and Block Producers"; } 2>/dev/null

$YOSEMITE_CLI get table yosemite yosemite global
$YOSEMITE_CLI get table yosemite yosemite sysdepos
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

$YOSEMITE_CLI create account yosemite producer.a YOS5Audoa4mpZaYhp7vwYVCUnsQCUVifftdPipvkfZ9qVggoYoHUn -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.b YOS5aw9PzjxJCTi23FWtcB6Q8feMhfLg7Toh7PwGoWge4K4xNWQdm -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.c YOS8cvC5FJozTTVfUVXZ4E4kz1eNsKoBsnG7J76Fw1gX1wstGoUWo -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.d YOS6ig1G6hpk1Tzj1Ko8zfysATY4eqpb9znyEnx25zbkHscV6qHvy -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.e YOS72LDKqDc2KvyN1XeEYhv7AbkMUYB8B3fJ55yMn4ZqLzeqxz3w1 -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.f YOS5SzGAGCMznawLLY9xkpa4ta62CfTKu6di9AjZ9bWCBJ1pFu641 -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.g YOS5t1fHFunR2rWq5z8NHPrxj1H4xG5Vq4bGKcH33yg1eZMCVPQRq -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.h YOS5unphoov9UCG8AidDYv14fMJVRnHuihScASHbdFXHuDeDAS2s6 -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.i YOS5KM5t7td26VGDSEHXPHoUHB1Md1NnGRbT9EkXDaBf5nyhw18is -p yosemite@active
$YOSEMITE_CLI create account yosemite producer.j YOS5oduMFs5Lrbb8ZEc11KtyoVqfUjvaRxbUsQGgTqsEq18p1KqoC -p yosemite@active
sleep 1

$YOSEMITE_CLI push action yosemite regproducer '["producer.a","YOS5Audoa4mpZaYhp7vwYVCUnsQCUVifftdPipvkfZ9qVggoYoHUn","http://producera.io",1]' -p producer.a@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.b","YOS5aw9PzjxJCTi23FWtcB6Q8feMhfLg7Toh7PwGoWge4K4xNWQdm","http://producerb.io",1]' -p producer.b@active -p yosemite@active

$YOSEMITE_CLI push action yosemite authproducer '["producer.a"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.b"]' -p yosemite@active

$YOSEMITE_CLI push action yosemite regproducer '["producer.c","YOS8cvC5FJozTTVfUVXZ4E4kz1eNsKoBsnG7J76Fw1gX1wstGoUWo","http://producerc.io",1]' -p producer.c@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.d","YOS6ig1G6hpk1Tzj1Ko8zfysATY4eqpb9znyEnx25zbkHscV6qHvy","http://producerd.io",1]' -p producer.d@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.e","YOS72LDKqDc2KvyN1XeEYhv7AbkMUYB8B3fJ55yMn4ZqLzeqxz3w1","http://producere.io",1]' -p producer.e@active -p yosemite@active

$YOSEMITE_CLI push action yosemite authproducer '["producer.c"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.d"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.e"]' -p yosemite@active

$YOSEMITE_CLI push action yosemite regproducer '["producer.f","YOS5SzGAGCMznawLLY9xkpa4ta62CfTKu6di9AjZ9bWCBJ1pFu641","http://producerf.io",1]' -p producer.f@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.g","YOS5t1fHFunR2rWq5z8NHPrxj1H4xG5Vq4bGKcH33yg1eZMCVPQRq","http://producerg.io",1]' -p producer.g@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.h","YOS5unphoov9UCG8AidDYv14fMJVRnHuihScASHbdFXHuDeDAS2s6","http://producerh.io",1]' -p producer.h@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.i","YOS5KM5t7td26VGDSEHXPHoUHB1Md1NnGRbT9EkXDaBf5nyhw18is","http://produceri.io",1]' -p producer.i@active -p yosemite@active
$YOSEMITE_CLI push action yosemite regproducer '["producer.j","YOS5oduMFs5Lrbb8ZEc11KtyoVqfUjvaRxbUsQGgTqsEq18p1KqoC","http://producerj.io",1]' -p producer.j@active -p yosemite@active

$YOSEMITE_CLI push action yosemite authproducer '["producer.f"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.g"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.h"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.i"]' -p yosemite@active
$YOSEMITE_CLI push action yosemite authproducer '["producer.j"]' -p yosemite@active

sleep 180
tail -n 300 $YOSEMITE_NODE_LOG_FILE


{ print_section_title "Setup Initial Transaction Fees"; } 2>/dev/null

$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.newacc", "1000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.regprod", "3000000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.regsysdep", "2000000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.regidauth", "2000000.0000 DKRW" ]' -p yosemite@active

$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.nissue", "0.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.nredeem", "1000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.transfer", "100.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.ntransfer", "200.0000 DKRW" ]' -p yosemite@active

$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.tcreate", "10000.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.tissue", "500.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.tredeem", "500.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.ttransfer", "100.0000 DKRW" ]' -p yosemite@active

$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.dccreate", "500.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.dcaddsign", "100.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.dcsign", "300.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.dcupadd", "50.0000 DKRW" ]' -p yosemite@active
$YOSEMITE_CLI push action yx.txfee settxfee '[ "tf.dcremove", "0.0000 DKRW" ]' -p yosemite@active

$YOSEMITE_CLI get table -l 100 yx.txfee yx.txfee txfees
$YOSEMITE_CLI get table -L tf.transfer -l 1 yx.txfee yx.txfee txfees


{ print_section_title "Resign \"yosemite\" delegating authority to \"yx.prods\""; } 2>/dev/null

$YOSEMITE_CLI get account yosemite
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yosemite","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yx.prods","permission":"active"}}]}}' -p yosemite@owner
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yosemite","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yx.prods","permission":"active"}}]}}' -p yosemite@active
$YOSEMITE_CLI get account yosemite
$YOSEMITE_CLI get account yx.prods

sleep 1

{ print_section_title "Resign \"yx.msig\" delegating authority to \"yosemite\""; } 2>/dev/null

$YOSEMITE_CLI get account yx.msig
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.msig","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.msig@owner
$YOSEMITE_CLI push action yosemite updateauth '{"account":"yx.msig","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"yosemite","permission":"active"}}]}}' -p yx.msig@active
$YOSEMITE_CLI get account yx.msig

sleep 1

{ print_section_title "Resign \"yx.txfee\", \"yx.identity\", \"yx.ntoken\", \"yx.token\", \"yx.dcontract\" delegating authority to \"yosemite\""; } 2>/dev/null

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

sleep 1

{ print_section_title "Create New User Accounts"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5KTdP4VyuhZ1ba3sX8JvHjKXgcTFUA75QHf92nrGYQX4coVJRAW
$YOSEMITE_CLI wallet import --private-key YPV_5KDKMejP2AZFGCMxLh67t7417coEgUu7PfQZk7ufuYA4wTBiNC4
$YOSEMITE_CLI wallet import --private-key YPV_5Jj4ryu8TY1cVhiLvPkBHKiwMAXV18WqJM97h8hfsBUt5sMJE3r
$YOSEMITE_CLI wallet import --private-key YPV_5KWoErpjc9yQ2HF6rLuijLFgmAqVEoDLGWmDQAT6NM5DCgcSgo1
$YOSEMITE_CLI wallet import --private-key YPV_5JP9oWWsMaTEBcw1Y7Jn8JzxQBq51FDcGVGqCi5gpMDyKqio26j

$YOSEMITE_CLI create account idauth1 useraccount1 YOS7y8FkTHo58ZvrMa6yrTpo3D2Y2AA4dMAZzQ8aZY6R6geZSDZ75 -p idauth1@active
$YOSEMITE_CLI create account idauth1 useraccount2 YOS8ZFsmfpevP8pKPMXuQMNB7P9KJoKzZwwzu2uww9VrRx7E7NUQj -p idauth1@active
$YOSEMITE_CLI create account idauth1 useraccount3 YOS5ubmvsnjHviACtfc9SwGbY7SprqTu1P5GQeDfwfZmhCq6aR7GH -p idauth1@active

$YOSEMITE_CLI create account yosemite com YOS6vdcL347XXzpxSdnLUKpNwmA3KhYagcFqnZ3rNfKp96hzFxyit -p yosemite@active

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"com\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"1f32i7t23\"}" -p idauth1@active
$YOSEMITE_CLI push action yx.ntoken nissue '["com",{"amount":"5000000.0000 DKRW","issuer":"sysdepo1"},"nissue com"]' -p sysdepo1@active
$YOSEMITE_CLI create account com acquire.com YOS6ibAshrW7QmXeM5gurexmw6ijwM9d1BYS1J6Y1kevAFf7PcLNh -p com@active

$YOSEMITE_CLI get table yx.ntoken sysdepo1 ntaccounts
$YOSEMITE_CLI get table yx.ntoken sysdepo1 ntstats
$YOSEMITE_CLI get table yx.ntoken com ntaccounts
$YOSEMITE_CLI get table yx.ntoken yx.txfee ntaccounts

$YOSEMITE_CLI get table -L com -l 1 yx.identity yx.identity identity

sleep 1

{ print_section_title "Managing Account Identity Info (including KYC)"; } 2>/dev/null

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccount1\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 0111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth1@active
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccount2\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"23uyiuye\"}" -p idauth1@active
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccount3\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"vewv23r3\"}" -p idauth1@active

$YOSEMITE_CLI get table -L useraccount1 -l 1 yx.identity yx.identity identity

$YOSEMITE_CLI push action yx.identity settype "{\"account\":\"useraccount1\", \"type\":$(echo 'ibase=2; 11111111' | bc)}" -p idauth1@active

$YOSEMITE_CLI push action yx.identity setkyc "{\"account\":\"useraccount1\", \"kyc\":$(echo 'ibase=2; 1111' | bc)}" -p idauth1@active

$YOSEMITE_CLI push action yx.identity setstate "{\"account\":\"useraccount1\", \"state\":$(echo 'ibase=2; 0001' | bc)}" -p idauth1@active

$YOSEMITE_CLI push action yx.identity setdata "{\"account\":\"useraccount1\", \"data\":\"23fiuygy3\"}" -p idauth1@active

sleep 1

{ print_section_title "Native Token Issue / Transfer"; } 2>/dev/null

$YOSEMITE_CLI push action yx.ntoken nissue '["useraccount2",{"amount":"100000.0000 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active
$YOSEMITE_CLI push action yx.ntoken nissue '["useraccount3",{"amount":"100000.0000 DKRW","issuer":"sysdepo1"},"nissue test"]' -p sysdepo1@active

$YOSEMITE_CLI push action yx.ntoken transfer '[ "useraccount2", "useraccount3", "10000.0000 DKRW", "memo" ]' -p useraccount2
$YOSEMITE_CLI push action yx.ntoken ntransfer '[ "useraccount2", "useraccount3", {"amount":"10000.0000 DKRW","issuer":"sysdepo1"}, "memo" ]' -p useraccount2

$YOSEMITE_CLI push action yx.ntoken wptransfer '[ "useraccount3", "useraccount2", "10000.0000 DKRW", "useraccount2", "memo" ]' -p useraccount3 -p useraccount2

$YOSEMITE_CLI get table yx.ntoken sysdepo1 ntstats
$YOSEMITE_CLI get table yx.ntoken useraccount1 ntaccounts
$YOSEMITE_CLI get table yx.ntoken useraccount2 ntaccounts
$YOSEMITE_CLI get table yx.ntoken useraccount3 ntaccounts
$YOSEMITE_CLI get table yx.ntoken yx.txfee ntaccounts
$YOSEMITE_CLI get table yx.ntoken sysdepo1 ntaccounts

sleep 1

{ print_section_title "Claim Producer Rewards"; } 2>/dev/null

sleep 3
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"producer.c\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"bp\"}" -p idauth1@active

$YOSEMITE_CLI push action yosemite claimrewards '["producer.c"]' -p producer.c@active

$YOSEMITE_CLI get table yosemite yosemite producers
$YOSEMITE_CLI get table yx.ntoken yx.txfee ntaccounts
$YOSEMITE_CLI get table yx.ntoken producer.c ntaccounts

sleep 1

{ print_section_title "Transaction Votes"; } 2>/dev/null

$YOSEMITE_CLI push action yx.ntoken transfer '[ "useraccount3", "useraccount2", "5000.0000 DKRW", "memo1" ]' -p useraccount3 -v producer.f
$YOSEMITE_CLI push action yx.ntoken transfer '[ "useraccount3", "useraccount2", "5000.0000 DKRW", "memo2" ]' -p useraccount3 -v producer.g
$YOSEMITE_CLI push action yx.ntoken transfer '[ "useraccount3", "useraccount2", "5000.0000 DKRW", "memo3" ]' -p useraccount3 -v producer.g
sleep 2

$YOSEMITE_CLI get table yosemite yosemite producers
sleep 1

{ print_section_title "Querying action data"; } 2>/dev/null

$YOSEMITE_CLI get actions yx.ntoken 0 9

$YOSEMITE_CLI get actions yx.ntoken 10 9

$YOSEMITE_CLI get actions yx.ntoken -1 -1

{ set +x; } 2>/dev/null
