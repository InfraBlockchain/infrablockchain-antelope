#!/usr/bin/env bash
# chmod +x ./local_testnet_boot_bios_infrablockchain.sh

setopt shwordsplit

export SCRIPT_DIR=$(dirname $(realpath "$0"))
export INFRABLOCKCHAIN_HOME=$(realpath "$SCRIPT_DIR/..")
#export INFRABLOCKCHAIN_HOME=/Users/bwlim/Documents/__InfraBlockchain__/infrablockchain-2-git

export INFRA_NODE_BIN_NAME=infra-node
export INFRA_CLI_BIN_NAME=infra-cli
export INFRA_KEYCHAIN_BIN_NAME=infra-keychain

export INFRA_BLOCKCHAIN_DATA_BASE_DIR=/Users/bwlim/Documents/__InfraBlockchain__/infrablockchain_local_testnet_data
export INFRA_BLOCKCHAIN_LOG_BASE_DIR=${INFRA_BLOCKCHAIN_DATA_BASE_DIR}/log

export INFRA_NODE=${INFRABLOCKCHAIN_HOME}/build/bin/$INFRA_NODE_BIN_NAME
export INFRA_NODE_LOG_FILE=${INFRA_BLOCKCHAIN_LOG_BASE_DIR}/$INFRA_NODE_BIN_NAME.log

