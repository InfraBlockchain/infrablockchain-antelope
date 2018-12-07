# Run in docker

Simple and fast setup of YOSEMITE on Docker is also available.

## Install Dependencies

- [Docker](https://docs.docker.com) Docker 17.05 or higher is required
- [docker-compose](https://docs.docker.com/compose/) version >= 1.10.0

## Docker Requirement

- At least 7GB RAM (Docker -> Preferences -> Advanced -> Memory -> 7GB or above)
- If the build below fails, make sure you've adjusted Docker Memory settings and try again.

## Build yosemite image

```bash
git clone https://github.com/YosemiteLabs/yosemite-public-blockchain --recursive  --depth 1
cd yosemite-public-blockchain/Docker
docker build . -t yosemitex/yosemite
```

By default, the native token symbol is set to DKRW with precision 4. You can override this using the symbol argument while building the docker image.

```bash
docker build -t yosemitex/yosemite --build-arg symbol=<symbol> .
```

## Start nodeos docker container only

```bash
docker run --name yosemite -p 8888:8888 -p 9876:9876 -t yosemitex/yosemite yosemited.sh -e --http-alias=yosemite:8888 --http-alias=127.0.0.1:8888 --http-alias=localhost:8888 arg1 arg2
```

By default, all data is persisted in a docker volume. It can be deleted if the data is outdated or corrupted:

```bash
$ docker inspect --format '{{ range .Mounts }}{{ .Name }} {{ end }}' yosemite
07c4108021e9523770cf2ee6a637364797e3a8a99589cecd2aa40f1e7ea2f436
$ docker volume rm 07c4108021e9523770cf2ee6a637364797e3a8a99589cecd2aa40f1e7ea2f436
```

Alternately, you can directly mount host directory into the container

```bash
docker run --name yosemite -v /path-to-data-dir:/opt/yosemite/bin/data-dir -p 8888:8888 -p 9876:9876 -t yosemitex/yosemite yosemited.sh -e --http-alias=nodeos:8888 --http-alias=127.0.0.1:8888 --http-alias=localhost:8888 arg1 arg2
```

## Get chain info

```bash
curl http://127.0.0.1:8888/v1/chain/get_info
```

## Start both yosemite and keyos containers

```bash
docker volume create --name=yosemite-data-volume
docker volume create --name=keyos-data-volume
docker-compose up -d
```

After `docker-compose up -d`, two services named `yosemited` and `keyos` will be started. The yosemited service would expose ports 8888 and 9876 to the host. The keyos service does not expose any port to the host, it is only accessible to clyos when running cleos is running inside the keyos container as described in "Execute clyos commands" section.

### Execute clyos commands

You can run the `clyos` commands via a bash alias.

```bash
alias clyos='docker-compose exec keyos /opt/yosemite/bin/clyos -u http://yosemited:8888 --wallet-url http://localhost:8900'
cleos get info
cleos get account inita
```

If you don't need keyos afterwards, you can stop the keyos service using

```bash
docker-compose stop keyos
```

### Change default configuration

You can use docker compose override file to change the default configurations. For example, create an alternate config file `config2.ini` and a `docker-compose.override.yml` with the following content.

```yaml
version: "2"

services:
  yosemite:
    volumes:
      - yosemite-data-volume:/opt/yosemite/bin/data-dir
      - ./config2.ini:/opt/yosemite/bin/data-dir/config.ini
```

Then restart your docker containers as follows:

```bash
docker-compose down
docker-compose up
```

### Clear data-dir

The data volume created by docker-compose can be deleted as follows:

```bash
docker volume rm yosemite-data-volume
docker volume rm keyos-data-volume
```

### Docker Hub

Docker Hub image available from [docker hub](https://hub.docker.com/r/yosemitex/yosemite/).
Create a new `docker-compose.yaml` file with the content below

```bash
version: "3"

services:
  yosemited:
    image: yosemitex/yosemite:latest
    command: /opt/yosemite/bin/yosemited.sh --data-dir /opt/yosemite/bin/data-dir -e --http-alias=yosemited:8888 --http-alias=127.0.0.1:8888 --http-alias=localhost:8888
    hostname: yosemited
    ports:
      - 8888:8888
      - 9876:9876
    expose:
      - "8888"
    volumes:
      - yosemite-data-volume:/opt/yosemite/bin/data-dir

  keyos:
    image: yosemitex/yosemite:latest
    command: /opt/yosemite/bin/keyos --wallet-dir /opt/yosemite/bin/data-dir --http-server-address=127.0.0.1:8900 --http-alias=localhost:8900 --http-alias=keyos:8900
    hostname: keyos
    links:
      - yosemited
    volumes:
      - keyos-data-volume:/opt/yosemite/bin/data-dir

volumes:
  yosemite-data-volume:
  keyos-data-volume:

```

*NOTE:* the default version is the latest, you can change it to what you want

run `docker pull yosemitex/yosemite:latest`

run `docker-compose up`

### EOSIO Testnet

We can easily set up a EOSIO local testnet using docker images. Just run the following commands:

Note: if you want to use the mongo db plugin, you have to enable it in your `data-dir/config.ini` first.

```
# create volume
docker volume create --name=yosemite-data-volume
docker volume create --name=keyos-data-volume
# pull images and start containers
docker-compose -f docker-compose-yosemite-latest.yaml up -d
# get chain info
curl http://127.0.0.1:8888/v1/chain/get_info
# get logs
docker-compose logs -f yosemited
# stop containers
docker-compose -f docker-compose-yosemite-latest.yaml down
```

The `blocks` data are stored under `--data-dir` by default, and the wallet files are stored under `--wallet-dir` by default, of course you can change these as you want.

### About MongoDB Plugin

Currently, the mongodb plugin is disabled in `config.ini` by default, you have to change it manually in `config.ini` or you can mount a `config.ini` file to `/opt/yosemite/bin/data-dir/config.ini` in the docker-compose file.
