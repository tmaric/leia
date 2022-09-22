#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ]; 
then 
    STUDY=3Ddeformation_poly
else
    STUDY=3Ddeformation_poly-$STUDY
fi

./create-study.py -c 3Ddeformation -p 3Ddeformation_poly.parameter \
	-s $STUDY 

./bulkrun "$STUDY"_00 \
	"cp ../3Ddeformation/box*.stl . && \
 	 cp system/controlDict3Ddeformation_poly system/controlDict" 
