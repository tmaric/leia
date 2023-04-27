#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ];
then
    STUDY=3Dshear_poly_000
fi

./bulkrun $STUDY "foamJob eval '\
	pMesh && decomposePar -force && mpirun -np 4 leiaSetFields -parallel && \
	mpirun -np 4 leiaLevelSetFoam -parallel'" 

echo $(date) $STUDY >> ACTIVE_STUDIES
