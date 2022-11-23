
# InfraBlockchain

* [InfraBlockchain Website - https://infrablockchain.com](https://infrablockchain.com)
* [Technical White Paper Version 2.4 - InfraBlockchain_Technical_White_Paper_Version_2_4_ENG_202008.pdf](https://infrablockchain.net/documents/InfraBlockchain_Technical_White_Paper_Version_2_4_ENG_202008.pdf)

### No Native Cryptocurrency
Using programmable fiat-pegged stable tokens (e.g. USD, EUR, CNY) as base currencies

### Proof-of-Transaction (PoT)-based Blockchain Consensus
To overcome the shortcomings of current public blockchains, we developed a more reasonable and fair consensus algorithm, Proof-of-Transaction (PoT)

### General Financial Platform
Smart contract based blockchain system designed for enterprise uses


## Key Features of InfraBlockchain

#### No Native Cryptocurrency
New method of public blockchain design that avoids issuing native cryptocurrency minted by the blockchain itself.
There is no single distinguished built-in native token, only user-created tokens are issued and circulated on the InfraBlockchain.
In other smart contract based blockchains like Ethereum and EOS, custom tokens implemented as smart contracts are totally separate from the native cryptocurrency (ETH, EOS) used for executing blockchain transactions.
In contrast, InfraBlockchain lets any custom user token conforming to the InfraBlockchain Standard Token interface become a Transaction Fee Token just as a native cryptocurrency can in other blockchain systems.

#### InfraBlockchain Standard Token Model
The InfraBlockchain provides a unique InfraBlockchain Standard Token model.
Every account created can mint a InfraBlockchain Standard Token and inherently run standard token operations natively supported at the blockchain core level.
Interoperability among the tokens on the InfraBlockchain is enforced and guaranteed by the blockchain core system.

#### Fiat-Pegged Stable Tokens, Asset-Backed Security Tokens
User-created fiat-pegged stable tokens (e.g. USD tokens backed by an equivalent amount of USD held in reserve)
issued by trusted entities (e.g. enterprises, financial institutions, governments) are selected as the transaction fee tokens.
Asset-backed security tokens (e.g. company shares, gold backed tokens, ...) can be also issued and traded through on-chain token exchange
using fiat-pegged stable tokens as base currencies.

#### Transaction Fee Tokens Selected from User-Created Tokens
Among the user-issued tokens, the elected block producers can select some tokens to be used for blockchain transaction fee payment, whereas typically a pre-built native cryptocurrency is used in other blockchain systems.
Whenever any blockchain transaction is executed on the InfraBlockchain, the block-producer-designated transaction fee tokens are collected as a transaction fee from users.

#### Proof-of-Transaction (PoT) Consensus Mechanism
The block producers of the InfraBlockchain are elected by a unique consensus mechanism: Proof-of-Transaction (PoT) using Transaction-as-a-Vote (TaaV).
The block producers are continuously elected and evicted by the transparent and fair vote index, the proof of generating meaningful transactions. The PoT mechanism incentivizes the application service providers,
who are directly contributing to the blockchain ecosystem by generating blockchain transactions associated with real economic activity.

#### Delegable Transaction Fee Payment
The transaction fee payer is always specified separately from the accounts directly involved in a transaction, the transaction fee payment can be delegated to some other account.
For the sake of a seamless and convenient user experience, blockchain service providers can pay blockchain transaction fees on behalf of the service’s users.

#### Smart Contract Enabled General Financial Platform
The InfraBlockchain is designed to be a general financial platform supporting smart contracts.
Business entities can process their financial transactions on the InfraBlockchain using the fiat-pegged stable tokens
and custom smart contract codes developed to meet the needs of their business process.

#### Integrated KYC/AML Support and Account Recovery
The integrated KYC/AML process for InfraBlockchain accounts is provided by trusted Identity Authorities designated by block producers,
which lays the foundation for the secure and reliable financial transactions maintaining regulatory compliance. The blockchain account recovery is provided by the Identity Authorities mitigating the blockchain key loss problem.

#### Scalable Blockchain Architecture
The enterprise level blockchain scalability enabled by the PoT-based BFT consensus with short block time and fast block finality,
the InfraBlockchain’s off-chain state-channel exchange technology and the future multi-blockchain architecture.
The InfraBlockchain enables financial transaction processing at global scale.

InfraBlockchain code base is forked from [EOSIO](https://github.com/EOSIO/eos)


## InfraBlockchain 'Sentinel' Test-net
* [http://testnet-sentinel-explorer.yosemitelabs.org](http://testnet-sentinel-explorer.yosemitelabs.org)


## InfraBlockchain Explorer
1. [InfraBlockchain Explorer Backend](https://github.com/InfraBlockchain/yosemite-explorer-backend)
1. [InfraBlockchain Explorer Web Frontend](https://github.com/InfraBlockchain/yosemite-explorer-web)


## Client Library
1. [Java Client Library](https://github.com/InfraBlockchain/yosemite-j)


## Supported Operating Systems

InfraBlockchain currently supports the following operating systems:  

1. Amazon Linux 2
2. CentOS 7
3. Ubuntu 16.04
4. Ubuntu 18.04
5. MacOS 10.14 (Mojave)

---

**Note: It may be possible to install InfraBlockchain on other Unix-based operating systems. This is not officially supported, though.**

---

## Build Script
To build the InfraBlockchain, run:
```sh
./scripts/infrablockchain_build.sh
```

## Install Script
To install the InfraBlockchain, run:
```sh
./scripts/infrablockchain_install.sh
```

## Uninstall Script
To uninstall the InfraBlockchain built/installed binaries and dependencies, run:
```sh
./scripts/infrablockchain_uninstall.sh
```


## License

InfraBlockchain is released under the open source MIT license and is offered “AS IS” without warranty of any kind, express or implied.
Any security provided by the InfraBlockchain software depends in part on how it is used, configured, and deployed.
InfraBlockchain is built upon many third-party libraries such as Binaryen (Apache License) and WAVM (BSD 3-clause) which are also provided “AS IS” without warranty of any kind.
Without limiting the generality of the foregoing, Blockchain Labs Inc. makes no representation or guarantee that InfraBlockchain or any third-party libraries will perform as intended or will be free of errors,
bugs or faulty code. Both may fail in large or small ways that could completely or partially limit functionality or compromise computer systems.
If you use or implement InfraBlockchain, you do so at your own risk. In no event will Blockchain Labs Inc. be liable to any party for any damages whatsoever,
even if it had been advised of the possibility of damage.

## Important

See [LICENSE](./LICENSE) for copyright and license terms.

