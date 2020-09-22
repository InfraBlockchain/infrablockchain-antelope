#!/usr/bin/env bash
# chmod +x ./yosemite_card_ytoken_sample_transactions.sh

#### Yosemite Card Yosemite Token contract sample transactions

$INFRA_CLI wallet import --private-key PVT_K1_r2nv6dJvB3pQyxSh5JkXyLaKrVkh9NDYAA1H9HAQmj6eCWGM6
$INFRA_CLI create account infrasys ycard.usd.yt PUB_K1_7nn2nbvyuCspiCt5nQp5Kx5GJKKoSLmsAPTdY9Yua34PcYVCYT -p infrasys@active --txfee-payer infrasys

$INFRA_CLI wallet import --private-key PVT_K1_d1mjQrD3BtnMbmgeUQX5oc5XwJRVUgJNY65MpZJTS6D6D1ucp
$INFRA_CLI create account infrasys ycard.users PUB_K1_5zWk8fvNugYJ9noncZkKcAnmfeojbBqc9F8X1j9FYdMPstcqLk -p infrasys@active --txfee-payer infrasys

sleep 2
$INFRA_CLI set contract ycard.usd.yt $INFRABLOCKCHAIN_CONTRACTS_DIR/yosemite.card.ytoken/ -p ycard.usd.yt@active --txfee-payer infrasys
sleep 2

$INFRA_CLI push action ycard.usd.yt settokenmeta '{"sym":"4,YTOKEN","url":"https://yosemitecardx.com","desc":"Yosemite Card Yosemite Token USD"}' -p ycard.usd.yt@active --txfee-payer infrasys

$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"ytokenissue","value":300,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"ytokenburn","value":500,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"ytokenredeem","value":500,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"ytokenpay","value":50,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"usdytissue","value":200,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"usdredeemto","value":200,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"yusdtransfer","value":100,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"yusdredeemrq","value":100,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"cnclyusdrdrq","value":100,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"creditlimit","value":100,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"creditissue","value":200,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"credittxfer","value":50,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"creditsettle","value":200,"feetype":1}' -p infrasys@active --txfee-payer infrasys
$INFRA_CLI push action infrasys settxfee '{"code":"ycard.usd.yt","action":"creditburn","value":50,"feetype":1}' -p infrasys@active --txfee-payer infrasys

$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"ycard.usd.yt\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a
$INFRA_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"ycard.usd.yt","qty":"10000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.a

$INFRA_CLI get token balance ysmt.dusd.a ycard.usd.yt


$INFRA_CLI wallet import --private-key PVT_K1_2DgoXF9WARUKb6caFLLTALYvbdHjHTKnS4FFfxnLv49sDsQrFy
$INFRA_CLI create account infrasys ycard.id.us PUB_K1_88E4kRrKvEaz9tfkAqeEmiyZLJPf5rb7psdTASHCNpWGHd9d9g -p infrasys@active --txfee-payer infrasys

$INFRA_CLI push action infrasys regidauth '{"identity_authority":"ycard.id.us","url":"https://yosemitecardx.com","location":1}' -p ycard.id.us@active --txfee-payer infrasys
$INFRA_CLI push action infrasys authidauth '{"identity_authority":"ycard.id.us"}' -p infrasys@active --txfee-payer infrasys

$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"ycard.id.us\", \"identity_authority\":\"ycard.id.us\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p ycard.id.us@active --txfee-payer infrasys
$INFRA_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"ycard.id.us","qty":"10000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.a

# deposit USD-backed Yosemite Tokens for Yosemite Card service operation
$INFRA_CLI push action ycard.usd.yt usdytissue '{"qty":"20000.0000 USD","tag":"issue USD-backed YTOKEN by Yosemite Card"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.d

$INFRA_CLI get token balance ycard.usd.yt ycard.usd.yt
curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.usd.yt"}'; echo;

sleep 2

