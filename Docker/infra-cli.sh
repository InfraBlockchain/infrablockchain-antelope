#!/bin/bash

# Usage:
# Go into cmd loop: sudo ./infra-cli.sh
# Run single cmd:  sudo ./infra-cli.sh <infra-cli paramers>

PREFIX="docker-compose exec infra-node infra-cli"
if [ -z $1 ] ; then
  while :
  do
    read -e -p "infra-cli " cmd
    history -s "$cmd"
    $PREFIX $cmd
  done
else
  $PREFIX "$@"
fi
