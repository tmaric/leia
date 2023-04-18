#!/usr/bin/env bash

STUDY=$1
SCRIPT=$2

if [ -z $STUDY ]; 
then 
	echo "Provide 1st parameter: studyname"
fi
if [ -z $SCRIPT ]; 
then 
	SCRIPT="./Allrun_hex_parallel.sh"
fi

./bulkrun $STUDY "foamJob ${SCRIPT}"

echo $(date) $STUDY >> ACTIVE_STUDIES