#################################################
### Yosemite Card User Account Setup
$INFRA_CLI wallet import --private-key PVT_K1_TiDZHzPzZwimu36VGt3YjmWqcSkRa9BjkVtBa7Zr6mjKKgMsE
#$INFRA_CLI create account ycard.usd.yt ycarduseraaa PUB_K1_7UPqYWRodQ1dhgCsgF4VnEi1mgcV4a6K7u95BrtTDoNCVXtFYT -p ycard.usd.yt@active --txfee-payer ycard.usd.yt
$INFRA_CLI push action infrasys newaccount '{"creator":"ycard.usd.yt","name":"ycarduseraaa","owner":{"threshold":1,"keys":[{"key":"PUB_K1_7UPqYWRodQ1dhgCsgF4VnEi1mgcV4a6K7u95BrtTDoNCVXtFYT","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]},"active":{"threshold":1,"keys":[{"key":"PUB_K1_7UPqYWRodQ1dhgCsgF4VnEi1mgcV4a6K7u95BrtTDoNCVXtFYT","weight":1}],"accounts":[],"waits":[]}}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c
#$INFRA_CLI push action infrasys updateauth '{"account":"ycarduseraaa","permission":"owner","parent":"","auth":{"threshold":1,"keys":[{"key":"PUB_K1_7UPqYWRodQ1dhgCsgF4VnEi1mgcV4a6K7u95BrtTDoNCVXtFYT","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]}}' -p ycarduseraaa@owner --txfee-payer ycard.usd.yt

$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"ycarduseraaa\", \"identity_authority\":\"ycard.id.us\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p ycard.id.us@active --txfee-payer ycard.id.us -v producer.a

# ycarduseraaa issues credit tokens and deposits to ycard service
$INFRA_CLI push action infrasys updateauth '{"account":"ycarduseraaa","permission":"creditissue","parent":"active","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"ycard.usd.yt","permission":"active"}}]}}' -p ycarduseraaa@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycarduseraaa","code":"ycard.usd.yt","type":"creditissue","requirement":"creditissue"}' -p ycarduseraaa@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys updateauth '{"account":"ycarduseraaa","permission":"codecrdtxfer","parent":"active","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"ycard.usd.yt","permission":"sys.code"}}]}}' -p ycarduseraaa@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycarduseraaa","code":"ycard.usd.yt","type":"credittxfer","requirement":"codecrdtxfer"}' -p ycarduseraaa@active --txfee-payer ycard.usd.yt -v producer.c

# offering credit limit to user account ycarduseraaa (issuing credit card)
$INFRA_CLI push action ycard.usd.yt creditlimit '{"account":"ycarduseraaa","credit_limit":"1000.0000 CREDIT","tag":"credit offering"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.b

$INFRA_CLI get table ycard.usd.yt ycard.usd.yt creditoffer -L ycarduseraaa -l 1
curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"creditoffer","lower_bound":"ycarduseraaa","limit":1}'; echo;

$INFRA_CLI push action ycard.usd.yt creditissue '{"issuer":"ycarduseraaa","to":"ycard.usd.yt","qty":"1000.0000 CREDIT","tag":"credit token issued by user, and deposited to yosemite card service"}' -p ycarduseraaa@creditissue --txfee-payer ycard.usd.yt -v producer.c

$INFRA_CLI get table ycard.usd.yt ycard.usd.yt creditoffer -L ycarduseraaa -l 1
curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"creditoffer","lower_bound":"ycarduseraaa","limit":1}'; echo;
$INFRA_CLI get table ycard.usd.yt ycard.usd.yt credittoken -L ycarduseraaa -l 1
curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"credittoken","lower_bound":"ycarduseraaa","limit":1}'; echo;
$INFRA_CLI get table ycard.usd.yt ycard.usd.yt credittoken -L ycard.usd.yt -l 1
curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"credittoken","lower_bound":"ycard.usd.yt","limit":1}'; echo;
$INFRA_CLI get table ycard.usd.yt ycard.usd.yt creditstat
curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"creditstat"}'; echo;

sleep 2

