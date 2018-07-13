#### Proof-of-Transaction(PoT) Consensus based YOSEMITE Public Blockchain 
* https://yosemitex.com

### EOS.IO Mirror
https://help.github.com/articles/duplicating-a-repository/

#### Mirroring EOSIO/eos to YosemiteLabs/yosemite-public-blockchain 
* excluding pull-request refs
  - https://stackoverflow.com/questions/37985275/how-can-i-exclude-pull-requests-from-git-mirror-clone

```console
git clone --mirror https://github.com/EOSIO/eos.git yosemite-public-blockchain-git-mirror
cd yosemite-public-blockchain-git-mirror
git remote set-url --push origin https://github.com/YosemiteLabs/yosemite-public-blockchain

git fetch -p origin
git show-ref | cut -d' ' -f2 | grep 'pull' | xargs -L1 git update-ref -d
git push --mirror
```

#### Tagging EOSIO/eos Release Version After Mirroring
* You must do this on your clone of yosemite-public-blockchain. Pull and rebase yosemite-master branch to the latest.
* For example, if release version is 1.0.10
```console
git tag yosemite_eos_v1.0.10
git push origin yosemite_eos_v1.0.10
```

#### cloning yosemite-public-blockchain
```console
git clone https://github.com/YosemiteLabs/yosemite-public-blockchain
cd yosemite-public-blockchain
git submodule update --init --recursive
git checkout yosemite-master
./eosio_build.sh
```
