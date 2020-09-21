#!/usr/bin/env bash
# chmod +x ./yosemite_card_ytoken_sample_transactions.sh

#### Yosemite Card Yosemite Token contract sample transactions

$YOSEMITE_CLI wallet import --private-key YPV_5Jz2zynNXzVrGtzu3BZE8AocZV9xfGuK1YtmyJhN8ydYaDhofVK
$YOSEMITE_CLI create account yosemite ycard.usd.yt YOS7v5xx9FHYEtaqUVHetLLyS6buGMpj4L9NDBpUjYUWACnvggcyt -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI wallet import --private-key YPV_5KJwxh2LyXU4yDmQHoyhsZX8hXgcs4quCYW6feasGx3ew8qbgxL
$YOSEMITE_CLI create account yosemite ycard.users YOS65muvcG3YCXA1NDKiSocpBhLtqZpfYVGjQKveT8fnhCnqppX6s -p yosemite@active --txfee-payer yosemite

sleep 2
$YOSEMITE_CLI set contract ycard.usd.yt $YOSEMITE_CONTRACTS_DIR/yosemite.card.ytoken/ -p ycard.usd.yt@active --txfee-payer yosemite
sleep 2

$YOSEMITE_CLI push action ycard.usd.yt settokenmeta '{"sym":"4,YTOKEN","url":"https://yosemitecardx.com","desc":"Yosemite Card Yosemite Token USD"}' -p ycard.usd.yt@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"ytokenissue","value":300,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"ytokenburn","value":500,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"ytokenredeem","value":500,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"ytokenpay","value":50,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"usdytissue","value":200,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"usdredeemto","value":200,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"yusdtransfer","value":100,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"yusdredeemrq","value":100,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"cnclyusdrdrq","value":100,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"creditlimit","value":100,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"creditissue","value":200,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"credittxfer","value":50,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"creditsettle","value":200,"feetype":1}' -p yosemite@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite settxfee '{"code":"ycard.usd.yt","action":"creditburn","value":50,"feetype":1}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action sys.identity setidinfo "{\"account\":\"ycard.usd.yt\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"ycard.usd.yt","qty":"10000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.a

$YOSEMITE_CLI get token balance ysmt.dusd.a ycard.usd.yt


$YOSEMITE_CLI wallet import --private-key YPV_5JGCdjc2Mn439e6f1YEcEzniPtSBeSZPdnuz6vsUaZB7KV1SnpT
$YOSEMITE_CLI create account yosemite ycard.id.us YOS8HxRbGMAMUsukWXbF4MxRVwAqRoSoYQnnYaV34PBiJTwFbfXGd -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action yosemite regidauth '{"identity_authority":"ycard.id.us","url":"https://yosemitecardx.com","location":1}' -p ycard.id.us@active --txfee-payer yosemite
$YOSEMITE_CLI push action yosemite authidauth '{"identity_authority":"ycard.id.us"}' -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI push action sys.identity setidinfo "{\"account\":\"ycard.id.us\", \"identity_authority\":\"ycard.id.us\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p ycard.id.us@active --txfee-payer yosemite
$YOSEMITE_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"ycard.id.us","qty":"10000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.a

# deposit USD-backed Yosemite Tokens for Yosemite Card service operation
$YOSEMITE_CLI push action ycard.usd.yt usdytissue '{"qty":"20000.0000 USD","tag":"issue USD-backed YTOKEN by Yosemite Card"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.d

$YOSEMITE_CLI get token balance ycard.usd.yt ycard.usd.yt
curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.usd.yt"}'; echo;

sleep 2

#################################################
### Yosemite Card User Account Setup
$YOSEMITE_CLI wallet import --private-key YPV_5KXuaLqsaniTc9qDJi851e1CBNjaEqUqoERyazRKUVBhwuCS8xr
#$YOSEMITE_CLI create account ycard.usd.yt ycarduseraaa YOS5HTxnZ6NZHKGjgZc6vK7bi6abtH7QEfEyu2rX1Lqm89mTyv2fk -p ycard.usd.yt@active --txfee-payer ycard.usd.yt
$YOSEMITE_CLI push action yosemite newaccount '{"creator":"ycard.usd.yt","name":"ycarduseraaa","owner":{"threshold":1,"keys":[{"key":"YOS5HTxnZ6NZHKGjgZc6vK7bi6abtH7QEfEyu2rX1Lqm89mTyv2fk","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]},"active":{"threshold":1,"keys":[{"key":"YOS5HTxnZ6NZHKGjgZc6vK7bi6abtH7QEfEyu2rX1Lqm89mTyv2fk","weight":1}],"accounts":[],"waits":[]}}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"ycarduseraaa","permission":"owner","parent":"","auth":{"threshold":1,"keys":[{"key":"YOS5HTxnZ6NZHKGjgZc6vK7bi6abtH7QEfEyu2rX1Lqm89mTyv2fk","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]}}' -p ycarduseraaa@owner --txfee-payer ycard.usd.yt

