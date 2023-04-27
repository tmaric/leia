#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ]; 
then 
    STUDY=3Dshear_poly
else
    STUDY=3Dshear_poly_$STUDY
fi

./create-study.py -c 3Dshear -p 3Dshear_poly.parameter -s $STUDY 

./bulkrun "$STUDY"_00 "cp ../3Dshear/box*stl . && \
	cp system/controlDict3Dshear_poly system/controlDict"