export INFRA_CLI=($INFRABLOCKCHAIN_HOME/build/bin/$INFRA_CLI_BIN_NAME --wallet-url http://127.0.0.1:8900/)
export INFRA_KEYCHAIN=${INFRABLOCKCHAIN_HOME}/build/bin/$INFRA_KEYCHAIN_BIN_NAME
export INFRA_KEYCHAIN_LOG_FILE=${INFRA_BLOCKCHAIN_LOG_BASE_DIR}/$INFRA_KEYCHAIN_BIN_NAME.log
export INFRABLOCKCHAIN_DEV_WALLET_DIR=${INFRA_BLOCKCHAIN_DATA_BASE_DIR}/infrablockchain_dev_wallet
export INFRA_KEYCHAIN_WALLET_PASSWORD=PW5JcN2AfwXxAV12W1mofb7pbeyJEwwie4JsCaTZvMx5kt38P8TP1
export INFRA_KEYCHAIN_WALLET_NAME=local-testnet

export INFRA_NODE_CONFIG=${INFRABLOCKCHAIN_HOME}/infrablockchain-bios/config/config_infrablockchain_local_testnet.ini
export INFRA_NODE_GENESIS_JSON=${INFRABLOCKCHAIN_HOME}/infrablockchain-bios/config/genesis_infrablockchain_local_testnet.json
export INFRA_NODE_DATA_DIR=${INFRA_BLOCKCHAIN_DATA_BASE_DIR}/infra_node_data

export INFRABLOCKCHAIN_CONTRACTS_DIR=${INFRABLOCKCHAIN_HOME}/contracts/contracts

export SYS_ACCOUNT=eosio
#export SYS_ACCOUNT=infrasys

export INFRA_NODE_API_ENDPOINT=http://127.0.0.1:8888

{ set +x; } 2>/dev/null
red=`tput setaf 1`
green=`tput setaf 2`
magenta=`tput setaf 6`
reset=`tput sgr0`
set -x

{ set +x; } 2>/dev/null
echo "${red}[Resetting INFRABLOCKCHAIN Local Testnet]${reset}"
echo
echo "${green}INFRABLOCKCHAIN_HOME${reset}=${red}$INFRABLOCKCHAIN_HOME${reset}"
echo "${green}INFRA_NODE${reset}=${red}$INFRA_NODE${reset}"
echo "${green}INFRA_NODE_LOG_FILE${reset}=${red}$INFRA_NODE_LOG_FILE${reset}"
echo "${green}INFRA_CLI${reset}=${red}$INFRA_CLI${reset}"
echo "${green}INFRA_CLI_TESTNET${reset}=${red}$INFRA_CLI_TESTNET${reset}"
echo "${green}INFRA_KEYCHAIN${reset}=${red}$INFRA_KEYCHAIN${reset}"
echo "${green}INFRA_KEYCHAIN_LOG_FILE${reset}=${red}$INFRA_KEYCHAIN_LOG_FILE${reset}"
echo "${green}INFRA_KEYCHAIN_WALLET_PASSWORD${reset}=${red}$INFRA_KEYCHAIN_WALLET_PASSWORD${reset}"
echo "${green}INFRA_NODE_CONFIG${reset}=${red}$INFRA_NODE_CONFIG${reset}"
echo "${green}INFRA_NODE_GENESIS_JSON${reset}=${red}$INFRA_NODE_GENESIS_JSON${reset}"
echo "${green}INFRA_NODE_DATA_DIR${reset}=${red}$INFRA_NODE_DATA_DIR${reset}"
echo "${green}INFRABLOCKCHAIN_DEV_WALLET_DIR${reset}=${red}$INFRABLOCKCHAIN_DEV_WALLET_DIR${reset}"
echo "${green}INFRABLOCKCHAIN_CONTRACTS_DIR${reset}=${red}$INFRABLOCKCHAIN_CONTRACTS_DIR${reset}"
echo "${green}SYS_ACCOUNT${reset}=${red}$SYS_ACCOUNT${reset}"
echo "${green}INFRA_NODE_API_ENDPOINT${reset}=${red}INFRA_NODE_API_ENDPOINT${reset}"
echo
set -x

{ set +x; } 2>/dev/null
echo "${red}Really want to reset Testnet node data?${reset}"
echo "${red}[WARNING] all data including infra-node data and mongo db data will be deleted permanantely.${reset}"
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

mkdir -p ${INFRA_BLOCKCHAIN_DATA_BASE_DIR}
mkdir -p ${INFRA_BLOCKCHAIN_LOG_BASE_DIR}

{ print_section_title "Reset infra-node data"; } 2>/dev/null

pgrep $INFRA_NODE_BIN_NAME
pkill -SIGINT $INFRA_NODE_BIN_NAME
sleep 5

du -sh $INFRA_NODE_DATA_DIR
rm -rf $INFRA_NODE_DATA_DIR
mkdir $INFRA_NODE_DATA_DIR

sleep 5


{ print_section_title "Restart key daemon"; } 2>/dev/null

pgrep $INFRA_KEYCHAIN_BIN_NAME
pkill -SIGINT $INFRA_KEYCHAIN_BIN_NAME
sleep 5
nohup $INFRA_KEYCHAIN --unlock-timeout 999999999 --http-server-address 127.0.0.1:8900 --wallet-dir $INFRABLOCKCHAIN_DEV_WALLET_DIR > $INFRA_KEYCHAIN_LOG_FILE 2>&1&
sleep 10
tail -n 300 $INFRA_KEYCHAIN_LOG_FILE

#$INFRA_CLI wallet create -n ${INFRA_KEYCHAIN_WALLET_NAME} --to-console
#Creating wallet: local-testnet
#Save password to use in the future to unlock this wallet.
#Without password imported keys will not be retrievable.
#"PW5JcN2AfwXxAV12W1mofb7pbeyJEwwie4JsCaTZvMx5kt38P8TP1"
$INFRA_CLI wallet open -n ${INFRA_KEYCHAIN_WALLET_NAME}
$INFRA_CLI wallet unlock -n ${INFRA_KEYCHAIN_WALLET_NAME} --password $INFRA_KEYCHAIN_WALLET_PASSWORD

# infrablockchain initial key
#$INFRA_CLI create key --to-console
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_VwEM9o5KsqdLs5jgHucBcE3PFbu1kk3fH2wbjRGve4QPzvScR


{ print_section_title "Start infra-node from genesis"; } 2>/dev/null

nohup $INFRA_NODE --config $INFRA_NODE_CONFIG --genesis-json $INFRA_NODE_GENESIS_JSON --data-dir $INFRA_NODE_DATA_DIR --disable-replay-opts > $INFRA_NODE_LOG_FILE 2>&1&
sleep 10
tail -n 300 $INFRA_NODE_LOG_FILE
pgrep $INFRA_NODE_BIN_NAME
pkill -SIGINT $INFRA_NODE_BIN_NAME
sleep 5
$INFRA_NODE --print-genesis-json --config $INFRA_NODE_CONFIG --data-dir $INFRA_NODE_DATA_DIR

nohup $INFRA_NODE --config $INFRA_NODE_CONFIG --data-dir $INFRA_NODE_DATA_DIR --disable-replay-opts > $INFRA_NODE_LOG_FILE 2>&1&
sleep 10
tail -n 300 $INFRA_NODE_LOG_FILE


# Activate the PREACTIVATE_FEATURE protocol
curl -X POST ${INFRA_NODE_API_ENDPOINT}/v1/producer/schedule_protocol_feature_activations -d '{"protocol_features_to_activate": ["0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd"]}'

{ print_section_title "Install InfraBlockchain BOOT Contract"; } 2>/dev/null

sleep 2
$INFRA_CLI set contract ${SYS_ACCOUNT} $INFRABLOCKCHAIN_CONTRACTS_DIR/eosio.boot/bin eosio.boot.wasm eosio.boot.abi -p ${SYS_ACCOUNT}@active
sleep 2

# Activate Protocol Feature for InfraBlockchain
# BUILTIN_STANDARD_TOKEN
$INFRA_CLI push action ${SYS_ACCOUNT} activate '["6ce3a43d696d3f333e534e9a75a57c53e94143b23d76cda203c8bb73d9b48e8b"]' -p ${SYS_ACCOUNT}@active

sleep 3

# SYSTEM_TOKEN_TRANSACTION_FEE_PAYMENT_PROTOCOL
$INFRA_CLI push action ${SYS_ACCOUNT} activate '["8e464e632461e194815960d36f7f04863bffada875759cb37835e1f7f48c61ef"]' -p ${SYS_ACCOUNT}@active

sleep 3

# PROOF_OF_TRANSACTION_PROTOCOL
$INFRA_CLI push action ${SYS_ACCOUNT} activate '["506f97ad585aeda03ad21e15fba886cfdae0d371d7e58c3ac05cedfbc6d121f3"]' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

sleep 3

{ print_section_title "Create System Accounts"; } 2>/dev/null

$INFRA_CLI create account ${SYS_ACCOUNT} sys.tokenabi PUB_K1_73E1bUPyVMTb7gjvvhk3bhR1YnKi6TtzgJUL3Ykea8QGU1ZLfR -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI create account ${SYS_ACCOUNT} sys.txfee PUB_K1_73E1bUPyVMTb7gjvvhk3bhR1YnKi6TtzgJUL3Ykea8QGU1ZLfR -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
#$INFRA_CLI create account ${SYS_ACCOUNT} sys.msig PUB_K1_73E1bUPyVMTb7gjvvhk3bhR1YnKi6TtzgJUL3Ykea8QGU1ZLfR -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
#$INFRA_CLI create account ${SYS_ACCOUNT} sys.identity PUB_K1_73E1bUPyVMTb7gjvvhk3bhR1YnKi6TtzgJUL3Ykea8QGU1ZLfR -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}


{ print_section_title "Install InfraBlockchain System Contract"; } 2>/dev/null

sleep 2
$INFRA_CLI set contract ${SYS_ACCOUNT} $INFRABLOCKCHAIN_CONTRACTS_DIR/infra.system/bin infra.system.wasm infra.system.abi -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
sleep 2


{ print_section_title "Install Standard Token ABI Interface System Contract"; } 2>/dev/null

sleep 2
$INFRA_CLI set contract sys.tokenabi $INFRABLOCKCHAIN_CONTRACTS_DIR/sys.tokenabi/bin sys.tokenabi.wasm sys.tokenabi.abi -p sys.tokenabi@active --txfee-payer ${SYS_ACCOUNT}
sleep 2


{ print_section_title "Create initial System Token Account"; } 2>/dev/null

$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_4pDTqEyBnCCacqUfE6RgRWmssghAHU497eDqLg9ZobcnBYWQx
$INFRA_CLI create account ${SYS_ACCOUNT} systoken.a PUB_K1_7DoMEdoWG4C9sf5B84HpG4k8MrnF1dcdsFTmk1Q348cn4XukEP -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2MPBZmG3HxvzYdFJB1h3sR1HWcXe6rJUxuG9Uf52smrqLUaoYX
$INFRA_CLI create account ${SYS_ACCOUNT} systoken.b PUB_K1_6XAnMYrHi4fiVDkj4F6iV3ujgvNpYqQBViQr8pgFxi4Zv9muWg -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}