#################################################
##### Yosemite Card Merchant Account Setup
$INFRA_CLI wallet import --private-key PVT_K1_SMmDwJed8ZR4hVsMD7bJLYirKpesXtr9tDwnmF5hW15R8myWA
#$INFRA_CLI create account ycard.usd.yt ycardshopaaa PUB_K1_7c2UjL57nCkxR8PWXzt3srpWEM1Cvv6JcAgwzJ6JtqMvQd52wZ -p ycard.usd.yt@active --txfee-payer ycard.usd.yt
$INFRA_CLI push action infrasys newaccount '{"creator":"ycard.usd.yt","name":"ycardshopaaa","owner":{"threshold":1,"keys":[{"key":"PUB_K1_7c2UjL57nCkxR8PWXzt3srpWEM1Cvv6JcAgwzJ6JtqMvQd52wZ","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]},"active":{"threshold":1,"keys":[{"key":"PUB_K1_7c2UjL57nCkxR8PWXzt3srpWEM1Cvv6JcAgwzJ6JtqMvQd52wZ","weight":1}],"accounts":[],"waits":[]}}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c
#$INFRA_CLI push action infrasys updateauth '{"account":"ycardshopaaa","permission":"owner","parent":"","auth":{"threshold":1,"keys":[{"key":"PUB_K1_7c2UjL57nCkxR8PWXzt3srpWEM1Cvv6JcAgwzJ6JtqMvQd52wZ","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]}}' -p ycardshopaaa@owner --txfee-payer ycard.usd.yt

$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"ycardshopaaa\", \"identity_authority\":\"ycard.id.us\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1101' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p ycard.id.us@active --txfee-payer ycard.id.us -v producer.a

$INFRA_CLI push action infrasys updateauth '{"account":"ycardshopaaa","permission":"ytokenaction","parent":"active","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"ycard.usd.yt","permission":"active"}}]}}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycardshopaaa","code":"ycard.usd.yt","type":"ytokenissue","requirement":"ytokenaction"}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycardshopaaa","code":"ycard.usd.yt","type":"ytokenburn","requirement":"ytokenaction"}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycardshopaaa","code":"ycard.usd.yt","type":"ytokenredeem","requirement":"ytokenaction"}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycardshopaaa","code":"ycard.usd.yt","type":"ytokenpay","requirement":"ytokenaction"}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycardshopaaa","code":"ycard.usd.yt","type":"yusdredeemrq","requirement":"ytokenaction"}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c

#################################################
#### Issue Yosemite Token backed by Merchant-issued gift card token

# merchant issues gift token backed yosemite token, yosemite card prepays USD to the merchant
$INFRA_CLI push action ycard.usd.yt ytokenissue '{"merchant":"ycardshopaaa","qty":"700.0000 YTOKEN","paid":"500.0000 USD","tag":"issue merchant gift token backed yosemite token"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

$INFRA_CLI get table ycard.usd.yt ycard.usd.yt ytokenissue -L ycardshopaaa -l 1
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenissue","lower_bound":"ycardshopaaa","limit":1}'; echo;

$INFRA_CLI get token balance ycard.usd.yt ycard.usd.yt
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.usd.yt"}'; echo;

sleep 2

#################################################
##### Yosemite Card Payment Transactions

### yosemite card payment (anonymous user)
$INFRA_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopaaa","price":"24.0000 YTOKEN","credit":"24.0000 CREDIT","ytoken":"0.0000 YTOKEN","reward":"6.0000 YTOKEN","tag":"yosemite card anonymous payment"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

$INFRA_CLI get token balance ycard.usd.yt ycard.usd.yt
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.usd.yt"}'; echo;
$INFRA_CLI get token balance ycard.usd.yt ycard.users
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.users"}'; echo;

$INFRA_CLI get table ycard.usd.yt ycard.usd.yt ytokenissue -L ycardshopaaa -l 1
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenissue","lower_bound":"ycardshopaaa","limit":1}'; echo;
$INFRA_CLI get table ycard.usd.yt ycard.usd.yt ytokenissue -L usdbacked -l 1
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenissue","lower_bound":"usdbacked","limit":1}'; echo;

### yosemite card payment using CREDIT + YTOKEN (anonymous user)
$INFRA_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopaaa","price":"32.0000 YTOKEN","credit":"28.0000 CREDIT","ytoken":"4.0000 YTOKEN","reward":"7.0000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

$INFRA_CLI get table ycard.usd.yt ycard.usd.yt ytokenissue -L ycardshopaaa -l 1
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenissue","lower_bound":"ycardshopaaa","limit":1}'; echo;
$INFRA_CLI get table ycard.usd.yt ycard.usd.yt ytokenissue -L usdbacked -l 1
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenissue","lower_bound":"usdbacked","limit":1}'; echo;

