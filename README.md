# YOSEMITE Public Blockchain - General Purpose Public Blockchain for Business

* [YOSEMITE Public Blockchain Website - https://yosemitex.com](https://yosemitex.com)
* [Technical White Paper Version 2 - YOSEMITE_Public_Blockchain_Technical_White_Paper_Version_2_ENG_201901a.pdf](https://yosemitex.com/documents/YOSEMITE_Public_Blockchain_Technical_White_Paper_Version_2_ENG_201901a.pdf)


### No Native Cryptocurrency
Using programmable fiat-pegged stable tokens (e.g. USD, EUR, CNY) as base currencies

### Proof-of-Transaction (PoT)-based Blockchain Consensus
To overcome the shortcomings of current public blockchains, we developed a more reasonable and fair consensus algorithm, Proof-of-Transaction (PoT)

### General Financial Platform
Smart contract based blockchain system designed for enterprise uses


## Key Features of YOSEMITE Public Blockchain

#### No Native Cryptocurrency
 New method of public blockchain design that avoids issuing native cryptocurrency minted by the blockchain itself. 
 There is no single distinguished built-in native token, only user-created tokens are issued and circulated on the YOSEMITE blockchain. 
 In other smart contract based blockchains like Ethereum and EOS, custom tokens implemented as smart contracts are totally separate from the native cryptocurrency (ETH, EOS) used for executing blockchain transactions. 
 In contrast, YOSEMITE blockchain lets any custom user token conforming to the YOSEMITE Standard Token interface become a Transaction Fee Token just as a native cryptocurrency can in other blockchain systems.

#### YOSEMITE Standard Token Model
 The YOSEMITE blockchain provides a unique YOSEMITE Standard Token model. 
 Every account created can mint a YOSEMITE Standard Token and inherently run standard token operations natively supported at the blockchain core level. 
 Interoperability among the tokens on the YOSEMITE blockchain is enforced and guaranteed by the blockchain core system.

#### Fiat-Pegged Stable Tokens, Asset-Backed Security Tokens
 User-created fiat-pegged stable tokens (e.g. USD tokens backed by an equivalent amount of USD held in reserve) 
 issued by trusted entities (e.g. enterprises, financial institutions, governments) are selected as the transaction fee tokens. 
 Asset-backed security tokens (e.g. company shares, gold backed tokens, ...) can be also issued and traded through on-chain token exchange 
 using fiat-pegged stable tokens as base currencies.

#### Transaction Fee Tokens Selected from User-Created Tokens
 Among the user-issued tokens, the elected block producers can select some tokens to be used for blockchain transaction fee payment, whereas typically a pre-built native cryptocurrency is used in other blockchain systems. 
 Whenever any blockchain transaction is executed on the YOSEMITE blockchain, the block-producer-designated transaction fee tokens are collected as a transaction fee from users.

#### Proof-of-Transaction (PoT) Consensus Mechanism
 The block producers of the YOSEMITE blockchain are elected by a unique consensus mechanism designed by YOSEMITE: Proof-of-Transaction (PoT) using Transaction-as-a-Vote (TaaV). 
 The block producers are continuously elected and evicted by the transparent and fair vote index, the proof of generating meaningful transactions. The PoT mechanism incentivizes the application service providers, 
 who are directly contributing to the blockchain ecosystem by generating blockchain transactions associated with real economic activity.
 
#### Delegable Transaction Fee Payment
 The transaction fee payer is always specified separately from the accounts directly involved in a transaction, the transaction fee payment can be delegated to some other account. 
 For the sake of a seamless and convenient user experience, blockchain service providers can pay blockchain transaction fees on behalf of the service’s users.
 
#### Smart Contract Enabled General Financial Platform
 The YOSEMITE public blockchain is designed to be a general financial platform supporting smart contracts. 
 Business entities can process their financial transactions on the YOSEMITE blockchain using the fiat-pegged stable tokens 
 and custom smart contract codes developed to meet the needs of their business process.
 
#### Integrated KYC/AML Support and Account Recovery
 The integrated KYC/AML process for YOSEMITE blockchain accounts is provided by trusted Identity Authorities designated by block producers, 
 which lays the foundation for the secure and reliable financial transactions maintaining regulatory compliance. The blockchain account recovery is provided by the Identity Authorities mitigating the blockchain key loss problem.

#### Scalable Blockchain Architecture
 The enterprise level blockchain scalability enabled by the PoT-based BFT consensus with short block time and fast block finality, 
 the YOSEMITE’s off-chain state-channel exchange technology and the future multi-blockchain architecture. 
 The YOSEMITE blockchain enables financial transaction processing at global scale.

YOSEMITE Public Blockchain code base is forked from [EOSIO](https://github.com/EOSIO/eos)

YOSEMITE Public Blockchain is released under the open source MIT license and is offered “AS IS” without warranty of any kind, express or implied. 
Any security provided by the YOSEMITE Public Blockchain software depends in part on how it is used, configured, and deployed.
YOSEMITE Public Blockchain is built upon many third-party libraries such as Binaryen (Apache License) and WAVM (BSD 3-clause) which are also provided “AS IS” without warranty of any kind. 
Without limiting the generality of the foregoing, Yosemite X Inc. makes no representation or guarantee that YOSEMITE Public Blockchain or any third-party libraries will perform as intended or will be free of errors, 
bugs or faulty code. Both may fail in large or small ways that could completely or partially limit functionality or compromise computer systems. 
If you use or implement YOSEMITE, you do so at your own risk. In no event will Yosemite X Inc. be liable to any party for any damages whatsoever, 
even if it had been advised of the possibility of damage.  


## YOSEMITE Public Blockchain 'Sentinel' Test-net
* [http://testnet-sentinel-explorer.yosemitelabs.org](http://testnet-sentinel-explorer.yosemitelabs.org)


## YOSEMITE Blockchain Explorer
1. [YOSEMITE Blockchain Explorer Backend](https://github.com/YosemiteLabs/yosemite-explorer-backend)
1. [YOSEMITE Blockchain Explorer Web Frontend](https://github.com/YosemiteLabs/yosemite-explorer-web)


## Client Library
1. [Java Client Library](https://github.com/YosemiteLabs/yosemite-j)


## Supported Operating Systems
1. Centos 7
2. Ubuntu 18.04
3. MacOS 10.13 and higher (MacOS 10.14.x recommended)



## Resources





<a name="gettingstarted"></a>
## Getting Started
YOSEMITE is based on [EOSIO software](https://github.com/EOSIO/eos). So instructions detailing the process of getting the software, building it, running a simple test network that produces blocks, account creation to the blockchain can be helped by [Getting Started](https://developers.eos.io/eosio-nodeos/docs/overview-1) on the [EOSIO Developer Portal](https://developers.eos.io).
But the names of programs and scripts are changed.
```
nodeos -> yosemite
cleos  -> infra-cli
keosd  -> keyos (key + 'yos'emite)
```
### Client Tool
infra-cli : https://developers.eos.io/eosio-nodeos/docs/cleos-overview

### Key Store Daemon
keyos : https://developers.eos.io/eosio-nodeos/docs/keosd-overview

## Build and Install
To build YOSEMITE, use `yosemite_build.sh`. To install and unsintall, use `yosemite_install.sh` and `yosemite_uninstall.sh`.
