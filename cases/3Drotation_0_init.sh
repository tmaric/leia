#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ]; 
then 
    STUDY=3Drotation
else
    STUDY=3Drotation_$STUDY
fi

./create-study.py -c 3Drotation -p 3Drotation.parameter -s $STUDY 
