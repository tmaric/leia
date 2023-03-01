#!/usr/bin/env bash

./bulkrun 3Dshear_tet_000 "foamJob eval '\
	tetMesh && decomposePar -force && mpirun -np 4 leiaSetFields -parallel && \
	mpirun -np 4 leiaLevelSetFoam -parallel'" 
