# YOSEMITE - Proof-of-Transaction(PoT) Consensus based Public Blockchain

<!-- [![Build status](https://badge.buildkite.com/370fe5c79410f7d695e4e34c500b4e86e3ac021c6b1f739e20.svg?branch=master)](https://buildkite.com/EOSIO/eosio) -->

Using a volatile native crypto-currency causes a great hindrance in running a business. YOSEMITE resolves that problem by using tokens that have their values pegged to government-approved fiat money. Those fiat-pegged stable coins (e.g. USD-pegged dUSD token) are issued by trust entities (Depositories) as the underlying token of the blockchain. We call it native token.

YOSEMITE's unique Proof-of-Transaction(PoT) consensus algorithm, which allows only the direct contributors to the blockchain to create blocks and get rewarded, can address the fundamental problems in PoW, PoS consensus algorithms and sustain a fair and reasonable operation.

Finally, governments and companies globally have a capable underlying system to utilize the advantages of blockchain and conduct sustainable policies and businesses.

YOSEMITE is powered by EOSIO software to get low latency block confirmation and BFT finality. Other than EOSIO features, some of the novel features of YOSEMITE include:

1. Uses real money (e.g. USD, EUR, CNY) as the native token for transaction fee : [README](contracts/yx.ntoken/README.md)
1. KYC/AML Compliant Blockchain
1. Proof-of-Transaction & Transaction-as-a-Vote (will be provided)
1. Embedded Decentralized Exchange (will be provided)
1. Smart contract platform powered by Web Assembly (will be provided)

YOSEMITE is released under the open source MIT license and is offered “AS IS” without warranty of any kind, express or implied. Any security provided by the YOSEMITE software depends in part on how it is used, configured, and deployed. YOSEMITE is built upon many third-party libraries such as Binaryen (Apache License) and WAVM (BSD 3-clause) which are also provided “AS IS” without warranty of any kind. Without limiting the generality of the foregoing, Yosemite X Inc. makes no representation or guarantee that YOSEMITE or any third-party libraries will perform as intended or will be free of errors, bugs or faulty code. Both may fail in large or small ways that could completely or partially limit functionality or compromise computer systems. If you use or implement YOSEMITE, you do so at your own risk. In no event will Yosemite X Inc. be liable to any party for any damages whatsoever, even if it had been advised of the possibility of damage.  

## Supported Operating Systems
1. Amazon 2017.09 and higher
2. Centos 7
3. Fedora 25 and higher (Fedora 27 recommended)
4. Mint 18
5. Ubuntu 16.04 (Ubuntu 16.10 recommended)
6. Ubuntu 18.04
7. MacOS Darwin 10.12 and higher (MacOS 10.13.x recommended)

## Integrated Services
1. [Native Token](contracts/yx.ntoken/README.md)
1. [Non-native or User Tokens](contracts/yx.token/README.md)
1. [Account Management and KYC](contracts/yx.identity/README.md)
1. [Digital Contract Signing Service](contracts/yx.dcontract/README.md)

## Resources
1. [Website](https://yosemitex.com)
1. [White Paper](https://yosemitex.com/documents/YOSEMITE_Blockchain_Technical_White_Paper_201802a.pdf)
1. [Java SDK](https://github.com/YosemiteLabs/yosemite-j)
<!-- 1. [Roadmap](https://github.com/EOSIO/Documentation/blob/master/Roadmap.md) -->

<!--
<a name="gettingstarted"></a>
## Getting Started
Instructions detailing the process of getting the software, building it, running a simple test network that produces blocks, account creation and uploading a sample contract to the blockchain can be found in [Getting Started](https://developers.eos.io/eosio-nodeos/docs/overview-1) on the [EOSIO Developer Portal](https://developers.eos.io).
-->
