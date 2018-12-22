#!/usr/bin/env bash
# chmod +x ./reset_std_token_yosemite_testnet_on_server_side.sh

YOSEMITE_NODE_BIN_NAME=yosemite
YOSEMITE_CLI_BIN_NAME=clyos
YOSEMITE_KEYD_BIN_NAME=keyos
YOSEMITE_HOME=/mnt/yosemite-public-blockchain-git
YOSEMITE_NODE=$YOSEMITE_HOME/build/programs/$YOSEMITE_NODE_BIN_NAME/$YOSEMITE_NODE_BIN_NAME
YOSEMITE_NODE_LOG_FILE=/mnt/$YOSEMITE_NODE_BIN_NAME.log
YOSEMITE_CLI="$YOSEMITE_HOME/build/programs/$YOSEMITE_CLI_BIN_NAME/$YOSEMITE_CLI_BIN_NAME --wallet-url http://127.0.0.1:8900/"
YOSEMITE_CLI_TESTNET="$YOSEMITE_CLI -u http://testnet.yosemitelabs.org:8888 --wallet-url http://127.0.0.1:8900/"
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

$YOSEMITE_CLI create account yosemite yx.tokenabi YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3 --fee-payer yosemite
$YOSEMITE_CLI create account yosemite yx.txfee YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3 --fee-payer yosemite
$YOSEMITE_CLI create account yosemite yx.msig YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3 --fee-payer yosemite
$YOSEMITE_CLI create account yosemite yx.identity YOS7qFXz5bvLYphF8S8XXTYgExnN2hXRBSMHTXs8oartApBx5upR3 --fee-payer yosemite