#{ print_section_title "Install Multi-sig System Contract"; } 2>/dev/null
#
#sleep 2
#$INFRA_CLI set contract sys.msig $INFRABLOCKCHAIN_CONTRACTS_DIR/eosio.msig/ -p sys.msig@active --txfee-payer ${SYS_ACCOUNT}
#sleep 2
#$INFRA_CLI push action ${SYS_ACCOUNT} setpriv '{"account":"sys.msig","is_priv":1}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
#sleep 2




{ print_section_title "Querying the status of Identity Authorities and Block Producers"; } 2>/dev/null

$INFRA_CLI get table ${SYS_ACCOUNT} ${SYS_ACCOUNT} global
$INFRA_CLI get table ${SYS_ACCOUNT} ${SYS_ACCOUNT} producers


{ print_section_title "Setup Initial System Tokens"; } 2>/dev/null

$INFRA_CLI push action systoken.a settokenmeta '{"sym":"4,DUSD","url":"http://sysdepo-a.org","desc":"system depository a"}' -p systoken.a@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"systoken.a","qty":"1000.0000 DUSD","tag":"issue systoken.a"}' -p systoken.a@active --txfee-payer ${SYS_ACCOUNT}
sleep 1

$INFRA_CLI get token info systoken.a
$INFRA_CLI get token balance systoken.a systoken.a
#$INFRA_CLI get token balance systoken.a idauth.a
#$INFRA_CLI get token balance systoken.a idauth.b

$INFRA_CLI push action systoken.b settokenmeta '{"sym":"4,DUSDB","url":"http://sysdepo-b.org","desc":"system depository b"}' -p systoken.b@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI push action systoken.b issue '{"t":"systoken.b","to":"systoken.b","qty":"2000.0000 DUSDB","tag":"issue systoken.b"}' -p systoken.b@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI get token info systoken.b
$INFRA_CLI get token balance systoken.b systoken.b

$INFRA_CLI push action ${SYS_ACCOUNT} addsystoken '{"token":"systoken.a","weight":10000}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI push action ${SYS_ACCOUNT} addsystoken '{"token":"systoken.b","weight":10000}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
sleep 2

$INFRA_CLI get systoken list

$INFRA_CLI get systoken balance systoken.a
$INFRA_CLI get systoken balance systoken.b

## ID Auth accounts
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2So2Tt7dAoMR6tk9npz1WrnbAgHadbEhbmkLChRvggduxnwxAn
$INFRA_CLI create account ${SYS_ACCOUNT} idauth.a PUB_K1_8QZSZGRFMiXk9GGEc7jPsyqZVF1CkJh16vHmnVt4PopkRZeyZJ -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2mrT8fELciM7qUvg7TSBG7SUVgvRL3gzATRMkY2JsJn1GMk2A5
$INFRA_CLI create account ${SYS_ACCOUNT} idauth.b PUB_K1_8acHA5kYZd9ktLC7zHWJcH8h63Zrt19kQbe11ASCobb7NNvZaq -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.a","qty":"5000.0000 DUSD","tag":"issue systoken.a to idauth.a"}' -p systoken.a@active --txfee-payer systoken.a
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"5000.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a

$INFRA_CLI get systoken balance idauth.a
$INFRA_CLI get systoken balance idauth.b


