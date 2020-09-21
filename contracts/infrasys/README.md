INFRABLOCKCHAIN System Contract 
===

* block producer management and election of block producers (currently permissioned setup)
* claiming block producer rewards from transaction fee profit pool
* account management (newaccount, updateauth, deleteauth, linkauth, unlinkauth, setpriv)
* managing the registration of System Depositories having right for issuing native fiat-stable token
* managing the registration of Identity Authroities, the authorized Identity Authorities have the right for managing user info and KYC info on blockchain.

Environment Var.
---

```bash
INFRA_CLI=$INFRABLOCKCHAIN_HOME/build/programs/infra-cli/infra-cli
INFRABLOCKCHAIN_CONTRACTS_DIR=$INFRABLOCKCHAIN_HOME/build/contracts
```

Install INFRABLOCKCHAIN System Contract
---
```bash
$INFRA_CLI set contract infrasys $INFRABLOCKCHAIN_CONTRACTS_DIR/infrasys/ -p infrasys@active
```

Create initial System Depository Account
---

* sysdepo1
```bash
$INFRA_CLI create key
Private key: 5KCqedPhHjbaBRi4ujhFsS3aGCcNuXwVGrMaxRW3DvB5Wo3beax
Public key: EOS7myk8qduMNnxo9Q9RZ2uxfaerQhXMPtxKUhxKh5HfSSM1dHeQo
$INFRA_CLI wallet import --private-key 5KCqedPhHjbaBRi4ujhFsS3aGCcNuXwVGrMaxRW3DvB5Wo3beax

$INFRA_CLI create account infrasys sysdepo1 EOS7myk8qduMNnxo9Q9RZ2uxfaerQhXMPtxKUhxKh5HfSSM1dHeQo

```

* sysdepo2
```bash
$INFRA_CLI create key
Private key: 5HyRsRWBi4bUXjmnmaskw9BU8a2sdqn3Jfqt21Few9xUyYR5tXr
Public key: EOS62AXYcUxcSQQgKhnQ1oid2w1XNvCKxmPgk7C8fEi4FnQKhHLwM
$INFRA_CLI wallet import --private-key 5HyRsRWBi4bUXjmnmaskw9BU8a2sdqn3Jfqt21Few9xUyYR5tXr

$INFRA_CLI create account infrasys sysdepo2 EOS62AXYcUxcSQQgKhnQ1oid2w1XNvCKxmPgk7C8fEi4FnQKhHLwM
```

Create initial Identity Authority Account
---

* idauth1
```bash
$INFRA_CLI create key
Private key: 5KND1U57MvsdWzPi8TY53XpaBPBGQNTdQSTDtEexTVexz9mP6Q9
Public key: EOS5gFJS9EhjNpvcaGuDQqaeGQnNLuF9sWUt2s8surF1tRtSzLJiG
$INFRA_CLI wallet import --private-key 5KND1U57MvsdWzPi8TY53XpaBPBGQNTdQSTDtEexTVexz9mP6Q9

$INFRA_CLI create account infrasys idauth1 EOS5gFJS9EhjNpvcaGuDQqaeGQnNLuF9sWUt2s8surF1tRtSzLJiG

```

* idauth2
```bash
$INFRA_CLI create key
Private key: 5K25HU8a7qgkfb9o2skiNupEs9H3wE6CWSd9qjako2gnFioekaX
Public key: EOS68JZEkVt4Z3dkrSmQ4SkH8XyVJVKTmnvawZyWcrxzhoH1WMp4Q
$INFRA_CLI wallet import --private-key 5K25HU8a7qgkfb9o2skiNupEs9H3wE6CWSd9qjako2gnFioekaX

$INFRA_CLI create account infrasys idauth2 EOS68JZEkVt4Z3dkrSmQ4SkH8XyVJVKTmnvawZyWcrxzhoH1WMp4Q
```

