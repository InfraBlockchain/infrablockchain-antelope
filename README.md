# InfraBlockchain

* [InfraBlockchain Website - https://infrablockchain.com](https://infrablockchain.com)
* [Technical White Paper Version 2.4 - InfraBlockchain_Technical_White_Paper_Version_2_4_ENG_202008.pdf](https://infrablockchain.com/documents/InfraBlockchain_Technical_White_Paper_Version_2_4_ENG_202008.pdf)

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

InfraBlockchain code base is forked from [mandel](https://github.com/eosnetworkfoundation/mandel)

## Software Installation

Visit the [release page](https://github.com/InfraBlockchain/infrablockchain/releases) for binaries. This is the fastest way to get started with the software.

### Building From Source

Recent Ubuntu LTS releases are the only Linux distributions that we fully support. Other Linux distros and other POSIX operating systems (such as macOS) are tended to on a best-effort basis and may not be full featured. Notable requirements to build are:
* C++17 compiler and standard library
* boost 1.67+
* CMake 3.8+
* (for Linux only) LLVM 7 - 11 (newer versions do not work)

A few other common libraries are tools also required such as openssl 1.1+, libcurl, curl, libusb, GMP, Python 3, and zlib.

**A Warning On Parallel Compilation Jobs (`-j` flag)**: When building C/C++ software often the build is performed in parallel via a command such as `make -j $(nproc)` which uses the number of CPU cores as the number of compilation jobs to perform simultaneously. However, be aware that some compilation units (.cpp files) in mandel are extremely complex and will consume nearly 4GB of memory to compile. You may need to reduce the level of parallelization depending on the amount of memory on your build host. e.g. instead of `make -j $(nproc)` run `make -j2`. Failures due to memory exhaustion will typically but not always manifest as compiler crashes.

Generally we recommend performing what we refer to as a "pinned build" which ensures the compiler and boost version remain the same between builds of different mandel versions (mandel requires these versions remain the same otherwise its state needs to be repopulated from a portable snapshot).

#### Building Pinned Build Binary Packages
In the directory `<mandel src>/scripts` you will find the two scripts `install_deps.sh` and `pinned_build.sh`. If you haven't installed build dependencies then run `install_deps.sh`. Then run `pinned_build.sh <dependencies directory> <mandel build directory> <number of jobs>`.

The dependencies directory is where the script will pull the C++ dependencies that need to be built with the pinned compiler for building the pinned binaries for binary packaging.

The binary package will be produced in the mandel build directory that was supplied.

#### Manual (non "pinned") Build Instructions

<details>
  <summary>Ubuntu 20.04 & 22.04 Build Instructions</summary>

Install required dependencies: 
```
apt-get update && apt-get install   \
        build-essential             \
        cmake                       \
        curl                        \
        git                         \
        libboost-all-dev            \
        libcurl4-openssl-dev        \
        libgmp-dev                  \
        libssl-dev                  \
        libusb-1.0-0-dev            \
        llvm-11-dev                 \
        pkg-config
```
and perform the build:
```
git submodule update --init --recursive
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j $(nproc) package
```
</details>

<details>
  <summary>Ubuntu 18.04 Build Instructions</summary>

Install required dependencies. You will need to build Boost from source on this distribution. 
```
apt-get update && apt-get install   \
        build-essential             \
        cmake                       \
        curl                        \
        g++-8                       \
        git                         \
        libcurl4-openssl-dev        \
        libgmp-dev                  \
        libssl-dev                  \
        libusb-1.0-0-dev            \
        llvm-7-dev                  \
        pkg-config                  \
        python3                     \
        zlib1g-dev
        
curl -L https://boostorg.jfrog.io/artifactory/main/release/1.79.0/source/boost_1_79_0.tar.bz2 | tar jx && \
   cd boost_1_79_0 &&                                                                                     \
   ./bootstrap.sh --prefix=$HOME/boost1.79 &&                                                             \
   ./b2 --with-iostreams --with-date_time --with-filesystem --with-system                                 \
        --with-program_options --with-chrono --with-test -j$(nproc) install &&                            \
   cd ..
```
and perform the build:
```
git submodule update --init --recursive
mkdir build
cd build
cmake -DCMAKE_C_COMPILER=gcc-8 -DCMAKE_CXX_COMPILER=g++-8 \
      -DCMAKE_PREFIX_PATH="$HOME/boost1.79;/usr/lib/llvm-7/"  -DCMAKE_BUILD_TYPE=Release .. \
make -j $(nproc) package
```
After building you may remove the `$HOME/boost1.79` directory, or you may keep it around until next time building the software.
</details>