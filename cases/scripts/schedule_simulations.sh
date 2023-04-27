#!/usr/bin/env bash

readonly SCRIPT_PATH=$(realpath "${BASH_SOURCE[0]}")
readonly DIR_PATH="$(dirname $SCRIPT_PATH)"

if [[ $# -ne 3 ]]; then
	echo "Provide file with list of cases as parameter"
	exit 1
fi

caselist=( $(<$1) )
jobscript=$(realpath $2)
concurrent_simulations=$3

for case in ${caselist[@]}; do
	while [[ $(ps -A | grep foamJob | wc -l) -ge "$concurrent_simulations" ]]; do
		sleep 1m
	done
	echo
	echo $case && \
	cd $case && \
	date && \
	foamJob "$jobscript" && \
	cd $DIR_PATH
	sleep 5
done