$YOSEMITE_CLI push action sys.identity setidinfo "{\"account\":\"ycarduseraaa\", \"identity_authority\":\"ycard.id.us\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p ycard.id.us@active --txfee-payer ycard.id.us -v producer.a

# ycarduseraaa issues credit tokens and deposits to ycard service
$YOSEMITE_CLI push action yosemite updateauth '{"account":"ycarduseraaa","permission":"creditissue","parent":"active","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"ycard.usd.yt","permission":"active"}}]}}' -p ycarduseraaa@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycarduseraaa","code":"ycard.usd.yt","type":"creditissue","requirement":"creditissue"}' -p ycarduseraaa@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite updateauth '{"account":"ycarduseraaa","permission":"codecrdtxfer","parent":"active","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"ycard.usd.yt","permission":"yx.code"}}]}}' -p ycarduseraaa@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycarduseraaa","code":"ycard.usd.yt","type":"credittxfer","requirement":"codecrdtxfer"}' -p ycarduseraaa@active --txfee-payer ycard.usd.yt -v producer.c

# offering credit limit to user account ycarduseraaa (issuing credit card)
$YOSEMITE_CLI push action ycard.usd.yt creditlimit '{"account":"ycarduseraaa","credit_limit":"1000.0000 CREDIT","tag":"credit offering"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.b

$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt creditoffer -L ycarduseraaa -l 1
curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"creditoffer","lower_bound":"ycarduseraaa","limit":1}'; echo;

$YOSEMITE_CLI push action ycard.usd.yt creditissue '{"issuer":"ycarduseraaa","to":"ycard.usd.yt","qty":"1000.0000 CREDIT","tag":"credit token issued by user, and deposited to yosemite card service"}' -p ycarduseraaa@creditissue --txfee-payer ycard.usd.yt -v producer.c

$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt creditoffer -L ycarduseraaa -l 1
curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"creditoffer","lower_bound":"ycarduseraaa","limit":1}'; echo;
$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt credittoken -L ycarduseraaa -l 1
curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"credittoken","lower_bound":"ycarduseraaa","limit":1}'; echo;
$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt credittoken -L ycard.usd.yt -l 1
curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"credittoken","lower_bound":"ycard.usd.yt","limit":1}'; echo;
$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt creditstat
curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"creditstat"}'; echo;

sleep 2

#################################################
##### Yosemite Card Merchant Account Setup
$YOSEMITE_CLI wallet import --private-key YPV_5JkupiGwhRCEwPon9EKMgkjscthhkUsrQYkUHTAF1Xp2hBYYKEW
#$YOSEMITE_CLI create account ycard.usd.yt ycardshopaaa YOS7XVhCkoiyPUaGGXhCLP1TM2HQtptZxY83aNuSWiJNVd2Gwoxvg -p ycard.usd.yt@active --txfee-payer ycard.usd.yt
$YOSEMITE_CLI push action yosemite newaccount '{"creator":"ycard.usd.yt","name":"ycardshopaaa","owner":{"threshold":1,"keys":[{"key":"YOS7XVhCkoiyPUaGGXhCLP1TM2HQtptZxY83aNuSWiJNVd2Gwoxvg","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]},"active":{"threshold":1,"keys":[{"key":"YOS7XVhCkoiyPUaGGXhCLP1TM2HQtptZxY83aNuSWiJNVd2Gwoxvg","weight":1}],"accounts":[],"waits":[]}}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"ycardshopaaa","permission":"owner","parent":"","auth":{"threshold":1,"keys":[{"key":"YOS7XVhCkoiyPUaGGXhCLP1TM2HQtptZxY83aNuSWiJNVd2Gwoxvg","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]}}' -p ycardshopaaa@owner --txfee-payer ycard.usd.yt