{ print_section_title "Setup Initial Transaction Fees for Built-in Actions"; } 2>/dev/null

$INFRA_CLI push action ${SYS_ACCOUNT} settxfee '{"code":"","action":"","value":1000,"feetype":1}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} settxfee '{"code":"","action":"issue","value":50,"feetype":1}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} settxfee '{"code":"","action":"transfer","value":100,"feetype":1}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} settxfee '{"code":"","action":"redeem","value":200,"feetype":1}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI push action ${SYS_ACCOUNT} settxfee "{\"code\":\"${SYS_ACCOUNT}\",\"action\":\"newaccount\",\"value\":500,\"feetype\":1}" -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} settxfee "{\"code\":\"${SYS_ACCOUNT}\",\"action\":\"updateauth\",\"value\":1000,\"feetype\":1}" -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} settxfee "{\"code\":\"${SYS_ACCOUNT}\",\"action\":\"linkauth\",\"value\":1000,\"feetype\":1}" -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI get txfee list -L "${SYS_ACCOUNT}" -U "${SYS_ACCOUNT}"
$INFRA_CLI push action ${SYS_ACCOUNT} unsettxfee "{\"code\":\"${SYS_ACCOUNT}\",\"action\":\"linkauth\"}" -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI get txfee list -L "${SYS_ACCOUNT}" -U "${SYS_ACCOUNT}"
$INFRA_CLI push action ${SYS_ACCOUNT} settxfee "{\"code\":\"${SYS_ACCOUNT}\",\"action\":\"linkauth\",\"value\":1500,\"feetype\":1}" -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

sleep 1


$INFRA_CLI get txfee item ${SYS_ACCOUNT} newaccount
$INFRA_CLI get txfee item "" transfer
$INFRA_CLI get txfee item "" ""
$INFRA_CLI get txfee list -L "" -l 1
$INFRA_CLI get txfee list -L ${SYS_ACCOUNT} -U ${SYS_ACCOUNT} -l 2
$INFRA_CLI get txfee list


{ print_section_title "Initial Block Producer Setup"; } 2>/dev/null

$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2ocZxRUNmcX94XZa3BkDQVadHhBpgveVu9uisn5rwnjA3HtcCJ
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_U9Ee4ZGBPVMfyRAhnWqvkGPJY24MK48j7GSA9eRckSroJ7f1i
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2RCY8az3ghcCjz9zCy3nSWCWhuskPm3hbCjUAoT8P1R2mCpp3L
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2sVfxUt4mEYXcmpeCHofTMh4eKZj39Si55M3E5qd1mf3VVk5fp
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_27iTXKeEN5J8eddX5dVFTGLR6CZt6sXHtFGhxhsTLJgsGYW5Zw
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_1BTEQ7FuXNpAwEwiVeBAM8ub9m2SHZii8HeH4U17pMirbCP4z
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2QCCaMVBmQw7zwv6QmeKVvQBdKovqsgCfG6qr9VRWRkCpNSQzi
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2atqfuPKaW8jvCVgNd2bjXQ2gsuJUrdjXFAe6KZCpTyeHc3Hqr
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_fkYpEQam1UKXJuPuYBKnbWk7nRzoGFC21x8WSuZFtRjgUKTvT
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_ci477gtWsaiitBZt5m9t2MwKdFzCR47gvThYjSFqqKh3u9WHu
sleep 1

$INFRA_CLI create account ${SYS_ACCOUNT} producer.a PUB_K1_4uE86f8v56gAGT92VjJqcVf2hdwMuwhQbVMk5Twta83StDyis1 -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI create account ${SYS_ACCOUNT} producer.b PUB_K1_7vx3brmnCQ6GvGyhHssNWbamN9VycZNHPuwq58NxQmixaXqoyB -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI create account ${SYS_ACCOUNT} producer.c PUB_K1_5pWYTTfJnogzVJmJ4E3x48S6wjY1VQ6VfwpnNq8f3iscRyMiGa -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI create account ${SYS_ACCOUNT} producer.d PUB_K1_5jHaKqt4jBXrnwCGGMbf1HmAQk4t1TUCUJHVNVkSrjB2Hr1Pux -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI create account ${SYS_ACCOUNT} producer.e PUB_K1_5oCuNPNUj2TiVbybefYzDq5kfpEaS6Y8iK2BL5jnwcKimp92zJ -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI create account ${SYS_ACCOUNT} producer.f PUB_K1_6f4NxFQoFTfQTvbg28g3uPAB2qF3g8ijbpXiVvp8AiSsQLGiXw -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI create account ${SYS_ACCOUNT} producer.g PUB_K1_5z17LRtLxDJRTDEVQRvUqZoq9ikzhyZzsAM6VZXN5atZsd4nqD -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI create account ${SYS_ACCOUNT} producer.h PUB_K1_8DppnPXPrajP93RuV1TUFQWKmjvebnWrwGqhAhucp8iifxkJvc -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI create account ${SYS_ACCOUNT} producer.i PUB_K1_7fMtu1cHqADH9NEvMXZWZaBg1rkM5k7s2dtP7NvPTTjFean4K3 -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI create account ${SYS_ACCOUNT} producer.j PUB_K1_8Xd6fUhpb7hLogbyo7z9sF6epfaeFx8vuAANPbcZtTDj6Yuzwk -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
sleep 1

