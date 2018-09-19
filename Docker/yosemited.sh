#!/bin/sh
cd /opt/yosemite/bin

if [ -f '/opt/yosemite/bin/data-dir/config.ini' ]; then
    echo
  else
    cp /config.ini /opt/yosemite/bin/data-dir
fi

if [ -d '/opt/yosemite/bin/data-dir/contracts' ]; then
    echo
  else
    cp -r /contracts /opt/yosemite/bin/data-dir
fi

while :; do
    case $1 in
        --config-dir=?*)
            CONFIG_DIR=${1#*=}
            ;;
        *)
            break
    esac
    shift
done

if [ ! "$CONFIG_DIR" ]; then
    CONFIG_DIR="--config-dir=/opt/yosemite/bin/data-dir"
else
    CONFIG_DIR=""
fi

exec /opt/yosemite/bin/yosemite $CONFIG_DIR "$@"
