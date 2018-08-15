YOSEMITE System Contract 
===

* block producer management and election of block producers (currently permissioned setup)
* claiming block producer rewards from transaction fee profit pool
* account management (newaccount, updateauth, deleteauth, linkauth, unlinkauth, setpriv)
* managing the registration of System Depositories having right for issuing native fiat-stable token
* managing the registration of Identity Authroities, the authorized Identity Authorities have the right for managing user info and KYC info on blockchain.

Environment Var.
---

```bash
YOSEMITE_CLEOS=$YOSEMITE_HOME/build/programs/cleos/cleos
YOSEMITE_CONTRACTS_DIR=$YOSEMITE_HOME/build/contracts
```

Install Yosemite System Contract
---
```bash
$YOSEMITE_CLEOS set contract eosio $YOSEMITE_CONTRACTS_DIR/yx.system/ -p eosio@active
```

Create initial System Depository Account
---

* sysdepo1
```bash
$YOSEMITE_CLEOS create key
Private key: 5KCqedPhHjbaBRi4ujhFsS3aGCcNuXwVGrMaxRW3DvB5Wo3beax
Public key: EOS7myk8qduMNnxo9Q9RZ2uxfaerQhXMPtxKUhxKh5HfSSM1dHeQo
$YOSEMITE_CLEOS wallet import --private-key 5KCqedPhHjbaBRi4ujhFsS3aGCcNuXwVGrMaxRW3DvB5Wo3beax

$YOSEMITE_CLEOS create account eosio sysdepo1 EOS7myk8qduMNnxo9Q9RZ2uxfaerQhXMPtxKUhxKh5HfSSM1dHeQo

```

* sysdepo2
```bash
$YOSEMITE_CLEOS create key
Private key: 5HyRsRWBi4bUXjmnmaskw9BU8a2sdqn3Jfqt21Few9xUyYR5tXr
Public key: EOS62AXYcUxcSQQgKhnQ1oid2w1XNvCKxmPgk7C8fEi4FnQKhHLwM
$YOSEMITE_CLEOS wallet import --private-key 5HyRsRWBi4bUXjmnmaskw9BU8a2sdqn3Jfqt21Few9xUyYR5tXr

$YOSEMITE_CLEOS create account eosio sysdepo2 EOS62AXYcUxcSQQgKhnQ1oid2w1XNvCKxmPgk7C8fEi4FnQKhHLwM
```

Create initial Identity Authority Account
---

* idauth1
```bash
$YOSEMITE_CLEOS create key
Private key: 5KND1U57MvsdWzPi8TY53XpaBPBGQNTdQSTDtEexTVexz9mP6Q9
Public key: EOS5gFJS9EhjNpvcaGuDQqaeGQnNLuF9sWUt2s8surF1tRtSzLJiG
$YOSEMITE_CLEOS wallet import --private-key 5KND1U57MvsdWzPi8TY53XpaBPBGQNTdQSTDtEexTVexz9mP6Q9

$YOSEMITE_CLEOS create account eosio idauth1 EOS5gFJS9EhjNpvcaGuDQqaeGQnNLuF9sWUt2s8surF1tRtSzLJiG

```

* idauth2
```bash
$YOSEMITE_CLEOS create key
Private key: 5K25HU8a7qgkfb9o2skiNupEs9H3wE6CWSd9qjako2gnFioekaX
Public key: EOS68JZEkVt4Z3dkrSmQ4SkH8XyVJVKTmnvawZyWcrxzhoH1WMp4Q
$YOSEMITE_CLEOS wallet import --private-key 5K25HU8a7qgkfb9o2skiNupEs9H3wE6CWSd9qjako2gnFioekaX

$YOSEMITE_CLEOS create account eosio idauth2 EOS68JZEkVt4Z3dkrSmQ4SkH8XyVJVKTmnvawZyWcrxzhoH1WMp4Q
```

