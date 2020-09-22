FROM eosio/builder
ARG branch=master
ARG native_token_symbol=DUSD
ARG native_token_precision=4

RUN git clone -b $branch https://github.com/YosemiteLabs/infrablockchain.git --recursive \
    && cd infrablockchain && echo "$branch:$(git rev-parse HEAD)" > /etc/infrablockchain-version \
    && cmake -H. -B"/opt/infrablockchain" -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_ROOT=/opt/wasm -DCMAKE_CXX_COMPILER=clang++ \
       -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=/opt/infrablockchain  -DSecp256k1_ROOT_DIR=/usr/local -DBUILD_MONGO_DB_PLUGIN=true \
       -DINFRABLOCKCHAIN_NATIVE_TOKEN_SYMBOL_NAME=$native_token_symbol -DINFRABLOCKCHAIN_NATIVE_TOKEN_PRECISION=$native_token_precision \
    && cmake --build /opt/infrablockchain --target install \
    && cp /infrablockchain/Docker/config.ini / && ln -s /opt/infrablockchain/contracts /contracts \
    && cp /infrablockchain/Docker/infra-node.sh /opt/infrablockchain/bin/infra-node.sh

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install openssl ca-certificates vim psmisc python3-pip && rm -rf /var/lib/apt/lists/*
RUN pip3 install numpy
ENV EOSIO_ROOT=/opt/infrablockchain
ENV INFRABLOCKCHAIN_ROOT=/opt/infrablockchain
RUN chmod +x /opt/infrablockchain/bin/infra-node.sh
ENV LD_LIBRARY_PATH /usr/local/lib
ENV PATH /opt/infrablockchain/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin