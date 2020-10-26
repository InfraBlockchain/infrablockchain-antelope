#!/usr/bin/env bash
# chmod +x ./start_yosemite_testnet_node_local.sh

# for zsh
setopt shwordsplit

YOSEMITE_CLI_BIN_NAME=clyos
YOSEMITE_HOME=/Users/bwlim/Documents/__HalfDomeChain__/yosemite-public-blockchain-git
YOSEMITE_CLI="$YOSEMITE_HOME/build/programs/$YOSEMITE_CLI_BIN_NAME/$YOSEMITE_CLI_BIN_NAME --wallet-url http://127.0.0.1:8900/"
YOSEMITE_CLI_TESTNET="$YOSEMITE_CLI -u http://testnet-sentinel.yosemitelabs.org:8888"
YOSEMITE_CONTRACTS_DIR=$YOSEMITE_HOME/build/contracts

###############################################################################################
###### AllCamper AKRW Token, Camping Car Trade/Rental smart contract test transactions

#$YOSEMITE_CLI_TESTNET wallet import --private-key YPV_5K5Qe2ndgcfhWqdUjHnfhvCKVqCE82Rih6JrNQuuVJBcathAx9m
#$YOSEMITE_CLI_TESTNET create account yosemite idauth.a YOS6EfGUaA5MNLH1GiHd64DcDr3HMgY1AM3WR1vdHKaah9Z4cWPZq -p yosemite@active --txfee-payer yosemite


#####################################
### AKRW Token Account Setting

#$YOSEMITE_CLI create key --to-console
$YOSEMITE_CLI wallet import --private-key YPV_5HuuX4d8ipdnZRRvyMiqvB4aNvLYHNCZeemtSfi6pjixVjEUd8F
$YOSEMITE_CLI_TESTNET create account yosemite akrw.token YOS5DVYgfHZK9h6hu8xww6NGLFRKH6V4ih7YoTLPb4NaUA7StXAo6 -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI_TESTNET set contract akrw.token $YOSEMITE_CONTRACTS_DIR/allcamper.akrw.token/ -p akrw.token@active --txfee-payer yosemite

$YOSEMITE_CLI_TESTNET push action akrw.token settokenmeta '{"sym":"0,AKRW","url":"https://allcamper.net","desc":"AllCamper KRW token"}' -p akrw.token@active --txfee-payer yosemite
$YOSEMITE_CLI_TESTNET push action yx.identity setidinfo "{\"account\":\"akrw.token\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI_TESTNET push action yosemite addsystoken '{"token":"akrw.token","weight":100000}' -p yosemite@active --txfee-payer yosemite


######################################################
### AllCamper Trade/Rental Contract Account Setting

#$YOSEMITE_CLI create key --to-console
$YOSEMITE_CLI wallet import --private-key YPV_5HsaeP8yGouGSG8HF1teyHEUhoRuEn8wYdhTNzpzqLhTeevrHqL
$YOSEMITE_CLI_TESTNET create account yosemite allcamper YOS7jteqfnSq7ma25qLRsBBXSKtpQGyru3QY317hJaD8NBfa6MnsJ -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI_TESTNET set contract allcamper $YOSEMITE_CONTRACTS_DIR/allcamper.contract/ -p allcamper@active --txfee-payer yosemite
$YOSEMITE_CLI_TESTNET set account permission allcamper active allcamper --add-code --txfee-payer yosemite
$YOSEMITE_CLI_TESTNET push action yx.identity setidinfo "{\"account\":\"allcamper\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a


###############################################################################
### AllCamper Service Account Setting (Account Creator, Tx Fee Payer)

#$YOSEMITE_CLI create key --to-console
$YOSEMITE_CLI wallet import --private-key YPV_5JhtKNmDyKLfLcbBwQZpQ9wVCSNhPt7wFfqwg9hM8LiudFFUa5v
$YOSEMITE_CLI_TESTNET create account yosemite allcampersvc YOS76D5jw2XziHbYP47QK4qRxhW4FWdf6vAyEng9Sp4z572BhDrfN -p yosemite@active --txfee-payer yosemite