Register Initial Identity Authority
---
```bash
$YOSEMITE_CLEOS push action eosio regidauth '["idauth1","http://idauth1.org",1]' -p idauth1@active -p eosio@active
$YOSEMITE_CLEOS push action eosio authidauth '["idauth1"]' -p eosio@active

$YOSEMITE_CLEOS push action eosio regidauth '["idauth2","http://idauth2.org",1]' -p idauth2@active -p eosio@active
$YOSEMITE_CLEOS push action eosio authidauth '["idauth2"]' -p eosio@active

//$YOSEMITE_CLEOS push action eosio rmvidauth '["idauth2"]' -p eosio@active
```

Querying information for the registered Identity Authorities
---

```bash
$YOSEMITE_CLEOS get table eosio eosio idauthority
{
  "rows": [{
      "owner": "idauth1",
      "is_authorized": 1,
      "url": "http://idauth1.org",
      "location": 1
    },{
      "owner": "idauth2",
      "is_authorized": 1,
      "url": "http://idauth2.org",
      "location": 1
    }
  ],
  "more": false
}
```

Register Initial System Depository
---
```bash
$YOSEMITE_CLEOS push action eosio regsysdepo '["sysdepo1","http://sysdepo1.org",1]' -p sysdepo1@active -p eosio@active
$YOSEMITE_CLEOS push action eosio authsysdepo '["sysdepo1"]' -p eosio@active

$YOSEMITE_CLEOS push action eosio regsysdepo '["sysdepo2","http://sysdepo2.org",1]' -p sysdepo2@active -p eosio@active
$YOSEMITE_CLEOS push action eosio authsysdepo '["sysdepo2"]' -p eosio@active
```

Querying information for the registered Identity Authorities
---

```bash
$YOSEMITE_CLEOS get table eosio eosio sysdepos
{
  "rows": [{
      "owner": "sysdepo1",
      "is_authorized": 1,
      "url": "http://sysdepo1.org",
      "location": 1
    },{
      "owner": "sysdepo2",
      "is_authorized": 1,
      "url": "http://sysdepo2.org",
      "location": 1
    }
  ],
  "more": false
}
```

Querying the global blockchain properties
---

```bash
$YOSEMITE_CLEOS get table eosio eosio global
{
  "rows": [{
      "max_block_net_usage": 1048576,
      "target_block_net_usage_pct": 1000,
      "max_transaction_net_usage": 524288,
      "base_per_transaction_net_usage": 12,
      "net_usage_leeway": 500,
      "context_free_discount_net_usage_num": 20,
      "context_free_discount_net_usage_den": 100,
      "max_block_cpu_usage": 200000,
      "target_block_cpu_usage_pct": 1000,
      "max_transaction_cpu_usage": 150000,
      "min_transaction_cpu_usage": 100,
      "max_transaction_lifetime": 3600,
      "deferred_trx_expiration_window": 600,
      "max_transaction_delay": 3888000,
      "max_inline_action_size": 4096,
      "max_inline_action_depth": 4,
      "max_authority_depth": 6,
      "max_ram_size": "68719476736",
      "total_ram_bytes_reserved": 0,
      "total_ram_stake": 0,
      "last_producer_schedule_update": "2018-07-31T07:30:53.500",
      "total_unpaid_blocks": 13498,
      "last_producer_schedule_size": 5,
      "total_producer_vote_weight": "0.00000000000000000"
    }
  ],
  "more": false
}
```

Permissioned Block Producer Setup
---

* config_yosemite_testnet.ini (one node multi producer setup)
```ini
producer-name = eosio
producer-name = producer.a
producer-name = producer.b
producer-name = producer.c
producer-name = producer.d
producer-name = producer.e

# producer.a
signature-provider = EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y=KEY:5JuTjNtFmqWG1ivMLJgeRgo5w2WV5iwHa2u1n8owvpyUdy7DUr6
# producer.b
signature-provider = EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS=KEY:5JbXQdi7vzRXtxnyBi9PSTzaxACJagDKUFQkLE7Dg9ZYMyykJR8
# producer.c
signature-provider = EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe=KEY:5JEJrZMbDSZmFfRUbt1JVC3wrBv9cJG7QaiP58f8Mr9K1Wf9Cnn
# producer.d
signature-provider = EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ=KEY:5HsDW4pNKWDA91WcBuNNXT8sL5soVhXLHqZU7ZZqJzJC1YghXjT
# producer.e
signature-provider = EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW=KEY:5JFbKCZray6WGQZtkkxqQ9mencwQn5qynjJ1tXJRtaUH7oNRGnP
```

