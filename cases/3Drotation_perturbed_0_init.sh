#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ]; 
then 
    STUDY=3Drotation-perturbed
else
    STUDY=3Drotation-perturbed-$STUDY
fi

./create-study.py -c 3Drotation -p 3Drotation_perturbed.parameter -s $STUDY
