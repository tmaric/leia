#!/usr/bin/env bash

. ${WM_PROJECT_DIR}/bin/tools/RunFunctions

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


decomposePar -force
${mpi_call} leiaSetFields -parallel

# Succesfull execution is optional. Purpose: error calculation
./system/init_End/init_End.parallel || { echo "Failed initialising endTime state." && exit 1; }

${mpi_call} leiaLevelSetFoam -parallel

