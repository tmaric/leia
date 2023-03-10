#! /usr/bin/env bash

# Script prints out if the simulation of an OpenFOAM case is finished/ complete.
# It searches for a log or out.* file and reads its last 5 lines.
# If it fetches a "End", the case is considered as complete. 


function foamIsCaseComplete()
{

	if [[ $# -ge 1 ]]; then
		local case="$(realpath $1)"
		if [[ -f $case/log ]]; then
			local log="${case}/log"
		elif find $case -maxdepth 1 -type f -name "out.*" > /dev/null ; then
			local log="$(find $case -maxdepth 1 -type f -name "out.*" | sort | tail -n1)"
		fi
	else
		echo "Provide case as argument" >2
		return 1
	fi


	local lastline="$(tail -n 5 $log 2> /dev/null)"
	if [[ $? -ne 0 ]]; then
		echo "NOT STARTED: $case has no log" 
	fi
	if echo $lastline | grep "End" > /dev/null; then
		echo "COMPLETE: $case ended."
		return 0
	else
		echo "FAILED: $case is not finished."
		return 2
	fi
}

foamIsCaseComplete "$@"

