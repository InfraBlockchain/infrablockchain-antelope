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

#### cloning yosemite-public-blockchain
```console
git clone https://github.com/YosemiteLabs/yosemite-public-blockchain
git submodule update --init --recursive
#git checkout tags/v1.0.2
git checkout yosemite-master
./eosio_build.sh
```