$INFRA_CLI push action ${SYS_ACCOUNT} regproducer '{"producer":"producer.a","producer_key":"PUB_K1_4uE86f8v56gAGT92VjJqcVf2hdwMuwhQbVMk5Twta83StDyis1","url":"http://producera.io","location":1}' -p producer.a@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} regproducer '{"producer":"producer.b","producer_key":"PUB_K1_7vx3brmnCQ6GvGyhHssNWbamN9VycZNHPuwq58NxQmixaXqoyB","url":"http://producerb.io","location":1}' -p producer.b@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI push action ${SYS_ACCOUNT} authproducer '{"producer":"producer.a"}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} authproducer '{"producer":"producer.b"}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI push action ${SYS_ACCOUNT} regproducer '{"producer":"producer.c","producer_key":"PUB_K1_5pWYTTfJnogzVJmJ4E3x48S6wjY1VQ6VfwpnNq8f3iscRyMiGa","url":"http://producerc.io","location":1}' -p producer.c@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} regproducer '{"producer":"producer.d","producer_key":"PUB_K1_5jHaKqt4jBXrnwCGGMbf1HmAQk4t1TUCUJHVNVkSrjB2Hr1Pux","url":"http://producerd.io","location":1}' -p producer.d@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} regproducer '{"producer":"producer.e","producer_key":"PUB_K1_5oCuNPNUj2TiVbybefYzDq5kfpEaS6Y8iK2BL5jnwcKimp92zJ","url":"http://producere.io","location":1}' -p producer.e@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI push action ${SYS_ACCOUNT} authproducer '{"producer":"producer.c"}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} authproducer '{"producer":"producer.d"}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} authproducer '{"producer":"producer.e"}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI push action ${SYS_ACCOUNT} regproducer '{"producer":"producer.f","producer_key":"PUB_K1_6f4NxFQoFTfQTvbg28g3uPAB2qF3g8ijbpXiVvp8AiSsQLGiXw","url":"http://producerf.io","location":1}' -p producer.f@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} regproducer '{"producer":"producer.g","producer_key":"PUB_K1_5z17LRtLxDJRTDEVQRvUqZoq9ikzhyZzsAM6VZXN5atZsd4nqD","url":"http://producerg.io","location":1}' -p producer.g@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} regproducer '{"producer":"producer.h","producer_key":"PUB_K1_8DppnPXPrajP93RuV1TUFQWKmjvebnWrwGqhAhucp8iifxkJvc","url":"http://producerh.io","location":1}' -p producer.h@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} regproducer '{"producer":"producer.i","producer_key":"PUB_K1_7fMtu1cHqADH9NEvMXZWZaBg1rkM5k7s2dtP7NvPTTjFean4K3","url":"http://produceri.io","location":1}' -p producer.i@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} regproducer '{"producer":"producer.j","producer_key":"PUB_K1_8Xd6fUhpb7hLogbyo7z9sF6epfaeFx8vuAANPbcZtTDj6Yuzwk","url":"http://producerj.io","location":1}' -p producer.j@active --txfee-payer ${SYS_ACCOUNT}

$INFRA_CLI push action ${SYS_ACCOUNT} authproducer '{"producer":"producer.f"}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} authproducer '{"producer":"producer.g"}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} authproducer '{"producer":"producer.h"}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} authproducer '{"producer":"producer.i"}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} authproducer '{"producer":"producer.j"}' -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

## Transaction votes for initial block producer election
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"10.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.a
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"20.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.b
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"30.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.c
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"40.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.d
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"50.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.e
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"60.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.f
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"70.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.g
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"80.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.h
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"90.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.i
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"100.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.j
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"110.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.a
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"120.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.b
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"130.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.c
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"140.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.d
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.b","qty":"150.0000 DUSD","tag":"issue systoken.a to idauth.b"}' -p systoken.a@active --txfee-payer systoken.a -v producer.e

$INFRA_CLI get txvote top
$INFRA_CLI get txvote account producer.a

sleep 125
tail -n 150 $INFRA_NODE_LOG_FILE

{ print_section_title "Resign \"${SYS_ACCOUNT}\" delegating authority to \"eosio.prods\""; } 2>/dev/null

$INFRA_CLI get account ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth "{\"account\":\"${SYS_ACCOUNT}\",\"permission\":\"owner\",\"parent\":\"\",\"auth\":{\"threshold\":1,\"keys\":[],\"waits\":[],\"accounts\":[{\"weight\":1,\"permission\":{\"actor\":\"eosio.prods\",\"permission\":\"active\"}}]}}" -p ${SYS_ACCOUNT}@owner --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth "{\"account\":\"${SYS_ACCOUNT}\",\"permission\":\"active\",\"parent\":\"owner\",\"auth\":{\"threshold\":1,\"keys\":[],\"waits\":[],\"accounts\":[{\"weight\":1,\"permission\":{\"actor\":\"eosio.prods\",\"permission\":\"active\"}}]}}" -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI get account ${SYS_ACCOUNT}
$INFRA_CLI get account eosio.prods

sleep 1