* create block producer accounts
```bash
$YOSEMITE_CLEOS create key
Private key: 5JuTjNtFmqWG1ivMLJgeRgo5w2WV5iwHa2u1n8owvpyUdy7DUr6
Public key: EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y
$YOSEMITE_CLEOS wallet import --private-key 5JuTjNtFmqWG1ivMLJgeRgo5w2WV5iwHa2u1n8owvpyUdy7DUr6

$YOSEMITE_CLEOS create key
Private key: 5JbXQdi7vzRXtxnyBi9PSTzaxACJagDKUFQkLE7Dg9ZYMyykJR8
Public key: EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS
$YOSEMITE_CLEOS wallet import --private-key 5JbXQdi7vzRXtxnyBi9PSTzaxACJagDKUFQkLE7Dg9ZYMyykJR8

$YOSEMITE_CLEOS create key
Private key: 5JEJrZMbDSZmFfRUbt1JVC3wrBv9cJG7QaiP58f8Mr9K1Wf9Cnn
Public key: EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe
$YOSEMITE_CLEOS wallet import --private-key 5JEJrZMbDSZmFfRUbt1JVC3wrBv9cJG7QaiP58f8Mr9K1Wf9Cnn

$YOSEMITE_CLEOS create key
Private key: 5HsDW4pNKWDA91WcBuNNXT8sL5soVhXLHqZU7ZZqJzJC1YghXjT
Public key: EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ
$YOSEMITE_CLEOS wallet import --private-key 5HsDW4pNKWDA91WcBuNNXT8sL5soVhXLHqZU7ZZqJzJC1YghXjT

$YOSEMITE_CLEOS create key
Private key: 5JFbKCZray6WGQZtkkxqQ9mencwQn5qynjJ1tXJRtaUH7oNRGnP
Public key: EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW
$YOSEMITE_CLEOS wallet import --private-key 5JFbKCZray6WGQZtkkxqQ9mencwQn5qynjJ1tXJRtaUH7oNRGnP

$YOSEMITE_CLEOS create account eosio producer.a EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y -p eosio@active
$YOSEMITE_CLEOS create account eosio producer.b EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS -p eosio@active
$YOSEMITE_CLEOS create account eosio producer.c EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe -p eosio@active
$YOSEMITE_CLEOS create account eosio producer.d EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ -p eosio@active
$YOSEMITE_CLEOS create account eosio producer.e EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW -p eosio@active
```

* register and authorize block producers (permissioned)

```bash
$YOSEMITE_CLEOS push action eosio regproducer '["producer.a","EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y","http://producera.io",1]' -p producer.a@active -p eosio@active
$YOSEMITE_CLEOS push action eosio regproducer '["producer.b","EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS","http://producerb.io",1]' -p producer.b@active -p eosio@active

$YOSEMITE_CLEOS push action eosio authproducer '["producer.a"]' -p eosio@active
$YOSEMITE_CLEOS push action eosio authproducer '["producer.b"]' -p eosio@active

promoting proposed schedule (set in block 2704) to pending; current block: 2705 lib: 2704 schedule: {"version":1,"producers":[{"producer_name":"producer.a","block_signing_key":"EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y"},{"producer_name":"producer.b","block_signing_key":"EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS"}]} 


$YOSEMITE_CLEOS push action eosio regproducer '["producer.c","EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe","http://producerc.io",1]' -p producer.c@active -p eosio@active
$YOSEMITE_CLEOS push action eosio regproducer '["producer.d","EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ","http://producerd.io",1]' -p producer.d@active -p eosio@active
$YOSEMITE_CLEOS push action eosio regproducer '["producer.e","EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW","http://producere.io",1]' -p producer.e@active -p eosio@active

$YOSEMITE_CLEOS push action eosio authproducer '["producer.c"]' -p eosio@active
$YOSEMITE_CLEOS push action eosio authproducer '["producer.d"]' -p eosio@active
$YOSEMITE_CLEOS push action eosio authproducer '["producer.e"]' -p eosio@active

promoting proposed schedule (set in block 2946) to pending; current block: 2967 lib: 2954 schedule: {"version":2,"producers":[{"producer_name":"producer.a","block_signing_key":"EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y"},{"producer_name":"producer.b","block_signing_key":"EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS"},{"producer_name":"producer.c","block_signing_key":"EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe"},{"producer_name":"producer.d","block_signing_key":"EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ"},{"producer_name":"producer.e","block_signing_key":"EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW"}]} 
```

