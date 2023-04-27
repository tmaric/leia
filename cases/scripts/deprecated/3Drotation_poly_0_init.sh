#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ]; 
then 
    STUDY=3Drotation_poly
else
    STUDY=3Drotation_poly_$STUDY
fi

./create-study.py -c 3Drotation -p 3Drotation_poly.parameter -s $STUDY

./bulkrun "$STUDY"_00 "cp system/controlDict3Drotation_poly system/controlDict && \
		       cp ../3Drotation/box*stl ."
