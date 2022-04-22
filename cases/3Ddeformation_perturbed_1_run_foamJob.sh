#!/usr/bin/env bash

./bulkrun 3Ddeformation_perturbed_000 "foamJob eval '\
	blockMesh && leiaPerturbMesh && cp -r 0/polyMesh/ constant/ && \
	rm -rf 0/polyMesh && decomposePar -force && mpirun -np 4 leiaSetFields -parallel && \
	mpirun -np 4 leiaLevelSetFoam -parallel'" 