$INFRA_CLI get token balance ycard.usd.yt ycard.usd.yt
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.usd.yt"}'; echo;
$INFRA_CLI get token balance ycard.usd.yt ycard.users
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.users"}'; echo;

$INFRA_CLI get token info ycard.usd.yt
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_token_info -d '{"token":"ycard.usd.yt"}'; echo;
$INFRA_CLI get table ycard.usd.yt ycard.usd.yt ytokenstat
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenstat"}'; echo;

$INFRA_CLI get table ycard.usd.yt ycard.usd.yt creditstat
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"creditstat"}'; echo;

$INFRA_CLI get table ycard.usd.yt ycard.usd.yt yusdstat
#curl ${INFRA_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"yusdstat"}'; echo;

### yosemite card payment using CREDIT + YTOKEN (anonymous user)
$INFRA_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopaaa","price":"350.0000 YTOKEN","credit":"342.0000 CREDIT","ytoken":"8.0000 YTOKEN","reward":"81.0000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

sleep 2

#####################################################
##### Monthly user credit balance settlement

### Yosemite Card user(s) sends money off-chain to Yosemite Card as credit balance settlement
$INFRA_CLI push action ycard.usd.yt usdytissue '{"qty":"394.0000 USD","tag":"YCardUserMonthlyCreditBalancePayment"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.d

$INFRA_CLI push action ycard.usd.yt creditburn '{"qty":"606.0000 CREDIT","tag":"burn unused credit"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.g

$INFRA_CLI push action ycard.usd.yt creditsettle '{"account":"ycarduseraaa","qty":"1000.0000 CREDIT","tag":"reset user credit offering"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.e

$INFRA_CLI push action ycard.usd.yt creditissue '{"issuer":"ycarduseraaa","to":"ycard.usd.yt","qty":"1000.0000 CREDIT","tag":"monthly issue credit token by user, and deposit to ycard service"}' -p ycarduseraaa@creditissue --txfee-payer ycard.usd.yt -v producer.e


#####################################################
##### Burn existing merchant-issued Yosemite Token (store-credit) and Re-issue merchant-issued Yosemite Tokens

$INFRA_CLI push action ycard.usd.yt ytokenburn '{"merchant":"ycardshopaaa","qty":"294.0000 YTOKEN","tag":""}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.d
### merchant refunded USD to Yosemite Card Services
$INFRA_CLI push action ycard.usd.yt usdytissue '{"qty":"210.0000 USD","tag":"merchant refunded money for burning merchant-backed Yosemite Tokens"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.d

### re-issue merchant backed Yosemite Tokens, YUSD token is paid to merchant account for future redeeming to USD fiat money
$INFRA_CLI push action ycard.usd.yt ytokenissue '{"merchant":"ycardshopaaa","qty":"600.0000 YTOKEN","paid":"500.0000 YUSD","tag":"issue merchant gift token backed yosemite token"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c


#####################################################
##### Redeem YUSD to real fiat money

### YUSD redeem request
$INFRA_CLI push action ycard.usd.yt yusdredeemrq '{"account":"ycardshopaaa","qty":"300.0000 YUSD","tag":""}' -p ycardshopaaa@ytokenaction --txfee-payer ycard.usd.yt -v producer.c

### Cancel YUSD redeem request
$INFRA_CLI push action ycard.usd.yt cnclyusdrdrq '{"account":"ycardshopaaa","qty":"300.0000 YUSD","tag":""}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### YUSD redeem request
$INFRA_CLI push action ycard.usd.yt yusdredeemrq '{"account":"ycardshopaaa","qty":"500.0000 YUSD","tag":""}' -p ycardshopaaa@ytokenaction --txfee-payer ycard.usd.yt -v producer.c

### Yosemite Card completed YUSD redeem to merchant (transferred USD fund to merchant's bank account)
$INFRA_CLI push action ycard.usd.yt usdredeemto '{"to":"ycardshopaaa","qty":"500.0000 YUSD","tag":""}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c


#####################################################
##### Yosemite Card Payment to a merchant who does not issue merchant-backed Yosemite Token

