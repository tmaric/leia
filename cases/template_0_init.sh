#!/usr/bin/env bash

TEMPLATE=$1
STUDY=$2

if [ -z $TEMPLATE ]; 
then 
	echo "Provide 1st parameter: templatecase"
fi

if [ -z $STUDY ]; 
then 
	echo "Provide 2st parameter: studyname"
fi

./create-study.py -c "${TEMPLATE}" -p "${TEMPLATE}.parameter" -s "$STUDY" 