$YOSEMITE_CLI_TESTNET push action yx.identity setidinfo "{\"account\":\"allcampersvc\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a
$YOSEMITE_CLI_TESTNET push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"allcampersvc","qty":"50000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.a

$YOSEMITE_CLI_TESTNET get token balance ysmt.dusd.a allcampersvc


#####################################
### Create AllCamper User Account and Register User Account On AllCamper Contract

#$YOSEMITE_CLI create key --to-console
$YOSEMITE_CLI wallet import --private-key YPV_5KcF2uRJNBt4pmnFMJzpMZh7JxREqV4SvzdZZXPHRWobE4jJy5G
$YOSEMITE_CLI_TESTNET create account allcampersvc camperaaa111 YOS89FZC81jpvLiZ1vYSckMKAwwuS2zkwTKhREsh1FTYtxZu1Smxe -p allcampersvc@active --txfee-payer allcampersvc -v producer.a
$YOSEMITE_CLI_TESTNET push action yx.identity setidinfo "{\"account\":\"camperaaa111\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a -v producer.a
$YOSEMITE_CLI_TESTNET push action allcamper reguser '{"account":"camperaaa111","user_info_multihash":"QmWWQSuPMS6aXCbZKpEjPHPUZN2NjB3YrhJTHsV4X3vb2t"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get account camperaaa111
$YOSEMITE_CLI_TESTNET get table -L camperaaa111 -U camperaaa111 -l 1 allcamper allcamper accounts


#$YOSEMITE_CLI create key --to-console
$YOSEMITE_CLI wallet import --private-key YPV_5JYb9i3En3U8mkcqKDPWYmShtTkVV7y58i7uoEVd96RSHYpNT3s
$YOSEMITE_CLI_TESTNET create account allcampersvc camperaaa112 YOS79hHZwGnQBC74UyiWqzJDxuQT3ULDHo7z3c8thwKtka1VDSbsU -p allcampersvc@active --txfee-payer allcampersvc -v producer.a
$YOSEMITE_CLI_TESTNET push action yx.identity setidinfo "{\"account\":\"camperaaa112\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a -v producer.a
$YOSEMITE_CLI_TESTNET push action allcamper reguser '{"account":"camperaaa112","user_info_multihash":"QmT4AeWE9Q9EaoyLJiqaZuYQ8mJeq4ZBncjjFH9dQ9uDVA"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get account camperaaa112
$YOSEMITE_CLI_TESTNET get table -L camperaaa112 -U camperaaa112 -l 1 allcamper allcamper accounts

#$YOSEMITE_CLI create key --to-console
$YOSEMITE_CLI wallet import --private-key YPV_5JwxWNNX8fxjEKJFu4mYQR3JiJZdqBRiadRL4WFTArefr8fpkYN
$YOSEMITE_CLI_TESTNET create account allcampersvc camperaaa113 YOS6QfZkcNhJPvAEUA1XEfLMdYTsoY5YGcUUyAv6HF9KkqajB1ryP -p allcampersvc@active --txfee-payer allcampersvc -v producer.a
$YOSEMITE_CLI_TESTNET push action yx.identity setidinfo "{\"account\":\"camperaaa113\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a -v producer.a
$YOSEMITE_CLI_TESTNET push action allcamper reguser '{"account":"camperaaa113","user_info_multihash":"QmT9qk3CRYbFDWpDFYeAv8T8H1gnongwKhh5J68NLkLir6"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get account camperaaa113
$YOSEMITE_CLI_TESTNET get table -L camperaaa113 -U camperaaa113 -l 1 allcamper allcamper accounts


#########################################
### Update AllCamper User Account Info

$YOSEMITE_CLI_TESTNET push action allcamper updateuser '{"account":"camperaaa113","user_info_multihash":"QmV8cfu6n4NT5xRr2AHdKxFMTZEJrA44qgrBCr739BN9Wb"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get table -L camperaaa113 -U camperaaa113 -l 1 allcamper allcamper accounts