$YOSEMITE_CLI push action sys.identity setidinfo "{\"account\":\"ycardshopaaa\", \"identity_authority\":\"ycard.id.us\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1101' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p ycard.id.us@active --txfee-payer ycard.id.us -v producer.a

$YOSEMITE_CLI push action yosemite updateauth '{"account":"ycardshopaaa","permission":"ytokenaction","parent":"active","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"ycard.usd.yt","permission":"active"}}]}}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycardshopaaa","code":"ycard.usd.yt","type":"ytokenissue","requirement":"ytokenaction"}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycardshopaaa","code":"ycard.usd.yt","type":"ytokenburn","requirement":"ytokenaction"}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycardshopaaa","code":"ycard.usd.yt","type":"ytokenredeem","requirement":"ytokenaction"}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycardshopaaa","code":"ycard.usd.yt","type":"ytokenpay","requirement":"ytokenaction"}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycardshopaaa","code":"ycard.usd.yt","type":"yusdredeemrq","requirement":"ytokenaction"}' -p ycardshopaaa@active --txfee-payer ycard.usd.yt -v producer.c

#################################################
#### Issue Yosemite Token backed by Merchant-issued gift card token

# merchant issues gift token backed yosemite token, yosemite card prepays USD to the merchant
$YOSEMITE_CLI push action ycard.usd.yt ytokenissue '{"merchant":"ycardshopaaa","qty":"700.0000 YTOKEN","paid":"500.0000 USD","tag":"issue merchant gift token backed yosemite token"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt ytokenissue -L ycardshopaaa -l 1
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenissue","lower_bound":"ycardshopaaa","limit":1}'; echo;

$YOSEMITE_CLI get token balance ycard.usd.yt ycard.usd.yt
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.usd.yt"}'; echo;

sleep 2

#################################################
##### Yosemite Card Payment Transactions

### yosemite card payment (anonymous user)
$YOSEMITE_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopaaa","price":"24.0000 YTOKEN","credit":"24.0000 CREDIT","ytoken":"0.0000 YTOKEN","reward":"6.0000 YTOKEN","tag":"yosemite card anonymous payment"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

$YOSEMITE_CLI get token balance ycard.usd.yt ycard.usd.yt
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.usd.yt"}'; echo;
$YOSEMITE_CLI get token balance ycard.usd.yt ycard.users
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.users"}'; echo;

$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt ytokenissue -L ycardshopaaa -l 1
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenissue","lower_bound":"ycardshopaaa","limit":1}'; echo;
$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt ytokenissue -L usdbacked -l 1
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenissue","lower_bound":"usdbacked","limit":1}'; echo;

### yosemite card payment using CREDIT + YTOKEN (anonymous user)
$YOSEMITE_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopaaa","price":"32.0000 YTOKEN","credit":"28.0000 CREDIT","ytoken":"4.0000 YTOKEN","reward":"7.0000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt ytokenissue -L ycardshopaaa -l 1
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenissue","lower_bound":"ycardshopaaa","limit":1}'; echo;
$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt ytokenissue -L usdbacked -l 1
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenissue","lower_bound":"usdbacked","limit":1}'; echo;

$YOSEMITE_CLI get token balance ycard.usd.yt ycard.usd.yt
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.usd.yt"}'; echo;
$YOSEMITE_CLI get token balance ycard.usd.yt ycard.users
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_token_balance -d '{"token":"ycard.usd.yt","account":"ycard.users"}'; echo;

$YOSEMITE_CLI get token info ycard.usd.yt
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_token_info -d '{"token":"ycard.usd.yt"}'; echo;
$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt ytokenstat
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"ytokenstat"}'; echo;

$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt creditstat
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"creditstat"}'; echo;

$YOSEMITE_CLI get table ycard.usd.yt ycard.usd.yt yusdstat
#curl ${YOSEMITE_NODE_API_ENDPOINT}/v1/chain/get_table_rows -d '{"json":true,"code":"ycard.usd.yt","scope":"ycard.usd.yt","table":"yusdstat"}'; echo;

