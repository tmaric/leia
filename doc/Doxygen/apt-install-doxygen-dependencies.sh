#!/usr/bin/env bash

# Latex installation is only required for plotting

readonly SCRIPT_PATH=$(realpath "${BASH_SOURCE[0]}")
readonly DIR_PATH="$(dirname $SCRIPT_PATH)"

xargs -a "${DIR_PATH}/doxygen-dependencies_pkgs.txt" sudo apt install -y 