#########################################
### Register Camping Car

$YOSEMITE_CLI_TESTNET push action allcamper regcampcar '{"car_id":10001,"owner":"camperaaa111","car_info_multihash":"QmWATWQ7fVPP2EFGu71UkfnqhYXDYH566qy47CnJDgvs8u"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars

$YOSEMITE_CLI_TESTNET push action allcamper regcampcar '{"car_id":10002,"owner":"camperaaa112","car_info_multihash":"QmVtYjNij3KeyGmcgg7yVXWskLaBtov3UYL9pgcGK3MCWu"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get table -L 10002 -U 10002 -l 1 allcamper allcamper campingcars


#########################################
### Update Camping Car Info

$YOSEMITE_CLI_TESTNET push action allcamper updatecar '{"car_id":10001,"owner":"camperaaa111","car_info_multihash":"QmcaHpwn3bs9DaeLsrk9ZvVxVcKTPXVWiU1XdrGNW9hpi3"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars

$YOSEMITE_CLI_TESTNET push action allcamper updatecar '{"car_id":10002,"owner":"camperaaa112","car_info_multihash":"QmbUSy8HCn8J4TMDRRdxCbK2uCCtkQyZtY6XYv3y7kLgDC"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get table -L 10002 -U 10002 -l 1 allcamper allcamper campingcars


#################################
### Issue AKRW To Use Account

$YOSEMITE_CLI_TESTNET push action akrw.token issue '{"t":"akrw.token","to":"camperaaa111","qty":"10000 AKRW","tag":""}' -p akrw.token@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa111

$YOSEMITE_CLI_TESTNET push action akrw.token issue '{"t":"akrw.token","to":"camperaaa112","qty":"1000000 AKRW","tag":""}' -p akrw.token@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa112

$YOSEMITE_CLI_TESTNET push action akrw.token issue '{"t":"akrw.token","to":"camperaaa113","qty":"3000000 AKRW","tag":""}' -p akrw.token@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa113


####################################
### Reserve Camping Car Purchasing

$YOSEMITE_CLI_TESTNET push action akrw.token transfer '{"t":"akrw.token","from":"camperaaa112","to":"allcamper","qty":"700000 AKRW","tag":"buy camping car"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a
$YOSEMITE_CLI_TESTNET push action allcamper rsvcartrade '{"deal_id":5001,"car_id":10001,"buyer":"camperaaa112","price":"700000 AKRW"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa112
$YOSEMITE_CLI_TESTNET get token balance akrw.token allcamper
$YOSEMITE_CLI_TESTNET get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars
$YOSEMITE_CLI_TESTNET get table -L 5001 -U 5001 -l 1 allcamper allcamper trades

$YOSEMITE_CLI_TESTNET push action akrw.token transfer '{"t":"akrw.token","from":"camperaaa113","to":"allcamper","qty":"500000 AKRW","tag":"buy camping car"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a
$YOSEMITE_CLI_TESTNET push action allcamper rsvcartrade '{"deal_id":5002,"car_id":10002,"buyer":"camperaaa113","price":"500000 AKRW"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa113
$YOSEMITE_CLI_TESTNET get token balance akrw.token allcamper
$YOSEMITE_CLI_TESTNET get table -L 10002 -U 10002 -l 1 allcamper allcamper campingcars
$YOSEMITE_CLI_TESTNET get table -L 5002 -U 5002 -l 1 allcamper allcamper trades


####################################
### Confirm Camping Car Trade

$YOSEMITE_CLI_TESTNET push action allcamper cnfmcartrade '{"deal_id":5001,"seller":"camperaaa111","buyer":"camperaaa112"}' -p camperaaa111@active -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars
$YOSEMITE_CLI_TESTNET get table -L 5001 -U 5001 -l 1 allcamper allcamper trades
$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa111
$YOSEMITE_CLI_TESTNET get token balance akrw.token allcamper


