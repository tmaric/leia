#!/usr/bin/env bash

./bulkrun 3Dtranslation_00 "foamJob eval '\
	blockMesh && decomposePar -force && mpirun -np 4 leiaSetFields -parallel && \
	mpirun -np 4 leiaLevelSetFoam -parallel'"

echo 3Dtranslation >> ACTIVE_STUDIES