#{ print_section_title "Resign \"sys.msig\" delegating authority to \"${SYS_ACCOUNT}\""; } 2>/dev/null
#
#$INFRA_CLI get account sys.msig
#$INFRA_CLI push action ${SYS_ACCOUNT} updateauth '{"account":"sys.msig","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"${SYS_ACCOUNT}","permission":"active"}}]}}' -p sys.msig@owner --txfee-payer ${SYS_ACCOUNT}
#$INFRA_CLI push action ${SYS_ACCOUNT} updateauth '{"account":"sys.msig","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"${SYS_ACCOUNT}","permission":"active"}}]}}' -p sys.msig@active --txfee-payer ${SYS_ACCOUNT}
#$INFRA_CLI get account sys.msig

sleep 1

{ print_section_title "Resign System Contract Accounts delegating authority to \"${SYS_ACCOUNT}\""; } 2>/dev/null

$INFRA_CLI get account sys.tokenabi
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth "{\"account\":\"sys.tokenabi\",\"permission\":\"owner\",\"parent\":\"\",\"auth\":{\"threshold\":1,\"keys\":[],\"waits\":[],\"accounts\":[{\"weight\":1,\"permission\":{\"actor\":\"${SYS_ACCOUNT}\",\"permission\":\"active\"}}]}}" -p sys.tokenabi@owner --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth "{\"account\":\"sys.tokenabi\",\"permission\":\"active\",\"parent\":\"owner\",\"auth\":{\"threshold\":1,\"keys\":[],\"waits\":[],\"accounts\":[{\"weight\":1,\"permission\":{\"actor\":\"${SYS_ACCOUNT}\",\"permission\":\"active\"}}]}}" -p sys.tokenabi@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI get account sys.tokenabi

$INFRA_CLI get account sys.txfee
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth "{\"account\":\"sys.txfee\",\"permission\":\"owner\",\"parent\":\"\",\"auth\":{\"threshold\":1,\"keys\":[],\"waits\":[],\"accounts\":[{\"weight\":1,\"permission\":{\"actor\":\"${SYS_ACCOUNT}\",\"permission\":\"active\"}}]}}" -p sys.txfee@owner --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth "{\"account\":\"sys.txfee\",\"permission\":\"active\",\"parent\":\"owner\",\"auth\":{\"threshold\":1,\"keys\":[],\"waits\":[],\"accounts\":[{\"weight\":1,\"permission\":{\"actor\":\"${SYS_ACCOUNT}\",\"permission\":\"active\"}}]}}" -p sys.txfee@active --txfee-payer ${SYS_ACCOUNT}
$INFRA_CLI get account sys.txfee


sleep 1

{ print_section_title "Create New User Accounts"; } 2>/dev/null

$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2gSaRyXwhPaRhpvZwF8ZCLvskJw8BpDnJzYa2b8siJC6sumY8A
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2YKh5fwAKdwG1vFvdSyUT636qesed171fL3nh4i5xLiHTp2NMz
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2VXd5W4vPMUZ998zButT7f2U2NbS93TqkzJ6tgQYHE6yHUApQb
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_W5DUCEM6xqmJqXvz3ZfH7VZ9gTPqCXVabDd2YzUowVBG7VKo
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_fGvQ9cRyumC3xzFG3cvPfNaNibkzRLFXBHF9P7FJixhvqkxHV

$INFRA_CLI create account idauth.a useraccount1 PUB_K1_6rGczYQ1yjJVZcm3TbKbkLwFX98LoigRHREvU1yWS4MPxRPCSx -p idauth.a@active --txfee-payer idauth.a
$INFRA_CLI create account idauth.a useraccount2 PUB_K1_8ZzFcSW18yA1JgwrEzxSN75GoE263dMWW8X4tHrvvC8nuN5z2j -p idauth.a@active --txfee-payer idauth.a
$INFRA_CLI create account idauth.a useraccount3 PUB_K1_7auEiXsYXmQe5WdyjGJmQ4e4eRpi3PFWvNzRQDajhCZ65jTY7R -p idauth.a@active --txfee-payer idauth.a

$INFRA_CLI create account ${SYS_ACCOUNT} com PUB_K1_6gr3cnC46CuMW4dEgp42ct64aWqFdcYQzRFdwCKU4X11Qa5ZKA -p ${SYS_ACCOUNT}@active --txfee-payer ${SYS_ACCOUNT}

#$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"com\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"1f32i7t23\"}" -p idauth.a@active --txfee-payer idauth.a
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"com","qty":"3000.0000 DUSD","tag":"issue systoken.a to com"}' -p systoken.a@active --txfee-payer systoken.a
$INFRA_CLI create account com acquire.com PUB_K1_5RyCzTHK13aRPA6nYmPzKgjVhdXUXtbEuj3JfQGhoKUaT56dnH -p com@active --txfee-payer com

$INFRA_CLI get systoken balance com
$INFRA_CLI get systoken balance acquire.com
$INFRA_CLI get systoken balance sys.txfee

#$INFRA_CLI get table -L com -l 1 sys.identity sys.identity identity

sleep 1

{ print_section_title "Managing Account Identity Info (KYC, Account Type/State/Data)"; } 2>/dev/null

sleep 1

{ print_section_title "System Token Issue / Transfer"; } 2>/dev/null

$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"useraccount2","qty":"1000.0000 DUSD","tag":"issue systoken.a"}' -p systoken.a@active --txfee-payer systoken.a
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"useraccount3","qty":"1000.0000 DUSD","tag":"issue systoken.a"}' -p systoken.a@active --txfee-payer systoken.a

