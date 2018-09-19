FROM eosio/builder
ARG branch=yosemite-master
ARG native_token_symbol=DKRW
ARG native_token_precision=4

RUN git clone -b $branch https://github.com/YosemiteLabs/yosemite-public-blockchain.git --recursive \
    && cd yosemite-public-blockchain && echo "$branch:$(git rev-parse HEAD)" > /etc/yosemite-version \
    && cmake -H. -B"/opt/yosemite" -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_ROOT=/opt/wasm -DCMAKE_CXX_COMPILER=clang++ \
       -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=/opt/yosemite  -DSecp256k1_ROOT_DIR=/usr/local -DBUILD_MONGO_DB_PLUGIN=true \
       -DYOSEMITE_NATIVE_TOKEN_SYMBOL_NAME=$native_token_symbol -DYOSEMITE_NATIVE_TOKEN_PRECISION=$native_token_precision \
    && cmake --build /opt/yosemite --target install \
    && cp /yosemite-public-blockchain/Docker/config.ini / && ln -s /opt/yosemite/contracts /contracts \
    && cp /yosemite-public-blockchain/Docker/yosemited.sh /opt/yosemite/bin/yosemited.sh

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get -y install openssl ca-certificates vim psmisc python3-pip && rm -rf /var/lib/apt/lists/*
RUN pip3 install numpy
ENV EOSIO_ROOT=/opt/yosemite
ENV YOSEMITE_ROOT=/opt/yosemite
RUN chmod +x /opt/yosemite/bin/yosemited.sh
ENV LD_LIBRARY_PATH /usr/local/lib
VOLUME /opt/yosemite/bin/data-dir
ENV PATH /opt/yosemite/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
