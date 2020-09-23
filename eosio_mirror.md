# Mirroring EOSIO/eos to InfraBlockchain/yosemite-public-blockchain 
https://github.com/InfraBlockchain/yosemite-public-blockchain/issues/94

We can simply mirror EOSIO by pushing from EOSIO/eos:master branch to InfraBlockchain/infrablockchain:eos-master branch. Don't forget our branch for EOSIO mirroring is <b>eosio-master</b>.

```console
git clone https://github.com/EOSIO/eos
cd eos
#git push https://github.com/InfraBlockchain/infrablockchain master:eosio-master

git remote add infrablockchain https://github.com/InfraBlockchain/infrablockchain
#git fetch
git checkout origin/master -b eosio-master
git push infrablockchain eosio-master
```

# Tagging EOSIO/eos Release Version After Mirroring
* You must do this on your repository of infrablockchain. Pull and rebase master branch to the latest.
* For example, if release version is 1.0.10

```console
git pull --rebase
git tag eos_v1.0.10_merge
git push origin eos_v1.0.10_merge
```