$INFRA_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccount2","to":"useraccount3","qty":"100.0000 DUSD","tag":"transfer memo"}' -p useraccount2@active --txfee-payer useraccount2
$INFRA_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccount2","to":"useraccount3","qty":"100.0000 DUSD","tag":"transfer memo 2"}' -p useraccount2@active --txfee-payer useraccount3

$INFRA_CLI get systoken balance useraccount1
$INFRA_CLI get systoken balance useraccount2
$INFRA_CLI get systoken balance useraccount3
$INFRA_CLI get systoken balance sys.txfee

sleep 1


{ print_section_title "Transaction Votes"; } 2>/dev/null

$INFRA_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccount3","to":"useraccount2","qty":"50.0000 DUSD","tag":"tag1"}' -p useraccount3@active --txfee-payer useraccount2 -v producer.f
$INFRA_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccount3","to":"useraccount2","qty":"50.0000 DUSD","tag":"tag2"}' -p useraccount3@active --txfee-payer useraccount2 -v producer.g
$INFRA_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccount3","to":"useraccount2","qty":"50.0000 DUSD","tag":"tag3"}' -p useraccount3@active --txfee-payer useraccount2 -v producer.g
sleep 2

#$INFRA_CLI get table ${SYS_ACCOUNT} ${SYS_ACCOUNT} producers
$INFRA_CLI get txvote top
sleep 1


{ print_section_title "Account Recovery"; } 2>/dev/null

$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_25dYUkNiDbDTS6QCbpVkVdD2hahWYPeKdcsaU7keoUmMn5Q8Sr
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2nCiLfxueijtLm6VSM84hVeekAYjvcXiZFUq837kZ8Q7MAaLBU
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_MHGoN1Dyp2Z1ESE77ngzenZn2jtzvtfPCEMgbQ3EaPPYxdrPc
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_MtWqXKuwY4hb8qr5c3fAMFLxgD7H6yVqepkN8ZWS5nSeudNXY
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_pN929oHCx3nef3kX2c4J9kJjjRQ4H2YJo4aMWEhH7zoTy2FPw

sleep 1
$INFRA_CLI push action ${SYS_ACCOUNT} newaccount '{"creator":"idauth.a","name":"useraccountz","owner":{"threshold":1,"keys":[{"key":"PUB_K1_7iQ8oVcnWs5ARztz8pDN2MSyFPFX3EXAKtK7zHuLYrbdCzToNm","weight":1}],"accounts":[{"permission":{"actor":"idauth.a","permission":"active"},"weight":1}],"waits":[]},"active":{"threshold":1,"keys":[{"key":"PUB_K1_7CLNHjPKHsRpgZmBctdkVKuZeqX5jgrro9uNTRfDRQMPwYEYPZ","weight":1}],"accounts":[],"waits":[]}}' -p idauth.a@active --txfee-payer idauth.a -v producer.c
$INFRA_CLI get account useraccountz
sleep 1
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth '{"account":"useraccountz","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key":"PUB_K1_6d1UeaP2MSHPqp1Eyku7cSmmBpkbdobbi17cNEpYPCgstmfExe","weight":1}],"waits":[],"accounts":[]}}' -p useraccountz@owner --txfee-payer idauth.a
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth '{"account":"useraccountz","permission":"owner","parent":"","auth":{"threshold":2,"keys":[{"key":"PUB_K1_65naa2mTDkHZQFXBn6AcbddHuqytPdKwQnRJv2H7BDaxdi1obA","weight":2}],"accounts":[{"permission":{"actor":"idauth.a","permission":"active"},"weight":1},{"permission":{"actor":"systoken.a","permission":"active"},"weight":1}],"waits":[]}}' -p useraccountz@owner --txfee-payer idauth.a
$INFRA_CLI get account useraccountz
sleep 1
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth '{"account":"useraccountz","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key":"PUB_K1_5yK2PNDHhwjrfmx5y4SyQocH1mEantPKsVNWQ69qC3Meh39RRH","weight":1}],"waits":[],"accounts":[]}}' -p useraccountz@owner --txfee-payer idauth.a
$INFRA_CLI get account useraccountz
sleep 1
$INFRA_CLI push action ${SYS_ACCOUNT} newaccount '{"creator":"idauth.a","name":"useraccountx","owner":{"threshold":2,"keys":[{"key":"PUB_K1_65naa2mTDkHZQFXBn6AcbddHuqytPdKwQnRJv2H7BDaxdi1obA","weight":2}],"accounts":[{"permission":{"actor":"idauth.a","permission":"active"},"weight":1},{"permission":{"actor":"systoken.a","permission":"active"},"weight":1}],"waits":[]},"active":{"threshold":1,"keys":[{"key":"PUB_K1_5yK2PNDHhwjrfmx5y4SyQocH1mEantPKsVNWQ69qC3Meh39RRH","weight":1}],"accounts":[],"waits":[]}}' -p idauth.a@active --txfee-payer idauth.a -v producer.c
$INFRA_CLI get account useraccountx
sleep 1
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth '{"account":"useraccountx","permission":"owner","parent":"","auth":{"threshold":2,"keys":[{"key":"PUB_K1_6d1UeaP2MSHPqp1Eyku7cSmmBpkbdobbi17cNEpYPCgstmfExe","weight":1}],"accounts":[{"permission":{"actor":"idauth.a","permission":"active"},"weight":1},{"permission":{"actor":"systoken.a","permission":"active"},"weight":1}],"waits":[]}}' -p useraccountx@owner --txfee-payer idauth.a
$INFRA_CLI push action ${SYS_ACCOUNT} updateauth '{"account":"useraccountx","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[{"key":"PUB_K1_6d1UeaP2MSHPqp1Eyku7cSmmBpkbdobbi17cNEpYPCgstmfExe","weight":1}],"accounts":[],"waits":[]}}' -p useraccountx@owner --txfee-payer idauth.a
$INFRA_CLI get account useraccountx
sleep 1
$INFRA_CLI push action ${SYS_ACCOUNT} newaccount '{"creator":"idauth.a","name":"useraccounty","owner":{"threshold":3,"keys":[{"key":"PUB_K1_65naa2mTDkHZQFXBn6AcbddHuqytPdKwQnRJv2H7BDaxdi1obA","weight":2}],"accounts":[{"permission":{"actor":"idauth.a","permission":"active"},"weight":2},{"permission":{"actor":"systoken.a","permission":"active"},"weight":1}],"waits":[{"wait_sec":604800,"weight":1}]},"active":{"threshold":1,"keys":[{"key":"PUB_K1_5yK2PNDHhwjrfmx5y4SyQocH1mEantPKsVNWQ69qC3Meh39RRH","weight":1}],"accounts":[],"waits":[]}}' -p idauth.a@active --txfee-payer idauth.a -v producer.c
$INFRA_CLI get account useraccounty
sleep 1


