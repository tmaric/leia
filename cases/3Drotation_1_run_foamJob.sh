#!/usr/bin/env bash

./bulkrun 3Drotation_000 "foamJob eval '\
	blockMesh && decomposePar -force && mpirun -np 4 leiaSetFields -parallel && \
	mpirun -np 4 leiaLevelSetFoam -parallel'" 