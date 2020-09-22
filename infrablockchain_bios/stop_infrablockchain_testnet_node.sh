#!/usr/bin/env bash
# chmod +x ./stop_infrablockchain_testnet_node.sh

#YSMT_TESTNET_SSH_KEY_FILE=~/Documents/__HalfDomeChain__/AWS/ssh_key/ysmt_testnet_dev_server_ap_northeast_seoul.pem
#YSMT_TESTNET_NODE_USER_HOST=ubuntu@ec2-13-124-23-23.ap-northeast-2.compute.amazonaws.com
#scp -i ${YSMT_TESTNET_SSH_KEY_FILE} ./stop_infrablockchain_testnet_node.sh ${YSMT_TESTNET_NODE_USER_HOST}:/mnt/infrablockchain_testnet_mgmt/stop_infrablockchain_testnet_node.sh

setopt shwordsplit

INFRA_NODE_BIN_NAME=infra-node
INFRA_CLI_BIN_NAME=infra-cli
INFRA_KEYSTORE_BIN_NAME=infra-keystore
INFRABLOCKCHAIN_HOME=/mnt/ifnrablockchain-git
INFRA_NODE=$INFRABLOCKCHAIN_HOME/build/programs/$INFRA_NODE_BIN_NAME/$INFRA_NODE_BIN_NAME
INFRA_NODE_LOG_FILE=/mnt/$INFRA_NODE_BIN_NAME.log
INFRA_CLI="$INFRABLOCKCHAIN_HOME/build/programs/$INFRA_CLI_BIN_NAME/$INFRA_CLI_BIN_NAME --wallet-url http://127.0.0.1:8900/"
INFRA_CLI_TESTNET="$INFRA_CLI -u http://testnet-sentinel.yosemitelabs.org:8888"
INFRA_CLI_TESTNET_V1="$INFRA_CLI -u http://testnet.yosemitelabs.org:8888"
INFRA_KEYSTORE=$INFRABLOCKCHAIN_HOME/build/programs/$INFRA_KEYSTORE_BIN_NAME/$INFRA_KEYSTORE_BIN_NAME
INFRA_KEYSTORE_LOG_FILE=/mnt/$INFRA_KEYSTORE_BIN_NAME.log
INFRA_KEYSTORE_WALLET_PASSWORD=PW5Jrpn9S5ygoA9r2bv47rv9gH2jVjntdyHWKL4QdoKVFRz6U17EM
INFRA_NODE_CONFIG=$INFRABLOCKCHAIN_HOME/infrablockchain_config/config_infrablockchain_testnet_boot.ini
INFRA_NODE_GENESIS_JSON=$INFRABLOCKCHAIN_HOME/infrablockchain_config/genesis_infrablockchain_testnet.json
INFRA_NODE_DATA_DIR=/mnt/INFRA_NODE_data
INFRABLOCKCHAIN_DEV_WALLET_DIR=/mnt/infrablockchain_dev_wallet
INFRABLOCKCHAIN_CONTRACTS_DIR=$INFRABLOCKCHAIN_HOME/build/contracts
INFRABLOCKCHAIN_MONGOD=/home/ubuntu/opt/mongodb/bin/mongod
INFRABLOCKCHAIN_MONGODB_CONFIG=/home/ubuntu/opt/mongodb/mongod.conf
INFRABLOCKCHAIN_MONGODB_DATA_DIR=/mnt/mongodb

{ set +x; } 2>/dev/null
red=`tput setaf 1`
green=`tput setaf 2`
magenta=`tput setaf 6`
reset=`tput sgr0`
set -x

{ set +x; } 2>/dev/null
echo "${red}[Resetting INFRABLOCKCHAIN Testnet]${reset}"
echo
echo "${green}INFRABLOCKCHAIN_HOME${reset}=${red}$INFRABLOCKCHAIN_HOME${reset}"
echo "${green}INFRA_NODE${reset}=${red}$INFRA_NODE${reset}"
echo "${green}INFRA_NODE_LOG_FILE${reset}=${red}$INFRA_NODE_LOG_FILE${reset}"
echo "${green}INFRA_CLI${reset}=${red}$INFRA_CLI${reset}"
echo "${green}INFRA_CLI_TESTNET${reset}=${red}$INFRA_CLI_TESTNET${reset}"
echo "${green}INFRA_KEYSTORE${reset}=${red}$INFRA_KEYSTORE${reset}"
echo "${green}INFRA_KEYSTORE_LOG_FILE${reset}=${red}$INFRA_KEYSTORE_LOG_FILE${reset}"
echo "${green}INFRA_KEYSTORE_WALLET_PASSWORD${reset}=${red}$INFRA_KEYSTORE_WALLET_PASSWORD${reset}"
echo "${green}INFRA_NODE_CONFIG${reset}=${red}$INFRA_NODE_CONFIG${reset}"
echo "${green}INFRA_NODE_GENESIS_JSON${reset}=${red}$INFRA_NODE_GENESIS_JSON${reset}"
echo "${green}INFRA_NODE_DATA_DIR${reset}=${red}$INFRA_NODE_DATA_DIR${reset}"
echo "${green}INFRABLOCKCHAIN_DEV_WALLET_DIR${reset}=${red}$INFRABLOCKCHAIN_DEV_WALLET_DIR${reset}"
echo "${green}INFRABLOCKCHAIN_CONTRACTS_DIR${reset}=${red}$INFRABLOCKCHAIN_CONTRACTS_DIR${reset}"
echo "${green}INFRABLOCKCHAIN_MONGOD${reset}=${red}$INFRABLOCKCHAIN_MONGOD${reset}"
echo "${green}INFRABLOCKCHAIN_MONGODB_CONFIG${reset}=${red}$INFRABLOCKCHAIN_MONGODB_CONFIG${reset}"
echo "${green}INFRABLOCKCHAIN_MONGODB_DATA_DIR${reset}=${red}$INFRABLOCKCHAIN_MONGODB_DATA_DIR${reset}"
echo
set -x

{ set +x; } 2>/dev/null
echo "${red}Really want to stop InfraBlockchain Testnet node?${reset}"
echo "write YES to proceed stop process."
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

{ print_section_title "Stop infra-node"; } 2>/dev/null

pgrep $INFRA_NODE_BIN_NAME
pkill -SIGINT $INFRA_NODE_BIN_NAME
sleep 2
tail $INFRA_NODE_LOG_FILE

{ print_section_title "Stop mongodb"; } 2>/dev/null

pgrep mongod
pkill -SIGINT mongod
sleep 2

{ print_section_title "Stop key daemon"; } 2>/dev/null

pgrep $INFRA_KEYSTORE_BIN_NAME
pkill -SIGINT $INFRA_KEYSTORE_BIN_NAME
sleep 5
tail $INFRA_KEYSTORE_LOG_FILE


{ set +x; } 2>/dev/null