$INFRA_CLI wallet import --private-key PVT_K1_KDQQ5JmS9sCZRZFqJwttJQqjpGrv5KqiSZdAMump34sQJ66MB
#$INFRA_CLI create account ycard.usd.yt ycarduserbbb PUB_K1_5FZrQjaDtifFxa45sKoPCJSsmp4kLuQKZU721HnjA17pchKVmX -p ycard.usd.yt@active --txfee-payer ycard.usd.yt
$INFRA_CLI push action infrasys newaccount '{"creator":"ycard.usd.yt","name":"ycardshopbbb","owner":{"threshold":1,"keys":[{"key":"PUB_K1_5FZrQjaDtifFxa45sKoPCJSsmp4kLuQKZU721HnjA17pchKVmX","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]},"active":{"threshold":1,"keys":[{"key":"PUB_K1_5FZrQjaDtifFxa45sKoPCJSsmp4kLuQKZU721HnjA17pchKVmX","weight":1}],"accounts":[],"waits":[]}}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c
#$INFRA_CLI push action infrasys updateauth '{"account":"ycardshopbbb","permission":"owner","parent":"","auth":{"threshold":1,"keys":[{"key":"PUB_K1_5FZrQjaDtifFxa45sKoPCJSsmp4kLuQKZU721HnjA17pchKVmX","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]}}' -p ycardshopbbb@owner --txfee-payer ycard.usd.yt

$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"ycardshopbbb\", \"identity_authority\":\"ycard.id.us\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1101' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p ycard.id.us@active --txfee-payer ycard.id.us -v producer.a

$INFRA_CLI push action infrasys updateauth '{"account":"ycardshopbbb","permission":"ytokenaction","parent":"active","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"ycard.usd.yt","permission":"active"}}]}}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycardshopbbb","code":"ycard.usd.yt","type":"ytokenissue","requirement":"ytokenaction"}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycardshopbbb","code":"ycard.usd.yt","type":"ytokenburn","requirement":"ytokenaction"}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycardshopbbb","code":"ycard.usd.yt","type":"ytokenredeem","requirement":"ytokenaction"}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycardshopbbb","code":"ycard.usd.yt","type":"ytokenpay","requirement":"ytokenaction"}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c
$INFRA_CLI push action infrasys linkauth '{"account":"ycardshopbbb","code":"ycard.usd.yt","type":"yusdredeemrq","requirement":"ytokenaction"}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c

### yosemite card payment using CREDIT + YTOKEN (anonymous user)
$INFRA_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopbbb","price":"120.0000 YTOKEN","credit":"110.0000 CREDIT","ytoken":"10.0000 YTOKEN","reward":"1.2000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### merchant redeems the earned YTOKEN to USD fiat money
$INFRA_CLI push action ycard.usd.yt ytokenredeem '{"account":"ycardshopbbb","qty":"50.0000 YTOKEN","redeemed":"50.0000 USD","tag":""}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c
### merchant redeems the earned YTOKEN to YUSD token
$INFRA_CLI push action ycard.usd.yt ytokenredeem '{"account":"ycardshopbbb","qty":"70.0000 YTOKEN","redeemed":"70.0000 YUSD","tag":""}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c


#####################################################
##### Yosemite Card - Cancel Payment

### yosemite card payment using CREDIT + YTOKEN (anonymous user)
$INFRA_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopaaa","price":"50.0000 YTOKEN","credit":"45.0000 CREDIT","ytoken":"5.0000 YTOKEN","reward":"7.5000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### cancel previous payment
$INFRA_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopaaa","price":"-50.0000 YTOKEN","credit":"-45.0000 CREDIT","ytoken":"-5.0000 YTOKEN","reward":"-7.5000 YTOKEN","tag":"cancel yosemite card payment"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### cannot cancel the payment already settled for a merchant not issuing merchant-backed Yosemite Token
#$INFRA_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopbbb","price":"-120.0000 YTOKEN","credit":"-110.0000 CREDIT","ytoken":"-10.0000 YTOKEN","reward":"-1.2000 YTOKEN","tag":"cancel yosemite card payment"}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### payment for a merchant not issuing merchant-backed yosemite tokens
$INFRA_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopbbb","price":"100.0000 YTOKEN","credit":"80.0000 CREDIT","ytoken":"20.0000 YTOKEN","reward":"0.8000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### cancel payment (rollback transferred yosemite token)
$INFRA_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopbbb","price":"-100.0000 YTOKEN","credit":"-80.0000 CREDIT","ytoken":"-20.0000 YTOKEN","reward":"-0.8000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c