Register Initial Identity Authority
---
```bash
$INFRA_CLI push action infrasys regidauth '["idauth1","http://idauth1.org",1]' -p idauth1@active -p infrasys@active
$INFRA_CLI push action infrasys authidauth '["idauth1"]' -p infrasys@active

$INFRA_CLI push action infrasys regidauth '["idauth2","http://idauth2.org",1]' -p idauth2@active -p infrasys@active
$INFRA_CLI push action infrasys authidauth '["idauth2"]' -p infrasys@active

//$INFRA_CLI push action infrasys rmvidauth '["idauth2"]' -p infrasys@active
```

Querying information for the registered Identity Authorities
---

```bash
$INFRA_CLI get table infrasys infrasys idauthority
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
$INFRA_CLI push action infrasys regsysdepo '["sysdepo1","http://sysdepo1.org",1]' -p sysdepo1@active -p infrasys@active
$INFRA_CLI push action infrasys authsysdepo '["sysdepo1"]' -p infrasys@active

$INFRA_CLI push action infrasys regsysdepo '["sysdepo2","http://sysdepo2.org",1]' -p sysdepo2@active -p infrasys@active
$INFRA_CLI push action infrasys authsysdepo '["sysdepo2"]' -p infrasys@active
```

Querying information for the registered Identity Authorities
---

```bash
$INFRA_CLI get table infrasys infrasys sysdepos
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
$INFRA_CLI get table infrasys infrasys global
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

* config_infrablockchain_testnet.ini (one node multi producer setup)
```ini
producer-name = infrasys
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
$INFRA_CLI create key
Private key: 5JuTjNtFmqWG1ivMLJgeRgo5w2WV5iwHa2u1n8owvpyUdy7DUr6
Public key: EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y
$INFRA_CLI wallet import --private-key 5JuTjNtFmqWG1ivMLJgeRgo5w2WV5iwHa2u1n8owvpyUdy7DUr6

$INFRA_CLI create key
Private key: 5JbXQdi7vzRXtxnyBi9PSTzaxACJagDKUFQkLE7Dg9ZYMyykJR8
Public key: EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS
$INFRA_CLI wallet import --private-key 5JbXQdi7vzRXtxnyBi9PSTzaxACJagDKUFQkLE7Dg9ZYMyykJR8

$INFRA_CLI create key
Private key: 5JEJrZMbDSZmFfRUbt1JVC3wrBv9cJG7QaiP58f8Mr9K1Wf9Cnn
Public key: EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe
$INFRA_CLI wallet import --private-key 5JEJrZMbDSZmFfRUbt1JVC3wrBv9cJG7QaiP58f8Mr9K1Wf9Cnn

$INFRA_CLI create key
Private key: 5HsDW4pNKWDA91WcBuNNXT8sL5soVhXLHqZU7ZZqJzJC1YghXjT
Public key: EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ
$INFRA_CLI wallet import --private-key 5HsDW4pNKWDA91WcBuNNXT8sL5soVhXLHqZU7ZZqJzJC1YghXjT

$INFRA_CLI create key
Private key: 5JFbKCZray6WGQZtkkxqQ9mencwQn5qynjJ1tXJRtaUH7oNRGnP
Public key: EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW
$INFRA_CLI wallet import --private-key 5JFbKCZray6WGQZtkkxqQ9mencwQn5qynjJ1tXJRtaUH7oNRGnP

$INFRA_CLI create account infrasys producer.a EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y -p infrasys@active
$INFRA_CLI create account infrasys producer.b EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS -p infrasys@active
$INFRA_CLI create account infrasys producer.c EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe -p infrasys@active
$INFRA_CLI create account infrasys producer.d EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ -p infrasys@active
$INFRA_CLI create account infrasys producer.e EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW -p infrasys@active
```

* register and authorize block producers (permissioned)

```bash
$INFRA_CLI push action infrasys regproducer '["producer.a","EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y","http://producera.io",1]' -p producer.a@active -p infrasys@active
$INFRA_CLI push action infrasys regproducer '["producer.b","EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS","http://producerb.io",1]' -p producer.b@active -p infrasys@active

$INFRA_CLI push action infrasys authproducer '["producer.a"]' -p infrasys@active
$INFRA_CLI push action infrasys authproducer '["producer.b"]' -p infrasys@active

promoting proposed schedule (set in block 2704) to pending; current block: 2705 lib: 2704 schedule: {"version":1,"producers":[{"producer_name":"producer.a","block_signing_key":"EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y"},{"producer_name":"producer.b","block_signing_key":"EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS"}]} 


$INFRA_CLI push action infrasys regproducer '["producer.c","EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe","http://producerc.io",1]' -p producer.c@active -p infrasys@active
$INFRA_CLI push action infrasys regproducer '["producer.d","EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ","http://producerd.io",1]' -p producer.d@active -p infrasys@active
$INFRA_CLI push action infrasys regproducer '["producer.e","EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW","http://producere.io",1]' -p producer.e@active -p infrasys@active

$INFRA_CLI push action infrasys authproducer '["producer.c"]' -p infrasys@active
$INFRA_CLI push action infrasys authproducer '["producer.d"]' -p infrasys@active
$INFRA_CLI push action infrasys authproducer '["producer.e"]' -p infrasys@active

promoting proposed schedule (set in block 2946) to pending; current block: 2967 lib: 2954 schedule: {"version":2,"producers":[{"producer_name":"producer.a","block_signing_key":"EOS6WZQdBdvfYre8akxVqCgp8SVYjrJRBnLqqpYGe7AXyJTTPYf4y"},{"producer_name":"producer.b","block_signing_key":"EOS79oucvH1iBTh5RrMgm6vRe9ERNSPQx4AiXGVctjmZQMkCkFrzS"},{"producer_name":"producer.c","block_signing_key":"EOS6FTViN9RtWd7B7RbERxcdhWSh2jmqiLkhMck315uSUwCsjeNHe"},{"producer_name":"producer.d","block_signing_key":"EOS66ABHVyT25wYUd2suW6JtbrwLrUdPFVDA1vjU8ttU289d2DPvJ"},{"producer_name":"producer.e","block_signing_key":"EOS8Ab5kDP22ta5FRmShTsZ4SZ3ubwptxzKWSBw7FtZLsew12SfrW"}]} 
```

Resign "infrasys" delegating authority to "sys.prods"
---
```bash
$INFRA_CLI get account infrasys
$INFRA_CLI push action infrasys updateauth '{"account":"infrasys","permission":"owner","parent":"","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"sys.prods","permission":"active"}}]}}' -p infrasys@owner
$INFRA_CLI push action infrasys updateauth '{"account":"infrasys","permission":"active","parent":"owner","auth":{"threshold":1,"keys":[],"waits":[],"accounts":[{"weight":1,"permission":{"actor":"sys.prods","permission":"active"}}]}}' -p infrasys@active
$INFRA_CLI get account infrasys

$INFRA_CLI get account sys.prods
permissions: 
     owner     1:    
        active     4:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
           prod.major     3:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
              prod.minor     2:    1 producer.a@active, 1 producer.b@active, 1 producer.c@active, 1 producer.d@active, 1 producer.e@active, 
```

Unauthorizing Block Producer
---
```bash
$INFRA_CLI push action -j -d infrasys rmvproducer '["producer.e"]' -p infrasys@active
#$INFRA_CLI push action infrasys rmvproducer '["producer.e"]' -p infrasys@active
#$INFRA_CLI push action infrasys rmvproducer '["producer.d"]' -p infrasys@active
$INFRA_CLI get account sys.prods
```

Claim Producer Rewards
---
```bash
$INFRA_CLI push action sys.identity setidinfo "{\"account\":\"producer.c\", \"identity_authority\":\"idauth1\", \"type\":$(echo 'ibase=2; 0' | bc), \"kyc\":$(echo 'ibase=2; 1111' | bc), \"state\":$(echo 'ibase=2; 0' | bc), \"data\":\"bp\"}" -p idauth1@active