### yosemite card payment using CREDIT + YTOKEN (anonymous user)
$YOSEMITE_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopaaa","price":"350.0000 YTOKEN","credit":"342.0000 CREDIT","ytoken":"8.0000 YTOKEN","reward":"81.0000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

sleep 2

#####################################################
##### Monthly user credit balance settlement

### Yosemite Card user(s) sends money off-chain to Yosemite Card as credit balance settlement
$YOSEMITE_CLI push action ycard.usd.yt usdytissue '{"qty":"394.0000 USD","tag":"YCardUserMonthlyCreditBalancePayment"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.d

$YOSEMITE_CLI push action ycard.usd.yt creditburn '{"qty":"606.0000 CREDIT","tag":"burn unused credit"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.g

$YOSEMITE_CLI push action ycard.usd.yt creditsettle '{"account":"ycarduseraaa","qty":"1000.0000 CREDIT","tag":"reset user credit offering"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.e

$YOSEMITE_CLI push action ycard.usd.yt creditissue '{"issuer":"ycarduseraaa","to":"ycard.usd.yt","qty":"1000.0000 CREDIT","tag":"monthly issue credit token by user, and deposit to ycard service"}' -p ycarduseraaa@creditissue --txfee-payer ycard.usd.yt -v producer.e


#####################################################
##### Burn existing merchant-issued Yosemite Token (store-credit) and Re-issue merchant-issued Yosemite Tokens

$YOSEMITE_CLI push action ycard.usd.yt ytokenburn '{"merchant":"ycardshopaaa","qty":"294.0000 YTOKEN","tag":""}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.d
### merchant refunded USD to Yosemite Card Services
$YOSEMITE_CLI push action ycard.usd.yt usdytissue '{"qty":"210.0000 USD","tag":"merchant refunded money for burning merchant-backed Yosemite Tokens"}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.d

### re-issue merchant backed Yosemite Tokens, YUSD token is paid to merchant account for future redeeming to USD fiat money
$YOSEMITE_CLI push action ycard.usd.yt ytokenissue '{"merchant":"ycardshopaaa","qty":"600.0000 YTOKEN","paid":"500.0000 YUSD","tag":"issue merchant gift token backed yosemite token"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c


#####################################################
##### Redeem YUSD to real fiat money

### YUSD redeem request
$YOSEMITE_CLI push action ycard.usd.yt yusdredeemrq '{"account":"ycardshopaaa","qty":"300.0000 YUSD","tag":""}' -p ycardshopaaa@ytokenaction --txfee-payer ycard.usd.yt -v producer.c

### Cancel YUSD redeem request
$YOSEMITE_CLI push action ycard.usd.yt cnclyusdrdrq '{"account":"ycardshopaaa","qty":"300.0000 YUSD","tag":""}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### YUSD redeem request
$YOSEMITE_CLI push action ycard.usd.yt yusdredeemrq '{"account":"ycardshopaaa","qty":"500.0000 YUSD","tag":""}' -p ycardshopaaa@ytokenaction --txfee-payer ycard.usd.yt -v producer.c

### Yosemite Card completed YUSD redeem to merchant (transferred USD fund to merchant's bank account)
$YOSEMITE_CLI push action ycard.usd.yt usdredeemto '{"to":"ycardshopaaa","qty":"500.0000 YUSD","tag":""}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c


#####################################################
##### Yosemite Card Payment to a merchant who does not issue merchant-backed Yosemite Token

$YOSEMITE_CLI wallet import --private-key YPV_5HtaMCXLonpjpk7ZsWncTKb2t4LpkveMTRKGaXW3Jd6D5g1b8ax
#$YOSEMITE_CLI create account ycard.usd.yt ycarduserbbb YOS8MTDtmS9zkYSzK6J5rNkuPrXBSfkvQTm9XF5vJjRsXxBcorbXw -p ycard.usd.yt@active --txfee-payer ycard.usd.yt
$YOSEMITE_CLI push action yosemite newaccount '{"creator":"ycard.usd.yt","name":"ycardshopbbb","owner":{"threshold":1,"keys":[{"key":"YOS8MTDtmS9zkYSzK6J5rNkuPrXBSfkvQTm9XF5vJjRsXxBcorbXw","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]},"active":{"threshold":1,"keys":[{"key":"YOS8MTDtmS9zkYSzK6J5rNkuPrXBSfkvQTm9XF5vJjRsXxBcorbXw","weight":1}],"accounts":[],"waits":[]}}' -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c
#$YOSEMITE_CLI push action yosemite updateauth '{"account":"ycardshopbbb","permission":"owner","parent":"","auth":{"threshold":1,"keys":[{"key":"YOS8MTDtmS9zkYSzK6J5rNkuPrXBSfkvQTm9XF5vJjRsXxBcorbXw","weight":1}],"accounts":[{"permission":{"actor":"ycard.id.us","permission":"active"},"weight":1}],"waits":[]}}' -p ycardshopbbb@owner --txfee-payer ycard.usd.yt

