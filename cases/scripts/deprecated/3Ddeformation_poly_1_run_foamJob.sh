#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ];
then
    STUDY=3Ddeformation_poly_000
fi


./bulkrun $STUDY "foamJob eval '\
	pMesh && decomposePar -force && mpirun -np 4 leiaSetFields -parallel && \
	mpirun -np 4 leiaLevelSetFoam -fluxCorrection -parallel'" 

echo $(date) $STUDY >> ACTIVE_STUDIES
