#!/usr/bin/env bash
set -eo pipefail
VERSION=2.1

# Ensure we're in the repo root and not inside of scripts
cd $( dirname "${BASH_SOURCE[0]}" )/..

# Load infrablockchain specific helper functions
. ./scripts/helpers/infrablockchain.sh

[[ ! $NAME == "Ubuntu" ]] && set -i # Ubuntu doesn't support interactive mode since it uses dash

[[ ! -f ${BUILD_DIR}/CMakeCache.txt ]] && printf "${COLOR_RED}Please run ${SCRIPT_DIR}/infrablockchain_build.sh first!${COLOR_NC}" && exit 1
echo "${COLOR_CYAN}====================================================================================="
echo "========================== ${COLOR_WHITE}Starting InfraBlockchain Installation${COLOR_CYAN} ==============================${COLOR_NC}"
execute cd $BUILD_DIR
execute make install
execute cd ..

echo "\n${COLOR_RED}  _____        __           ____  _            _        _           _"
echo " |_   _|      / _|         |  _ \| |          | |      | |         (_)"
echo "   | |  _ __ | |_ _ __ __ _| |_) | | ___   ___| | _____| |__   __ _ _ _ __"
echo "   | | | '_ \|  _| '__/ _\` |  _ <| |/ _ \ / __| |/ / __| '_ \ / _\` | | '_ \\"
echo "  _| |_| | | | | | | | (_| | |_) | | (_) | (__|   < (__| | | | (_| | | | | |"
echo " |_____|_| |_|_| |_|  \__,_|____/|_|\___/ \___|_|\_\___|_| |_|\__,_|_|_| |_|${COLOR_NC}"
echo " "

printf "==============================================================================================\\n"
printf "${COLOR_GREEN}InfraBlockchain has been installed into ${CACHED_INSTALL_PATH}/bin${COLOR_NC}"
printf "\\n${COLOR_YELLOW}Uninstall with: ${SCRIPT_DIR}/infrablockchain_uninstall.sh${COLOR_NC}\\n"
printf "==============================================================================================\\n\\n"
resources
