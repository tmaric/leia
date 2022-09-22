#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ]; 
then 
    STUDY=3Dshear
else
    STUDY=3Dshear-$STUDY
fi

./create-study.py -c 3Dshear -p 3Dshear.parameter -s $STUDY 
