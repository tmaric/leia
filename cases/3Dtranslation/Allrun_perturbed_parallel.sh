#!/usr/bin/env bash

# if mpi_call is unset, initialise it
if [[ -z ${mpi_call+x} ]]; then
  mpi_call="mpirun -np 4"
fi

set -o verbose

rm -rf 0 && cp -r 0.org 0  
find 0/ -name "*.template" -delete && \
touch "$(basename ${PWD}).foam" && \
set -o errexit
blockMesh 

leiaPerturbMesh 
cp -r 0/polyMesh/ constant/ 
rm -rf 0/polyMesh

decomposePar -force
${mpi_call} leiaSetFields -parallel

# Succesfull execution is optional. Purpose: error calculation
./system/init_End/init_End.parallel || { echo "Failed initialising endTime state." && exit 1; }

${mpi_call} leiaLevelSetFoam -fluxCorrection -parallel
