#!/usr/bin/env bash

STUDY=$1

if [ -z $STUDY ];
then
    STUDY=3Drotation_perturbed_000
fi

./bulkrun $STUDY "foamJob eval '\
	blockMesh && leiaPerturbMesh && cp -r 0/polyMesh/ constant/ && \
	rm -rf 0/polyMesh && decomposePar -force && mpirun -np 4 leiaSetFields -parallel && \
	mpirun -np 4 leiaLevelSetFoam -fluxCorrection -parallel'" 

echo $(date) $STUDY >> ACTIVE_STUDIES