$YOSEMITE_CLI push action sys.identity setidinfo "{\"account\":\"ycardshopbbb\", \"identity_authority\":\"ycard.id.us\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1101' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p ycard.id.us@active --txfee-payer ycard.id.us -v producer.a

$YOSEMITE_CLI push action yosemite updateauth '{"account":"ycardshopbbb","permission":"ytokenaction","parent":"active","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"ycard.usd.yt","permission":"active"}}]}}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycardshopbbb","code":"ycard.usd.yt","type":"ytokenissue","requirement":"ytokenaction"}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycardshopbbb","code":"ycard.usd.yt","type":"ytokenburn","requirement":"ytokenaction"}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycardshopbbb","code":"ycard.usd.yt","type":"ytokenredeem","requirement":"ytokenaction"}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycardshopbbb","code":"ycard.usd.yt","type":"ytokenpay","requirement":"ytokenaction"}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c
$YOSEMITE_CLI push action yosemite linkauth '{"account":"ycardshopbbb","code":"ycard.usd.yt","type":"yusdredeemrq","requirement":"ytokenaction"}' -p ycardshopbbb@active --txfee-payer ycard.usd.yt -v producer.c

### yosemite card payment using CREDIT + YTOKEN (anonymous user)
$YOSEMITE_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopbbb","price":"120.0000 YTOKEN","credit":"110.0000 CREDIT","ytoken":"10.0000 YTOKEN","reward":"1.2000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### merchant redeems the earned YTOKEN to USD fiat money
$YOSEMITE_CLI push action ycard.usd.yt ytokenredeem '{"account":"ycardshopbbb","qty":"50.0000 YTOKEN","redeemed":"50.0000 USD","tag":""}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c
### merchant redeems the earned YTOKEN to YUSD token
$YOSEMITE_CLI push action ycard.usd.yt ytokenredeem '{"account":"ycardshopbbb","qty":"70.0000 YTOKEN","redeemed":"70.0000 YUSD","tag":""}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c


#####################################################
##### Yosemite Card - Cancel Payment

### yosemite card payment using CREDIT + YTOKEN (anonymous user)
$YOSEMITE_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopaaa","price":"50.0000 YTOKEN","credit":"45.0000 CREDIT","ytoken":"5.0000 YTOKEN","reward":"7.5000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### cancel previous payment
$YOSEMITE_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopaaa","price":"-50.0000 YTOKEN","credit":"-45.0000 CREDIT","ytoken":"-5.0000 YTOKEN","reward":"-7.5000 YTOKEN","tag":"cancel yosemite card payment"}' -p ycardshopaaa@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### cannot cancel the payment already settled for a merchant not issuing merchant-backed Yosemite Token
#$YOSEMITE_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopbbb","price":"-120.0000 YTOKEN","credit":"-110.0000 CREDIT","ytoken":"-10.0000 YTOKEN","reward":"-1.2000 YTOKEN","tag":"cancel yosemite card payment"}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### payment for a merchant not issuing merchant-backed yosemite tokens
$YOSEMITE_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopbbb","price":"100.0000 YTOKEN","credit":"80.0000 CREDIT","ytoken":"20.0000 YTOKEN","reward":"0.8000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c

### cancel payment (rollback transferred yosemite token)
$YOSEMITE_CLI push action ycard.usd.yt ytokenpay '{"merchant":"ycardshopbbb","price":"-100.0000 YTOKEN","credit":"-80.0000 CREDIT","ytoken":"-20.0000 YTOKEN","reward":"-0.8000 YTOKEN","tag":"yosemite card anonymous payment using YTOKEN"}' -p ycardshopbbb@ytokenaction -p ycard.usd.yt@active --txfee-payer ycard.usd.yt -v producer.c