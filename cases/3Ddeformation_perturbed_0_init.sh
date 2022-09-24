#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ]; 
then 
    STUDY=3Ddeformation_perturbed
else
    STUDY=3Ddeformation_perturbed_$STUDY
fi

./create-study.py -c 3Ddeformation -p 3Ddeformation.parameter -s $STUDY 
