#!/usr/bin/env bash
# chmod +x ./allcamper_local_testnet_transaction_examples.sh

# for zsh
setopt shwordsplit

INFRA_CLI_BIN_NAME=infra-cli
INFRABLOCKCHAIN_HOME=/Users/bwlim/Documents/__InfraBlockchain__/infrablockchain-git
INFRA_CLI="$INFRABLOCKCHAIN_HOME/build/programs/$INFRA_CLI_BIN_NAME/$INFRA_CLI_BIN_NAME --wallet-url http://127.0.0.1:8900/"
INFRABLOCKCHAIN_CONTRACTS_DIR=$INFRABLOCKCHAIN_HOME/build/contracts

###############################################################################################
###### AllCamper AKRW Token, Camping Car Trade/Rental smart contract test transactions

#$INFRA_CLI wallet import --private-key PVT_K1_2So2Tt7dAoMR6tk9npz1WrnbAgHadbEhbmkLChRvggduxnwxAn
#$INFRA_CLI create account infrasys idauth.a PUB_K1_8QZSZGRFMiXk9GGEc7jPsyqZVF1CkJh16vHmnVt4PopkRZeyZJ -p infrasys@active --txfee-payer infrasys


#####################################
### AKRW Token Account Setting

#$INFRA_CLI create key --to-console
$INFRA_CLI wallet import --private-key PVT_K1_2VWJgnxdisC8aNWTkzTWuFpymevJ7kKohTL8hZyAmSCJknn1tk
$INFRA_CLI create account infrasys akrw.token PUB_K1_7CSGzHK1h5Vmw5jtvBAVfR4hLsEyCXAHC4tqojUVXwrUjwWsmU -p infrasys@active --txfee-payer infrasys

$INFRA_CLI set contract akrw.token $INFRABLOCKCHAIN_CONTRACTS_DIR/allcamper.akrw.token/ -p akrw.token@active --txfee-payer infrasys

$INFRA_CLI push action akrw.token settokenmeta '{"sym":"0,AKRW","url":"https://allcamper.net","desc":"AllCamper KRW token"}' -p akrw.token@active --txfee-payer infrasys
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"akrw.token\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a
$INFRA_CLI push action infrasys addsystoken '{"token":"akrw.token","weight":100000}' -p infrasys@active --txfee-payer infrasys


######################################################
### AllCamper Trade/Rental Contract Account Setting

#$INFRA_CLI create key --to-console
$INFRA_CLI wallet import --private-key PVT_K1_2kPWwhKeRNX4SE2XhXYQA3seCkVyuvZe71aBSF6vvbqP7ui5M8
$INFRA_CLI create account infrasys allcamper PUB_K1_7Qt7nWLvhJ5v7BwEvgzHf5Q3UJDiRVFLNTviEcDLXUHdhrxr1u -p infrasys@active --txfee-payer infrasys

$INFRA_CLI set contract allcamper $INFRABLOCKCHAIN_CONTRACTS_DIR/allcamper.contract/ -p allcamper@active --txfee-payer infrasys
$INFRA_CLI set account permission allcamper active allcamper --add-code --txfee-payer infrasys
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"allcamper\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a


###############################################################################
### AllCamper Service Account Setting (Account Creator, Tx Fee Payer)

#$INFRA_CLI create key --to-console
$INFRA_CLI wallet import --private-key PVT_K1_m2EyfZ4YNL76Ku1AarpJWYdkUWidqKTKnFcHwiJwU9ytLUt3y
$INFRA_CLI create account infrasys allcampersvc PUB_K1_5qKtzkewKvEFP7guiHwGydazf65JFPt1oenDJGsQqAHgxbktVT -p infrasys@active --txfee-payer infrasys

$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"allcampersvc\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a
$INFRA_CLI push action ysmt.dusd.a issue '{"t":"ysmt.dusd.a","to":"allcampersvc","qty":"50000.0000 DUSD","tag":""}' -p ysmt.dusd.a@active --txfee-payer ysmt.dusd.a -v producer.a

$INFRA_CLI get token balance ysmt.dusd.a allcampersvc


#####################################
### Create AllCamper User Account and Register User Account On AllCamper Contract

#$INFRA_CLI create key --to-console
$INFRA_CLI wallet import --private-key PVT_K1_2b4oHy5zxbCPd4NZgd1jzLQrVwg5XsNUevk8mDoWtYPpDBHwkN
$INFRA_CLI create account allcampersvc camperaaa111 PUB_K1_7CeeMTb4EotPGzc6LQ6wgwM2fc55gWVVjd4EkjvsYfrHpGeKrL -p allcampersvc@active --txfee-payer allcampersvc -v producer.a
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"camperaaa111\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a -v producer.a
$INFRA_CLI push action allcamper reguser '{"account":"camperaaa111","user_info_multihash":"QmWWQSuPMS6aXCbZKpEjPHPUZN2NjB3YrhJTHsV4X3vb2t"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get account camperaaa111
$INFRA_CLI get table -L camperaaa111 -U camperaaa111 -l 1 allcamper allcamper accounts


