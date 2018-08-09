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

TIME_BEGIN=$( date -u +%s )

${PWD}/eosio_build.sh

TIME_END=$(( $(date -u +%s) - ${TIME_BEGIN} ))

printf "\n\n${bldred}\t__   _____  ____  _____ __  __ ___ _____ _____\n"
printf "\t\ \ / / _ \/ ___|| ____|  \/  |_ _|_   _| ____|\n"
printf "\t \ V / | | \___ \|  _| | |\/| || |  | | |  _|\n"
printf "\t  | || |_| |___) | |___| |  | || |  | | | |___\n"
printf "\t  |_| \___/|____/|_____|_|  |_|___| |_| |_____|${txtrst}\n"

printf "\\n\\tYOSEMITE has been successfully built. %02d:%02d:%02d\\n\\n" $(($TIME_END/3600)) $(($TIME_END%3600/60)) $(($TIME_END%60))

printf "\\tFor more information:\\n"
printf "\\tYOSEMITE website: https://yosemitex.com\\n"
printf "\\tYOSEMITE github: https://github.com/YosemiteLabs/yosemite-public-blockchain\\n\\n\\n"
