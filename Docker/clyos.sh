#!/bin/bash

# Usage:
# Go into cmd loop: sudo ./clyos.sh
# Run single cmd:  sudo ./clyos.sh <clyos paramers>

PREFIX="docker-compose exec yosemited clyos"
if [ -z $1 ] ; then
  while :
  do
    read -e -p "clyos " cmd
    history -s "$cmd"
    $PREFIX $cmd
  done
else
  $PREFIX "$@"
fi