#$INFRA_CLI create key --to-console
$INFRA_CLI wallet import --private-key PVT_K1_2Cvn7wbUcLLpXzLcH4xkBtVRHN2TNfn2k9pCnXShJnSjejN1XS
$INFRA_CLI create account allcampersvc camperaaa112 PUB_K1_6XMEtKQtgx7kSkbs59oV6P74RiBxUrwEytNB6ZS7GdiDmueCBq -p allcampersvc@active --txfee-payer allcampersvc -v producer.a
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"camperaaa112\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a -v producer.a
$INFRA_CLI push action allcamper reguser '{"account":"camperaaa112","user_info_multihash":"QmT4AeWE9Q9EaoyLJiqaZuYQ8mJeq4ZBncjjFH9dQ9uDVA"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get account camperaaa112
$INFRA_CLI get table -L camperaaa112 -U camperaaa112 -l 1 allcamper allcamper accounts

#$INFRA_CLI create key --to-console
$INFRA_CLI wallet import --private-key PVT_K1_SVgLz8QFcnxc8fv2CbDPt8VMGKZAmLUiKe5MpjhFE22jZyLP5
$INFRA_CLI create account allcampersvc camperaaa113 PUB_K1_7T1FU2VMwuvrPBHeaHbrgr6Wu5vvh8u225LoccbFNLbnCQ8GV7 -p allcampersvc@active --txfee-payer allcampersvc -v producer.a
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"camperaaa113\", \"identity_authority\":\"idauth.a\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"\"}" -p idauth.a@active --txfee-payer idauth.a -v producer.a
$INFRA_CLI push action allcamper reguser '{"account":"camperaaa113","user_info_multihash":"QmT9qk3CRYbFDWpDFYeAv8T8H1gnongwKhh5J68NLkLir6"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get account camperaaa113
$INFRA_CLI get table -L camperaaa113 -U camperaaa113 -l 1 allcamper allcamper accounts


#########################################
### Update AllCamper User Account Info

$INFRA_CLI push action allcamper updateuser '{"account":"camperaaa113","user_info_multihash":"QmV8cfu6n4NT5xRr2AHdKxFMTZEJrA44qgrBCr739BN9Wb"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get table -L camperaaa113 -U camperaaa113 -l 1 allcamper allcamper accounts


#########################################
### Register Camping Car

$INFRA_CLI push action allcamper regcampcar '{"car_id":10001,"owner":"camperaaa111","car_info_multihash":"QmWATWQ7fVPP2EFGu71UkfnqhYXDYH566qy47CnJDgvs8u"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars

$INFRA_CLI push action allcamper regcampcar '{"car_id":10002,"owner":"camperaaa112","car_info_multihash":"QmVtYjNij3KeyGmcgg7yVXWskLaBtov3UYL9pgcGK3MCWu"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get table -L 10002 -U 10002 -l 1 allcamper allcamper campingcars


#########################################
### Update Camping Car Info

$INFRA_CLI push action allcamper updatecar '{"car_id":10001,"owner":"camperaaa111","car_info_multihash":"QmcaHpwn3bs9DaeLsrk9ZvVxVcKTPXVWiU1XdrGNW9hpi3"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars

$INFRA_CLI push action allcamper updatecar '{"car_id":10002,"owner":"camperaaa112","car_info_multihash":"QmbUSy8HCn8J4TMDRRdxCbK2uCCtkQyZtY6XYv3y7kLgDC"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get table -L 10002 -U 10002 -l 1 allcamper allcamper campingcars


#################################
### Issue AKRW To Use Account

$INFRA_CLI push action akrw.token issue '{"t":"akrw.token","to":"camperaaa111","qty":"10000 AKRW","tag":""}' -p akrw.token@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get token balance akrw.token camperaaa111

$INFRA_CLI push action akrw.token issue '{"t":"akrw.token","to":"camperaaa112","qty":"1000000 AKRW","tag":""}' -p akrw.token@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get token balance akrw.token camperaaa112

$INFRA_CLI push action akrw.token issue '{"t":"akrw.token","to":"camperaaa113","qty":"3000000 AKRW","tag":""}' -p akrw.token@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get token balance akrw.token camperaaa113


####################################
### Reserve Camping Car Purchasing

$INFRA_CLI push action akrw.token transfer '{"t":"akrw.token","from":"camperaaa112","to":"allcamper","qty":"700000 AKRW","tag":"buy camping car"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a
$INFRA_CLI push action allcamper rsvcartrade '{"deal_id":5001,"car_id":10001,"buyer":"camperaaa112","price":"700000 AKRW"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get token balance akrw.token camperaaa112
$INFRA_CLI get token balance akrw.token allcamper
$INFRA_CLI get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars
$INFRA_CLI get table -L 5001 -U 5001 -l 1 allcamper allcamper trades

$INFRA_CLI push action akrw.token transfer '{"t":"akrw.token","from":"camperaaa113","to":"allcamper","qty":"500000 AKRW","tag":"buy camping car"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a
$INFRA_CLI push action allcamper rsvcartrade '{"deal_id":5002,"car_id":10002,"buyer":"camperaaa113","price":"500000 AKRW"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get token balance akrw.token camperaaa113
$INFRA_CLI get token balance akrw.token allcamper
$INFRA_CLI get table -L 10002 -U 10002 -l 1 allcamper allcamper campingcars
$INFRA_CLI get table -L 5002 -U 5002 -l 1 allcamper allcamper trades


####################################
### Confirm Camping Car Trade

$INFRA_CLI push action allcamper cnfmcartrade '{"deal_id":5001,"seller":"camperaaa111","buyer":"camperaaa112"}' -p camperaaa111@active -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars
$INFRA_CLI get table -L 5001 -U 5001 -l 1 allcamper allcamper trades
$INFRA_CLI get token balance akrw.token camperaaa111
$INFRA_CLI get token balance akrw.token allcamper


####################################
### Cancel Camping Car Trade

$INFRA_CLI push action allcamper cnclcartrade '{"deal_id":5002,"canceler":"camperaaa112"}' -p camperaaa112@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get table -L 10002 -U 10002 -l 1 allcamper allcamper campingcars
$INFRA_CLI get table -L 5002 -U 5002 -l 1 allcamper allcamper trades
$INFRA_CLI get token balance akrw.token camperaaa113
$INFRA_CLI get token balance akrw.token allcamper



####################################
### Reserve Camping Car Rental

$INFRA_CLI push action akrw.token transfer '{"t":"akrw.token","from":"camperaaa113","to":"allcamper","qty":"10000 AKRW","tag":"rent camping car"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a
$INFRA_CLI push action allcamper rsvrentcar '{"rent_id":7001,"car_id":10001,"renter":"camperaaa113","price":"10000 AKRW"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get token balance akrw.token camperaaa113
$INFRA_CLI get token balance akrw.token allcamper
$INFRA_CLI get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars
$INFRA_CLI get table -L 7001 -U 7001 -l 1 allcamper allcamper rentals


$INFRA_CLI push action akrw.token transfer '{"t":"akrw.token","from":"camperaaa111","to":"allcamper","qty":"20000 AKRW","tag":"rent camping car"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a
$INFRA_CLI push action allcamper rsvrentcar '{"rent_id":7002,"car_id":10002,"renter":"camperaaa111","price":"20000 AKRW"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get token balance akrw.token camperaaa111
$INFRA_CLI get token balance akrw.token allcamper
$INFRA_CLI get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars
$INFRA_CLI get table -L 7001 -U 7001 -l 1 allcamper allcamper rentals


####################################
### Confirm Camping Car Rental

$INFRA_CLI get token balance akrw.token camperaaa112

$INFRA_CLI push action allcamper cnfmrentcar '{"rent_id":7001,"owner":"camperaaa112","renter":"camperaaa113"}' -p camperaaa112@active -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars
$INFRA_CLI get table -L 7001 -U 7001 -l 1 allcamper allcamper rentals
$INFRA_CLI get token balance akrw.token camperaaa112
$INFRA_CLI get token balance akrw.token allcamper


####################################
### Cancel Camping Car Rental

$INFRA_CLI push action allcamper cnclrentcar '{"rent_id":7002,"canceler":"camperaaa111"}' -p camperaaa111@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get table -L 10002 -U 10002 -l 1 allcamper allcamper campingcars
$INFRA_CLI get table -L 7002 -U 7002 -l 1 allcamper allcamper rentals
$INFRA_CLI get token balance akrw.token camperaaa111
$INFRA_CLI get token balance akrw.token allcamper


####################################
### Review Camping Car Rental

$INFRA_CLI push action allcamper reviewrental '{"rent_id":7001,"renter":"camperaaa113","score":7.5,"review_data_multihash":"QmfM2r8seH2GiRaC4esTjeraXEachRt8ZsSeGaWTPLyMoG"}' -p camperaaa113@active --txfee-payer allcampersvc -v producer.a

$INFRA_CLI get table -L 7001 -U 7001 -l 1 allcamper allcamper reviews
$INFRA_CLI get table -L 10001 -U 10001 -l 1 allcamper allcamper campingcars

