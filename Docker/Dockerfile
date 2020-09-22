FROM eosio/builder as builder
ARG branch=master
ARG native_token_symbol=DUSD
ARG native_token_precision=4

RUN git clone -b $branch https://github.com/YosemiteLabs/infrablockchain.git --recursive \
    && cd infrablockchain && echo "$branch:$(git rev-parse HEAD)" > /etc/infrablockchain-version \
    && cmake -H. -B"/tmp/build" -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_ROOT=/opt/wasm -DCMAKE_CXX_COMPILER=clang++ \
       -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=/tmp/build -DBUILD_MONGO_DB_PLUGIN=true \
       -DINFRABLOCKCHAIN_NATIVE_TOKEN_SYMBOL_NAME=$native_token_symbol -DINFRABLOCKCHAIN_NATIVE_TOKEN_PRECISION=$native_token_precision \
    && cmake --build /tmp/build --target install

FROM ubuntu:18.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install openssl ca-certificates && rm -rf /var/lib/apt/lists/*
COPY --from=builder /usr/local/lib/* /usr/local/lib/
COPY --from=builder /tmp/build/bin /opt/infrablockchain/bin
COPY --from=builder /tmp/build/contracts /contracts
COPY --from=builder /infrablockchain/Docker/config.ini /
COPY --from=builder /etc/infrablockchain-version /etc
COPY --from=builder /infrablockchain/Docker/infra-node.sh /opt/infrablockchain/bin/infra-node.sh
ENV EOSIO_ROOT=/opt/infrablockchain
ENV INFRABLOCKCHAIN_ROOT=/opt/infrablockchain
RUN chmod +x /opt/infrablockchain/bin/infra-node.sh
ENV LD_LIBRARY_PATH /usr/local/lib
ENV PATH /opt/infrablockchain/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