Resign "eosio" delegating authority to "eosio.prods"
---
```bash
$YOSEMITE_CLEOS get account eosio
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"eosio","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio.prods","permission":"active"}}]}}' -p eosio@owner
$YOSEMITE_CLEOS push action eosio updateauth '{"account":"eosio","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"eosio.prods","permission":"active"}}]}}' -p eosio@active
$YOSEMITE_CLEOS get account eosio

$YOSEMITE_CLEOS get account eosio.prods
permissions: 
     owner     1:    
        active     4:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
           prod.major     3:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
              prod.minor     2:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
```

Unauthorizing Block Producer
---
```bash
$YOSEMITE_CLEOS push action -j -d eosio rmvproducer '["producer.e"]' -p eosio@active
#$YOSEMITE_CLEOS push action eosio rmvproducer '["producer.e"]' -p eosio@active
#$YOSEMITE_CLEOS push action eosio rmvproducer '["producer.d"]' -p eosio@active
$YOSEMITE_CLEOS get account eosio.prods
```

Claim Producer Rewards
---
```bash
$YOSEMITE_CLEOS push action yx.identity setidinfo "{\"account\":\"producer.c\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"bp\"}" -p idauth1@active

$YOSEMITE_CLEOS push action eosio claimrewards '["producer.c"]' -p producer.c@active
executed transaction: 96672bbf34019be7f616ac1194af1ca2e68ffb0e332503155b6075cb964893c5  104 bytes  4045 us
#         eosio <= eosio::claimrewards          {"owner":"producer.c"}
#     yx.ntoken <= yx.ntoken::transfer          {"from":"yx.txfee","to":"producer.c","asset":"472.6714 DKRW","memo":""}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"yx.txfee","to":"producer.c","asset":{"asset":"472.6714 DKRW","issuer":"sysdepo1"},"memo":""...
#      yx.txfee <= yx.ntoken::ntransfer         {"from":"yx.txfee","to":"producer.c","asset":{"asset":"472.6714 DKRW","issuer":"sysdepo1"},"memo":""...
#    producer.c <= yx.ntoken::ntransfer         {"from":"yx.txfee","to":"producer.c","asset":{"asset":"472.6714 DKRW","issuer":"sysdepo1"},"memo":""...

$YOSEMITE_CLEOS get table eosio eosio producers

$YOSEMITE_CLEOS get table yx.ntoken yx.txfee ntaccounts
$YOSEMITE_CLEOS get table yx.ntoken producer.c ntaccounts

```

Create New User Accounts
---

