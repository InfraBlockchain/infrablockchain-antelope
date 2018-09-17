FROM eosio/builder as builder
ARG branch=master
ARG native_token_symbol=DKRW
ARG native_token_precision=4

RUN git clone -b $branch https://github.com/YosemiteLabs/yosemite-public-blockchain.git --recursive \
    && cd yosemite-public-blockchain && echo "$branch:$(git rev-parse HEAD)" > /etc/yosemite-version \
    && cmake -H. -B"/tmp/build" -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_ROOT=/opt/wasm -DCMAKE_CXX_COMPILER=clang++ \
       -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=/tmp/build  -DSecp256k1_ROOT_DIR=/usr/local -DBUILD_MONGO_DB_PLUGIN=true \
       -DYOSEMITE_NATIVE_TOKEN_SYMBOL_NAME=$native_token_symbol -DYOSEMITE_NATIVE_TOKEN_PRECISION=$native_token_precision \
    && cmake --build /tmp/build --target install


FROM ubuntu:18.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install openssl ca-certificates && rm -rf /var/lib/apt/lists/*
COPY --from=builder /usr/local/lib/* /usr/local/lib/
COPY --from=builder /tmp/build/bin /opt/yosemite/bin
COPY --from=builder /tmp/build/contracts /contracts
COPY --from=builder /yosemite-public-blockchain/Docker/config.ini /
COPY --from=builder /etc/yosemite-version /etc
COPY --from=builder /yosemite-public-blockchain/Docker/yosemited.sh /opt/yosemite/bin/yosemited.sh
ENV EOSIO_ROOT=/opt/yosemite
ENV YOSEMITE_ROOT=/opt/yosemite
RUN chmod +x /opt/yosemite/bin/yosemited.sh
ENV LD_LIBRARY_PATH /usr/local/lib
VOLUME /opt/yosemite/bin/data-dir
ENV PATH /opt/yosemite/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