$INFRA_CLI push action infrasys claimrewards '["producer.c"]' -p producer.c@active

$INFRA_CLI get table eosio infrasys producers

```

Create New User Accounts
---

```bash
$INFRA_CLI create key
Private key: 5JsqBiEGge54oBRPEuPZ4rTm8EHxzDNzXMbwNoTh8y5Lms1YMJT
Public key: EOS7qw27KHaVWJEgA113u9xQVUtEubUKFhRJfiDt752iAqb6qbv9V
$INFRA_CLI wallet import --private-key 5JsqBiEGge54oBRPEuPZ4rTm8EHxzDNzXMbwNoTh8y5Lms1YMJT

$INFRA_CLI create key
Private key: 5JMsmWCrQ7YEDfUNhmPts7W3AuUFxeWpionyP8EuMf35kG3AeCU
Public key: EOS7hbLPy5cmtNBdHibrS8CD1YUsKbvq7t9nGVvnJtTKHvxsq6ynf
$INFRA_CLI wallet import --private-key 5JMsmWCrQ7YEDfUNhmPts7W3AuUFxeWpionyP8EuMf35kG3AeCU

$INFRA_CLI create key
Private key: 5K2iUzbSy4qyEzjf93DGDxBMrL8c8VGnSvLhE26DEJhEwvVEwj8
Public key: EOS75qaGrHrXskTUJLoedxwc7KbLxksfgiYBpgeiMcTCsnhiuhx6U
$INFRA_CLI wallet import --private-key 5K2iUzbSy4qyEzjf93DGDxBMrL8c8VGnSvLhE26DEJhEwvVEwj8

$INFRA_CLI create key
Private key: 5HrWpMMjCM1UhxKAvCg8NKRyExapwS1vyiQXe8VBpNEDsNMn4nE
Public key: EOS584bQn471vv2q7N1xeDXayfDtiim8kPsNm45TRuJ9D51YPFFmX
$INFRA_CLI wallet import --private-key 5HrWpMMjCM1UhxKAvCg8NKRyExapwS1vyiQXe8VBpNEDsNMn4nE

$INFRA_CLI create key
Private key: 5HsUEcT7J56iRrHFiGR9pGeQAiLaow4AbR7QA38w9T44SBnEMYk
Public key: EOS5UbAi7wTM1wpFNp81bghD9EfV1HHSn8n39Qz4jKD1oHmW7oyNS
$INFRA_CLI wallet import --private-key 5HsUEcT7J56iRrHFiGR9pGeQAiLaow4AbR7QA38w9T44SBnEMYk

// Identity Authority can create accounts having 12 chracters without paying 'tf.newacc' transaction fee
$INFRA_CLI create account idauth1 useraccount1 EOS7qw27KHaVWJEgA113u9xQVUtEubUKFhRJfiDt752iAqb6qbv9V -p idauth1@active
$INFRA_CLI create account idauth1 useraccount2 EOS7hbLPy5cmtNBdHibrS8CD1YUsKbvq7t9nGVvnJtTKHvxsq6ynf -p idauth1@active
$INFRA_CLI create account idauth1 useraccount3 EOS75qaGrHrXskTUJLoedxwc7KbLxksfgiYBpgeiMcTCsnhiuhx6U -p idauth1@active

// Only system account can create accounts having less than 12 characters
$INFRA_CLI create account infrasys com EOS584bQn471vv2q7N1xeDXayfDtiim8kPsNm45TRuJ9D51YPFFmX -p infrasys@active

$INFRA_CLI get table -L com -l 1 sys.identity sys.identity identity
``` 

Making Privileged Account
---

```bash
$INFRA_CLI push action infrasys setpriv '["sys.service",1]' -p infrasys@active
```
