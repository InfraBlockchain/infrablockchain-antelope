#!/bin/bash
############################################################################################
# This is the InfraBlockchain automated install script for Linux and Mac OS.
# This file was downloaded from https://github.com/YosemiteLabs/infrablockchain
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
# https://github.com/YosemiteLabs/infrablockchain/blob/master/LICENSE
############################################################################################

txtbld=$(tput bold)
bldred=${txtbld}$(tput setaf 6)
txtrst=$(tput sgr0)

TIME_BEGIN=$( date -u +%s )

/bin/bash ${PWD}/eosio_build.sh
retVal=$?
if [ $retVal -ne 0 ]; then
    exit $retVal
fi

TIME_END=$(( $(date -u +%s) - ${TIME_BEGIN} ))

printf "\n\n${bldred}"
printf "\t _____        __           ____  _            _        _           _\n"
printf "\t|_   _|      / _|         |  _ \\| |          | |      | |         (_)\n"
printf "\t  | |  _ __ | |_ _ __ __ _| |_) | | ___   ___| | _____| |__   __ _ _ _ __\n"
printf "\t  | | | '_ \\|  _| '__/ _\` |  _ <| |/ _ \\ / __| |/ / __| '_ \\ / _\` | | '_ \\ \n"
printf "\t _| |_| | | | | | | | (_| | |_) | | (_) | (__|   < (__| | | | (_| | | | | |\n"
printf "\t|_____|_| |_|_| |_|  \\__,_|____/|_|\\___/ \\___|_|\\_\\___|_| |_|\\__,_|_|_| |_|${txtrst}\n"


printf "\\n\\tInfraBlockchain has been successfully built. %02d:%02d:%02d\\n\\n" $(($TIME_END/3600)) $(($TIME_END%3600/60)) $(($TIME_END%60))

printf "\\tFor more information:\\n"
printf "\\tInfraBlockchain website: https://infrablockchain.com\\n"
printf "\\tInfraBlockchain github: https://github.com/YosemiteLabs/infrablockchain\\n\\n\\n"