```bash
$YOSEMITE_CLEOS create key
Private key: 5JsqBiEGge54oBRPEuPZ4rTm8EHxzDNzXMbwNoTh8y5Lms1YMJT
Public key: EOS7qw27KHaVWJEgA113u9xQVUtEubUKFhRJfiDt752iAqb6qbv9V
$YOSEMITE_CLEOS wallet import --private-key 5JsqBiEGge54oBRPEuPZ4rTm8EHxzDNzXMbwNoTh8y5Lms1YMJT

$YOSEMITE_CLEOS create key
Private key: 5JMsmWCrQ7YEDfUNhmPts7W3AuUFxeWpionyP8EuMf35kG3AeCU
Public key: EOS7hbLPy5cmtNBdHibrS8CD1YUsKbvq7t9nGVvnJtTKHvxsq6ynf
$YOSEMITE_CLEOS wallet import --private-key 5JMsmWCrQ7YEDfUNhmPts7W3AuUFxeWpionyP8EuMf35kG3AeCU

$YOSEMITE_CLEOS create key
Private key: 5K2iUzbSy4qyEzjf93DGDxBMrL8c8VGnSvLhE26DEJhEwvVEwj8
Public key: EOS75qaGrHrXskTUJLoedxwc7KbLxksfgiYBpgeiMcTCsnhiuhx6U
$YOSEMITE_CLEOS wallet import --private-key 5K2iUzbSy4qyEzjf93DGDxBMrL8c8VGnSvLhE26DEJhEwvVEwj8

$YOSEMITE_CLEOS create key
Private key: 5HrWpMMjCM1UhxKAvCg8NKRyExapwS1vyiQXe8VBpNEDsNMn4nE
Public key: EOS584bQn471vv2q7N1xeDXayfDtiim8kPsNm45TRuJ9D51YPFFmX
$YOSEMITE_CLEOS wallet import --private-key 5HrWpMMjCM1UhxKAvCg8NKRyExapwS1vyiQXe8VBpNEDsNMn4nE

$YOSEMITE_CLEOS create key
Private key: 5HsUEcT7J56iRrHFiGR9pGeQAiLaow4AbR7QA38w9T44SBnEMYk
Public key: EOS5UbAi7wTM1wpFNp81bghD9EfV1HHSn8n39Qz4jKD1oHmW7oyNS
$YOSEMITE_CLEOS wallet import --private-key 5HsUEcT7J56iRrHFiGR9pGeQAiLaow4AbR7QA38w9T44SBnEMYk

// Identity Authority can create accounts having 12 chracters without paying 'tf.newacc' transaction fee
$YOSEMITE_CLEOS create account idauth1 useraccount1 EOS7qw27KHaVWJEgA113u9xQVUtEubUKFhRJfiDt752iAqb6qbv9V -p idauth1@active
$YOSEMITE_CLEOS create account idauth1 useraccount2 EOS7hbLPy5cmtNBdHibrS8CD1YUsKbvq7t9nGVvnJtTKHvxsq6ynf -p idauth1@active
$YOSEMITE_CLEOS create account idauth1 useraccount3 EOS75qaGrHrXskTUJLoedxwc7KbLxksfgiYBpgeiMcTCsnhiuhx6U -p idauth1@active

// Only system account can create accounts having less than 12 characters
$YOSEMITE_CLEOS create account eosio com EOS584bQn471vv2q7N1xeDXayfDtiim8kPsNm45TRuJ9D51YPFFmX -p eosio@active

// Only suffix account can create accounts having arbitrary prefix with the same suffix name
$YOSEMITE_CLEOS push action yx.identity setidinfo "{\"account\":\"com\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"1f32i7t23\"}" -p idauth1@active
$YOSEMITE_CLEOS push action yx.ntoken nissue '["com",{"asset":"5000000.0000 DKRW","issuer":"sysdepo1"},"nissue com"]' -p sysdepo1@active
executed transaction: e2dc09299d5596fa957da04a3ac7a33d4f4d1c5046ddf966638787eab078926a  136 bytes  3845 us
#     yx.ntoken <= yx.ntoken::nissue            {"to":"com","asset":{"asset":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"nissue com"}
#     yx.ntoken <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","asset":{"asset":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"niss...
#      sysdepo1 <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","asset":{"asset":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"niss...
#           com <= yx.ntoken::ntransfer         {"from":"sysdepo1","to":"com","asset":{"asset":"5000000.0000 DKRW","issuer":"sysdepo1"},"memo":"niss...
$YOSEMITE_CLEOS create account com acquire.com EOS5UbAi7wTM1wpFNp81bghD9EfV1HHSn8n39Qz4jKD1oHmW7oyNS -p com@active

$YOSEMITE_CLEOS get table yx.ntoken sysdepo1 ntaccounts
$YOSEMITE_CLEOS get table yx.ntoken com ntaccounts
$YOSEMITE_CLEOS get table yx.ntoken yx.txfee ntaccounts

$YOSEMITE_CLEOS get table -L com -l 1 yx.identity yx.identity identity
``` 

Making Privileged Account
---

```bash
$YOSEMITE_CLEOS push action eosio setpriv '["yx.ntoken",1]' -p eosio@active
```