{ print_section_title "InfraBlockchain Standard Token Test"; } 2>/dev/null

$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"idauth.a","qty":"10000.0000 DUSD","tag":""}' -p systoken.a@active --txfee-payer systoken.a

$INFRA_CLI push action systoken.a transfer '{"t":"systoken.a","from":"idauth.a","to":"idauth.b","qty":"50.0000 DUSD","tag":"transfer tag"}' -p idauth.a@active --txfee-payer idauth.a
$INFRA_CLI push action systoken.a transfer '{"t":"systoken.a","from":"idauth.a","to":"idauth.b","qty":"30.0000 DUSD","tag":"transfer tag 2"}' -p idauth.a@active --txfee-payer idauth.b

$INFRA_CLI get token balance systoken.a idauth.a
$INFRA_CLI get token balance systoken.a idauth.b

$INFRA_CLI get token balance systoken.a systoken.a
$INFRA_CLI push action systoken.a redeem '{"qty":"500.0000 DUSD","tag":"redeem tag"}' -p systoken.a@active --txfee-payer systoken.a

$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2K75BtRGgahpZqT4QfeVNyrNN7gBaSHTpVg5XMWLXkKjNETCWt
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_CKtaYYgdJsHnN5kmJ5WuxcqFQVkuG5U1Wspp79BXMYE7m3Zen
$INFRA_CLI wallet import -n ${INFRA_KEYCHAIN_WALLET_NAME} --private-key PVT_K1_2GbCUqeWSrbSfdL3H3ngermFP7m1paiPDKCn8kT3p4HNkQDqnk

$INFRA_CLI create account idauth.a useraccounta PUB_K1_59GwcpZm6FqQDDfuRsCTsVWQ6MgWWUE4wCFVACDqx5dwMBwBwR -p idauth.a@active --txfee-payer idauth.a
$INFRA_CLI create account idauth.a useraccountb PUB_K1_8iVnvTrvG4vQMcJ7R45BKhzNeijn5LDE8d4sq32EGknDc3Zro1 -p idauth.a@active --txfee-payer idauth.a
$INFRA_CLI create account idauth.a useraccountc PUB_K1_62eHR1NUJCULSTfcSKKGxi1Cb3ctPxgnK8MyzXhtZUL2jhkmpq -p idauth.a@active --txfee-payer idauth.a

$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"useraccounta","qty":"1000.0000 DUSD","tag":""}' -p systoken.a@active --txfee-payer systoken.a
$INFRA_CLI push action systoken.b issue '{"t":"systoken.b","to":"useraccounta","qty":"1000.0000 DUSDB","tag":""}' -p systoken.b@active --txfee-payer systoken.b

$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"useraccountb","qty":"2000.0000 DUSD","tag":""}' -p systoken.a@active --txfee-payer systoken.a
$INFRA_CLI push action systoken.a issue '{"t":"systoken.a","to":"useraccountc","qty":"3000.0000 DUSD","tag":""}' -p systoken.a@active --txfee-payer systoken.a

$INFRA_CLI get token balance systoken.a useraccounta
$INFRA_CLI get token balance systoken.b useraccounta
$INFRA_CLI get systoken balance useraccounta

$INFRA_CLI get token balance systoken.a useraccountb
$INFRA_CLI get token balance systoken.a useraccountc

$INFRA_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccounta","to":"useraccountb","qty":"999.5000 DUSD","tag":"transfer tag"}' -p useraccounta@active --txfee-payer useraccounta -v producer.a

# first authorizer as tx fee payer
$INFRA_CLI push action systoken.a transfer '{"t":"systoken.a","from":"useraccounta","to":"useraccountb","qty":"0.1000 DUSD","tag":"transfer tag"}' -p useraccounta@active -v producer.a

$INFRA_CLI get systoken balance useraccounta
$INFRA_CLI get systoken balance useraccountb
$INFRA_CLI get systoken balance useraccountc

sleep 2
