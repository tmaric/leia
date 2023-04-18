#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ]; 
then 
    STUDY=3Dshear_perturbed
else
    STUDY=3Dshear_perturbed_$STUDY
fi

./create-study.py -c 3Dshear -p 3Dshear_perturbed.parameter -s $STUDY
