#!/usr/bin/env bash
# chmod +x ./stop_yosemite_testnet_node.sh

YOSEMITE_HOME=/mnt/yosemite-public-blockchain-git
YOSEMITE_NODEOS=$YOSEMITE_HOME/build/programs/nodeos/nodeos
YOSEMITE_NODEOS_LOG_FILE=/mnt/nodeos.log
YOSEMITE_CLEOS=$YOSEMITE_HOME/build/programs/cleos/cleos
YOSEMITE_TESTNET_CLEOS="$YOSEMITE_HOME/build/programs/cleos/cleos -u http://testnet.yosemitelabs.org:8888"
YOSEMITE_KEOSD=$YOSEMITE_HOME/build/programs/keosd/keosd
YOSEMITE_KEOSD_LOG_FILE=/mnt/keosd.log
YOSEMITE_KEOSD_WALLET_PASSWORD=PW5KH7i8ZEuVMvywMschs3TznhTfCdmgpPBGKJLUQjs6N6oQ7boZj
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
echo "${red}[Stopping YOSEMITE Testnet]${reset}"
echo
echo "${green}YOSEMITE_HOME${reset}=${red}$YOSEMITE_HOME${reset}"
echo "${green}YOSEMITE_NODEOS${reset}=${red}$YOSEMITE_NODEOS${reset}"
echo "${green}YOSEMITE_NODEOS_LOG_FILE${reset}=${red}$YOSEMITE_NODEOS_LOG_FILE${reset}"
echo "${green}YOSEMITE_CLEOS${reset}=${red}$YOSEMITE_CLEOS${reset}"
echo "${green}YOSEMITE_TESTNET_CLEOS${reset}=${red}$YOSEMITE_TESTNET_CLEOS${reset}"
echo "${green}YOSEMITE_KEOSD${reset}=${red}$YOSEMITE_KEOSD${reset}"
echo "${green}YOSEMITE_KEOSD_LOG_FILE${reset}=${red}$YOSEMITE_KEOSD_LOG_FILE${reset}"
echo "${green}YOSEMITE_KEOSD_WALLET_PASSWORD${reset}=${red}$YOSEMITE_KEOSD_WALLET_PASSWORD${reset}"
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
echo "${red}Really want to stop Testnet?${reset}"
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

{ print_section_title "Stop nodeos node"; } 2>/dev/null

pgrep nodeos
pkill -SIGINT nodeos
sleep 2
tail $YOSEMITE_NODEOS_LOG_FILE

{ print_section_title "Stop mongodb"; } 2>/dev/null

pgrep mongod
pkill -SIGINT mongod
sleep 2

{ print_section_title "Stop keosd"; } 2>/dev/null

pgrep keosd
pkill -SIGINT keosd
sleep 5
tail $YOSEMITE_KEOSD_LOG_FILE


{ set +x; } 2>/dev/null