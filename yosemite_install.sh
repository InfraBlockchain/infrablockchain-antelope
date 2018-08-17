#!/bin/bash
############################################################################################
# This is the YOSEMITE automated install script for Linux and Mac OS.
# This file was downloaded from https://github.com/YosemiteLabs/yosemite-public-blockchain
#
# This software is available under the following terms:
#
# The MIT License
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# https://github.com/YosemiteLabs/yosemite-public-blockchain/blob/master/LICENSE
############################################################################################
   

CWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ "${CWD}" != "${PWD}" ]; then
  printf "\\n\\tPlease cd into directory %s to run this script.\\n \\tExiting now.\\n\\n" "${CWD}"
  exit 1
fi

BUILD_DIR="${PWD}/build"
CMAKE_BUILD_TYPE=Release
TIME_BEGIN=$( date -u +%s )
INSTALL_PREFIX="/usr/local/yosemite"
VERSION=1.2

txtbld=$(tput bold)
bldred=${txtbld}$(tput setaf 1)
txtrst=$(tput sgr0)

create_symlink() {
  pushd /usr/local/bin &> /dev/null
  ln -sf ../yosemite/bin/$1 $1
  popd &> /dev/null
}

install_symlinks() {
  printf "\\n\\tInstalling YOSEMITE Binary Symlinks\\n\\n"
  create_symlink "clyos"
  create_symlink "keyos"
  create_symlink "yosemite"
}

if [ ! -d "${BUILD_DIR}" ]; then
  printf "\\n\\tError, yosemite_build.sh has not ran.  Please run ./yosemite_build.sh first!\\n\\n"
  exit -1
fi

#${PWD}/scripts/clean_old_install.sh
#if [ $? -ne 0 ]; then
#   printf "\\n\\tError occurred while trying to remove old installation!\\n\\n"
#   exit -1
#fi

if ! pushd "${BUILD_DIR}"
then
  printf "Unable to enter build directory %s.\\n Exiting now.\\n" "${BUILD_DIR}"
  exit 1;
fi

if ! make install
then
  printf "\\n\\t>>>>>>>>>>>>>>>>>>>> MAKE installing YOSEMITE has exited with the above error.\\n\\n"
  exit -1
fi
popd &> /dev/null

install_symlinks

printf "\n\n${bldred}\t__   _____  ____  _____ __  __ ___ _____ _____\n"
printf "\t\ \ / / _ \/ ___|| ____|  \/  |_ _|_   _| ____|\n"
printf "\t \ V / | | \___ \|  _| | |\/| || |  | | |  _|\n"
printf "\t  | || |_| |___) | |___| |  | || |  | | | |___\n"
printf "\t  |_| \___/|____/|_____|_|  |_|___| |_| |_____|${txtrst}\n"

printf "\\tFor more information:\\n"
printf "\\tYOSEMITE website: https://yosemitex.com\\n"
printf "\\tYOSEMITE github: https://github.com/YosemiteLabs/yosemite-public-blockchain\\n\\n\\n"