####################################
### Cancel Camping Car Trade

$YOSEMITE_CLI_TESTNET push action allcamper cnclcartrade '{"deal_id":5002,"canceler":"camperaaa112"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get table -L 10002 -U 10002 -l 1 allcamper allcamper campingcars
$YOSEMITE_CLI_TESTNET get table -L 5002 -U 5002 -l 1 allcamper allcamper trades
$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa113
$YOSEMITE_CLI_TESTNET get token balance akrw.token allcamper



####################################
### Reserve Camping Car Rental

$YOSEMITE_CLI_TESTNET push action akrw.token transfer '{"t":"akrw.token","from":"camperaaa113","to":"allcamper","qty":"10000 AKRW","tag":"rent camping car"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a
$YOSEMITE_CLI_TESTNET push action allcamper rsvrentcar '{"rent_id":7001,"car_id":10001,"renter":"camperaaa113","price":"10000 AKRW"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa113
$YOSEMITE_CLI_TESTNET get token balance akrw.token allcamper
$YOSEMITE_CLI_TESTNET get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars
$YOSEMITE_CLI_TESTNET get table -L 7001 -U 7001 -l 1 allcamper allcamper rentals


$YOSEMITE_CLI_TESTNET push action akrw.token transfer '{"t":"akrw.token","from":"camperaaa111","to":"allcamper","qty":"20000 AKRW","tag":"rent camping car"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a
$YOSEMITE_CLI_TESTNET push action allcamper rsvrentcar '{"rent_id":7002,"car_id":10002,"renter":"camperaaa111","price":"20000 AKRW"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa111
$YOSEMITE_CLI_TESTNET get token balance akrw.token allcamper
$YOSEMITE_CLI_TESTNET get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars
$YOSEMITE_CLI_TESTNET get table -L 7001 -U 7001 -l 1 allcamper allcamper rentals


####################################
### Confirm Camping Car Rental

$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa112

$YOSEMITE_CLI_TESTNET push action allcamper cnfmrentcar '{"rent_id":7001,"owner":"camperaaa112","renter":"camperaaa113"}' -p camperaaa112@active -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars
$YOSEMITE_CLI_TESTNET get table -L 7001 -U 7001 -l 1 allcamper allcamper rentals
$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa112
$YOSEMITE_CLI_TESTNET get token balance akrw.token allcamper


####################################
### Cancel Camping Car Rental

$YOSEMITE_CLI_TESTNET push action allcamper cnclrentcar '{"rent_id":7002,"canceler":"camperaaa111"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get table -L 10002 -U 10002 -l 1 allcamper allcamper campingcars
$YOSEMITE_CLI_TESTNET get table -L 7002 -U 7002 -l 1 allcamper allcamper rentals
$YOSEMITE_CLI_TESTNET get token balance akrw.token camperaaa111
$YOSEMITE_CLI_TESTNET get token balance akrw.token allcamper


####################################
### Review Camping Car Rental

$YOSEMITE_CLI_TESTNET push action allcamper reviewrental '{"rent_id":7001,"renter":"camperaaa113","score":7.5,"review_data_multihash":"QmfM2r8seH2GiRaC4esTjeraXEachRt8ZsSeGaWTPLyMoG"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$YOSEMITE_CLI_TESTNET get table -L 7001 -U 7001 -l 1 allcamper allcamper reviews
$YOSEMITE_CLI_TESTNET get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars


##################################
## Sentinel Testnet Explorer

## AllCamper AKRW Token Contract
http://testnet-sentinel-explorer.yosemitelabs.org/accounts/akrw.token

## AllCamper Trade/Rental Contract
http://testnet-sentinel-explorer.yosemitelabs.org/accounts/allcamper

## AllCamper User Account
http://testnet-sentinel-explorer.yosemitelabs.org/accounts/camperaaa111


curl -XPOST http://testnet-sentinel.yosemitelabs.org:8888/v1/chain/get_token_balance -d '{"token":"akrw.token","account":"camperaaa113"}' | jq