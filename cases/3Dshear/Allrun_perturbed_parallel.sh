#!/usr/bin/env bash

# if mpi_call is unset, initialise it
if [[ -z ${mpi_call+x} ]]; then
  mpi_call="mpirun -np 4"
fi

set -o verbose

restore0Dir  
find 0/ -name "*.template" -delete && \
touch "$(basename ${PWD}).foam" && \
set -o errexit
blockMesh 

leiaPerturbMesh 
cp -r 0/polyMesh/ constant/ 
rm -rf 0/polyMesh


decomposePar -force
${mpi_call} leiaSetFields -parallel
${mpi_call} leiaLevelSetFoam -fluxCorrection -parallel
