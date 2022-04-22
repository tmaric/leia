#!/usr/bin/env bash

./bulkrun 3Ddeformation_poly_000 "foamJob eval '\
	pMesh && decomposePar -force && mpirun -np 4 leiaSetFields -parallel && \
	mpirun -np 4 leiaLevelSetFoam -parallel'" 