{ print_section_title "Create initial Identity Authority Account"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5K5Qe2ndgcfhWqdUjHnfhvCKVqCE82Rih6JrNQuuVJBcathAx9m
$YOSEMITE_CLI create account yosemite idauth.a YOS6EfGUaA5MNLH1GiHd64DcDr3HMgY1AM3WR1vdHKaah9Z4cWPZq --fee-payer yosemite

$YOSEMITE_CLI wallet import --private-key YPV_5JRzPhyg9jY4thDJ1rLsSr4zdLLoA7UEMQRw3vJFGmGKTZF5Kbx
$YOSEMITE_CLI create account yosemite idauth.b YOS5VvWLCqXL3AQyuXZfUTnEiWuhJNV6Nq3YbbS7Z98cN7GiRo9LR --fee-payer yosemite



{ print_section_title "Create initial System Token Account"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5J8WZRwfYVzsH8i3LaRQLGorVD788RFYqALEsLmeK8a6Kq6re2W
$YOSEMITE_CLI create account yosemite systoken.a YOS6o8hwGio5V2LgBxFMZi4bdVG5AvcHeFiMpaREKuZ29BwXpNTpH --fee-payer yosemite

$YOSEMITE_CLI wallet import --private-key YPV_5K5MBizJ6HuG1kdvQTeMnpKjpFSZHm64Nj3X6MvJR6AnTF8mHkx
$YOSEMITE_CLI create account yosemite systoken.b YOS61njg9Zs9oQmAmwLnrcVBrniWKshKAhoQJJqNVVnYXCPSRKLTE --fee-payer yosemite


{ print_section_title "Install YOSEMITE System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yosemite $YOSEMITE_CONTRACTS_DIR/yx.system/ -p yosemite@active --fee-payer yosemite
sleep 2


{ print_section_title "Install Standard Token ABI Interface System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.tokenabi $YOSEMITE_CONTRACTS_DIR/yx.tokenabi/ -p yx.tokenabi@active -p yosemite@active --fee-payer yosemite
sleep 2


{ print_section_title "Install Multi-sig System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.msig $YOSEMITE_CONTRACTS_DIR/eosio.msig/ -p yx.msig@active -p yosemite@active --fee-payer yosemite
sleep 2
$YOSEMITE_CLI push action yosemite setpriv '["yx.msig",1]' -p yosemite@active --fee-payer yosemite
sleep 2


{ print_section_title "Install Identity System Contract"; } 2>/dev/null

sleep 2
$YOSEMITE_CLI set contract yx.identity $YOSEMITE_CONTRACTS_DIR/yx.identity/ -p yx.identity@active -p yosemite@active --fee-payer yosemite
sleep 2


{ print_section_title "Register Initial Identity Authorities"; } 2>/dev/null

$YOSEMITE_CLI push action yosemite regidauth '["idauth.a","http://idauth-a.org",1]' -p idauth.a@active -p yosemite@active --fee-payer yosemite
$YOSEMITE_CLI push action yosemite authidauth '["idauth.a"]' -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI push action yosemite regidauth '["idauth.b","http://idauth-b.org",1]' -p idauth.b@active -p yosemite@active --fee-payer yosemite
$YOSEMITE_CLI push action yosemite authidauth '["idauth.b"]' -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI get table yosemite yosemite idauthority

sleep 2


{ print_section_title "Querying the status of Identity Authorities and Block Producers"; } 2>/dev/null

$YOSEMITE_CLI get table yosemite yosemite global
$YOSEMITE_CLI get table yosemite yosemite idauthority
$YOSEMITE_CLI get table yosemite yosemite producers


{ print_section_title "Setup Initial System Tokens"; } 2>/dev/null

$YOSEMITE_CLI push action systoken.a settokenmeta '["2,DKRW","http://sysdepo-a.org","system depository a"]' -p systoken.a@active -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI push action systoken.a issue '["systoken.a","systoken.a","100000.00 DKRW","issue systoken.a"]' -p systoken.a@active -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI get token info systoken.a
$YOSEMITE_CLI get token balance systoken.a systoken.a

$YOSEMITE_CLI push action systoken.b settokenmeta '["2,DKRWB","http://sysdepo-b.org","system depository b"]' -p systoken.b@active -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI push action systoken.b issue '["systoken.b","systoken.b","200000.00 DKRWB","issue systoken.b"]' -p systoken.b@active -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI get token info systoken.b
$YOSEMITE_CLI get token balance systoken.b systoken.b

$YOSEMITE_CLI push action yosemite addsystoken '["systoken.a",10000]' -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI push action yosemite addsystoken '["systoken.b",10000]' -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI get systoken list


{ print_section_title "Setup Initial Transaction Fees for Built-in Actions"; } 2>/dev/null

$YOSEMITE_CLI push action yosemite settxfee '["yosemite","newaccount",50000,1]' -p yosemite@active --fee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["","issue",5000,1]' -p yosemite@active --fee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["","transfer",10000,1]' -p yosemite@active --fee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["","redeem",20000,1]' -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '["yx.identity","setidinfo",0,1]' -p yosemite@active --fee-payer yosemite


{ print_section_title "Deploy YOSEMITE Fiat Stable Coin as a System Token"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5K9aJSJyMhNLWvAtYm6x3ZYL13eaToVeT6JjQ1ABkQd383gXHFa
$YOSEMITE_CLI create account yosemite ysmt.dkrw YOS568YZYS8i2Gr6gbJE9CEEsyaZFfRXzVS9BNw9FfCLr9Yccpg7o -p yosemite@active --fee-payer yosemite

sleep 2
$YOSEMITE_CLI set contract ysmt.dkrw $YOSEMITE_CONTRACTS_DIR/yosemitex.fiat.stable.token/ -p ysmt.dkrw@active -p yosemite@active --fee-payer yosemite
sleep 2

$YOSEMITE_CLI push action ysmt.dkrw settokenmeta '["2,DKRW","https://yosemitex.com","Yosemite X Digital KRW Token"]' -p ysmt.dkrw@active -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"ysmt.dkrw\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"ysmt.dkrw\"}" -p idauth.a@active --fee-payer idauth.a
$YOSEMITE_CLI push action ysmt.dkrw issue '["ysmt.dkrw","ysmt.dkrw","10000000.00 DKRW",""]' -p ysmt.dkrw@active -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '["ysmt.dkrw","issue",0,1]' -p yosemite@active --fee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["ysmt.dkrw","redeem",5000,1]' -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI push action yosemite addsystoken '["ysmt.dkrw",10000]' -p yosemite@active --fee-payer yosemite



{ print_section_title "YOSEMITE Standard Token Test"; } 2>/dev/null

$YOSEMITE_CLI push action systoken.a issue '["systoken.a","idauth.a","1000000.00 DKRW",""]' -p systoken.a@active -fee-payer systoken.a

$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","idauth.a","idauth.b","5000.00 DKRW","transfer tag"]' -p idauth.a@active --fee-payer idauth.a
$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","idauth.a","idauth.b","3000.00 DKRW","transfer tag 2"]' -p idauth.a@active -p idauth.b@active --fee-payer idauth.b

$YOSEMITE_CLI get token balance systoken.a idauth.a
$YOSEMITE_CLI get token balance systoken.a idauth.b

$YOSEMITE_CLI get token balance systoken.a systoken.a
$YOSEMITE_CLI push action systoken.a redeem '["50000.00 DKRW","redeem tag"]' -p systoken.a@active --fee-payer systoken.a

$YOSEMITE_CLI wallet import --private-key YPV_5KTdP4VyuhZ1ba3sX8JvHjKXgcTFUA75QHf92nrGYQX4coVJRAW
$YOSEMITE_CLI wallet import --private-key YPV_5KDKMejP2AZFGCMxLh67t7417coEgUu7PfQZk7ufuYA4wTBiNC4
$YOSEMITE_CLI wallet import --private-key YPV_5Jj4ryu8TY1cVhiLvPkBHKiwMAXV18WqJM97h8hfsBUt5sMJE3r

$YOSEMITE_CLI create account idauth.a useraccounta YOS7y8FkTHo58ZvrMa6yrTpo3D2Y2AA4dMAZzQ8aZY6R6geZSDZ75 -p idauth.a@active --fee-payer idauth.a
$YOSEMITE_CLI create account idauth.a useraccountb YOS8ZFsmfpevP8pKPMXuQMNB7P9KJoKzZwwzu2uww9VrRx7E7NUQj -p idauth.a@active --fee-payer idauth.a
$YOSEMITE_CLI create account idauth.a useraccountc YOS5ubmvsnjHviACtfc9SwGbY7SprqTu1P5GQeDfwfZmhCq6aR7GH -p idauth.a@active --fee-payer idauth.a

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccounta\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"qwefewff2f\"}" -p idauth.a@active --fee-payer idauth.a
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccountb\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"wef2ouih23\"}" -p idauth.a@active --fee-payer idauth.a
$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"useraccountc\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 0111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"kh23r23ruy\"}" -p idauth.a@active --fee-payer idauth.a

$YOSEMITE_CLI push action ysmt.dkrw issue '["ysmt.dkrw","useraccounta","5000000.00 DKRW",""]' -p ysmt.dkrw@active --fee-payer ysmt.dkrw
$YOSEMITE_CLI push action ysmt.dkrw issue '["ysmt.dkrw","useraccountb","2000000.00 DKRW",""]' -p ysmt.dkrw@active --fee-payer ysmt.dkrw
#$YOSEMITE_CLI push action ysmt.dkrw issue '["ysmt.dkrw","useraccountc","1000000.00 DKRW",""]' -p ysmt.dkrw@active --fee-payer ysmt.dkrw

$YOSEMITE_CLI get token balance ysmt.dkrw useraccounta
$YOSEMITE_CLI get token balance ysmt.dkrw useraccountb

$YOSEMITE_CLI push action ysmt.dkrw transfer '["ysmt.dkrw","useraccounta","useraccountb","10000.00 DKRW","transfer tag"]' -p useraccounta@active --fee-payer useraccounta



$YOSEMITE_CLI push action systoken.a issue '["systoken.a","useraccounta","100000.00 DKRW",""]' -p systoken.a@active --fee-payer systoken.a
$YOSEMITE_CLI push action systoken.b issue '["systoken.b","useraccounta","100000.00 DKRWB",""]' -p systoken.b@active --fee-payer systoken.b

$YOSEMITE_CLI push action systoken.a issue '["systoken.a","useraccountb","200000.00 DKRW",""]' -p systoken.a@active --fee-payer systoken.a
$YOSEMITE_CLI push action systoken.a issue '["systoken.a","useraccountc","300000.00 DKRW",""]' -p systoken.a@active --fee-payer systoken.a

$YOSEMITE_CLI get token balance systoken.a useraccounta
$YOSEMITE_CLI get token balance systoken.b useraccounta
#$YOSEMITE_CLI get systoken balance useraccounta

$YOSEMITE_CLI get token balance systoken.a useraccountb
$YOSEMITE_CLI get token balance systoken.a useraccountc

$YOSEMITE_CLI push action systoken.a transfer '["systoken.a","useraccounta","useraccountb","99950.00 DKRW","transfer tag"]' -p useraccounta@active --fee-payer useraccounta



{ print_section_title "YOSEMITE Credit Token Test"; } 2>/dev/null

$YOSEMITE_CLI wallet import --private-key YPV_5KKMcPu86p6vbsuAozTT9vLxUT4GeZFqSWrbgok1LjpthJEB5Qe
$YOSEMITE_CLI create account yosemite ycardcredit YOS7mK7vvTTCDzEr8gtMjrMsbWA25UsS3WmV8WRoqC5gSMh9JLcQM -p yosemite@active --fee-payer yosemite

sleep 2
$YOSEMITE_CLI set contract ycardcredit $YOSEMITE_CONTRACTS_DIR/yosemitex.credit.token/ -p ycardcredit@active -p yosemite@active --fee-payer yosemite
sleep 2

$YOSEMITE_CLI push action ycardcredit settokenmeta '["2,CRD","https://yosemitecardx.com","Yosemite Card X Credit Token"]' -p ycardcredit@active -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '["ycardcredit","creditlimit",1000,1]' -p yosemite@active --fee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["ycardcredit","creditissue",3000,1]' -p yosemite@active --fee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '["ycardcredit","creditredeem",5000,1]' -p yosemite@active --fee-payer yosemite

$YOSEMITE_CLI push action yx.identity setidinfo "{\"account\":\"ycardcredit\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"if3fhu3eu\"}" -p idauth.a@active --fee-payer idauth.a
$YOSEMITE_CLI push action ysmt.dkrw issue '["ysmt.dkrw","ycardcredit","7000000.00 DKRW",""]' -p ysmt.dkrw@active --fee-payer ysmt.dkrw

$YOSEMITE_CLI get token balance ysmt.dkrw ycardcredit

## issuing credit card to useraccounta by offering credit limit
$YOSEMITE_CLI push action ycardcredit creditlimit '["useraccounta","50000.00 CRD","vhw93rh23r"]' -p ycardcredit@active --fee-payer ycardcredit

$YOSEMITE_CLI get table -L useraccounta -l 1 ycardcredit ycardcredit creditoffer

## useraccounta issues credit tokens and deposits to ycard service
$YOSEMITE_CLI push action ycardcredit creditissue '["useraccounta","50000.00 CRD","issue credit token by user"]' -p useraccounta@active -p ycardcredit@active --fee-payer ycardcredit

$YOSEMITE_CLI get table -L useraccounta -l 1 ycardcredit ycardcredit creditoffer
$YOSEMITE_CLI get token balance ycardcredit useraccounta

$YOSEMITE_CLI push action ycardcredit transfer '["ycardcredit","useraccounta","ycardcredit","50000.00 CRD","deposit credit to service account for anonymous tx"]' -p useraccounta@active -p ycardcredit@active --fee-payer ycardcredit

$YOSEMITE_CLI get token balance ycardcredit ycardcredit

## anonymous payment transaction to merchant useraccountb
$YOSEMITE_CLI push action ycardcredit transfer '["ycardcredit","ycardcredit","useraccountb","10000.00 CRD","payment tx"]' -p ycardcredit@active --fee-payer ycardcredit
$YOSEMITE_CLI push action ycardcredit transfer '["ycardcredit","ycardcredit","useraccountb","10000.00 CRD","payment tx"]' -p ycardcredit@active --fee-payer ycardcredit

$YOSEMITE_CLI get token balance ycardcredit useraccountb

## monthly user credit balance settlement
$YOSEMITE_CLI push action ysmt.dkrw issue '["ysmt.dkrw","ycardcredit","20000.00 DKRW","useraccounta credit settlement"]' -p ysmt.dkrw@active --fee-payer ysmt.dkrw

$YOSEMITE_CLI push action ycardcredit redeem '["30000.00 CRD","burn unused useraccounta credit"]' -p ycardcredit@active --fee-payer ycardcredit

$YOSEMITE_CLI push action ycardcredit creditredeem '["useraccounta","50000.00 CRD","reset useraccounta credit offering"]' -p ycardcredit@active --fee-payer ycardcredit

$YOSEMITE_CLI push action ycardcredit creditissue '["useraccounta","50000.00 CRD","issue credit token by user"]' -p useraccounta@active -p ycardcredit@active --fee-payer ycardcredit

$YOSEMITE_CLI push action ycardcredit transfer '["ycardcredit","useraccounta","ycardcredit","50000.00 CRD","deposit credit to service account for anonymous tx"]' -p useraccounta@active -p ycardcredit@active --fee-payer ycardcredit

## merchant(useraccountb) redeems credit tokens and withdraws fiat
$YOSEMITE_CLI push action ycardcredit transfer '["ycardcredit","useraccountb","ycardcredit","20000.00 CRD","request redeeming credit token"]' -p useraccountb@active -p ycardcredit@active --fee-payer ycardcredit

$YOSEMITE_CLI push action ysmt.dkrw transfer '["ysmt.dkrw","ycardcredit","useraccountb","20000.00 DKRW","redeeming CRD to DKRW"]' -p ycardcredit@active --fee-payer ycardcredit

$YOSEMITE_CLI push action ycardcredit redeem '["20000.00 CRD","burn redeemed credit tokens"]' -p ycardcredit@active --fee-payer ycardcredit

$YOSEMITE_CLI get token balance ycardcredit useraccountb
$YOSEMITE_CLI get token balance ysmt.dkrw useraccountb

$YOSEMITE_CLI push action ysmt.dkrw transfer '["ysmt.dkrw","useraccountb","ysmt.dkrw","20000.00 DKRW","request redeeming DKRW to fiat"]' -p useraccountb@active -p ycardcredit@active --fee-payer ycardcredit
$YOSEMITE_CLI push action ysmt.dkrw redeem '["20000.00 DKRW","redeem DKRW to fiat"]' -p ysmt.dkrw@active --fee-payer ysmt.dkrw