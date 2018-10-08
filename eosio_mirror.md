# Mirroring EOSIO/eos to YosemiteLabs/yosemite-public-blockchain 
https://github.com/YosemiteLabs/yosemite-public-blockchain/issues/94

We can simply mirror EOSIO by pushing from EOSIO/eos:master branch to YosemiteLabs/yosemite-public-blockchain:eos-master branch. Don't forget our branch for EOSIO mirroring is <b>eosio-master</b>.

```console
git clone https://github.com/EOSIO/eos
cd eos
git push https://github.com/YosemiteLabs/yosemite-public-blockchain master:eosio-master
```

# Tagging EOSIO/eos Release Version After Mirroring
* You must do this on your repository of yosemite-public-blockchain. Pull and rebase yosemite-master branch to the latest.
* For example, if release version is 1.0.10

```console
git pull --rebase
git tag eos_v1.0.10_merge
git push origin eos_v1.0.10_merge
```
