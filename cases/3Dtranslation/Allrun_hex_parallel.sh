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

checkMesh | tee log.checkMesh
# Fetch minCellVolume from checkMesh
minVol=$(awk '/Min volume/{print $4}' log.checkMesh | sed 's/\.$//')
# Calc deltaX with python, because #eval #calc are buggy with small float
deltaX=$(python3 -c "dX=${minVol}**(1/3); print(f'{dX:e}')")
echo $deltaX
# Set deltaX entry in controlDict
sed -i "/^deltaX\>/s/\<[^ \t]*;$/${deltaX};/" system/controlDict

decomposePar -force
${mpi_call} leiaSetFields -parallel

# Succesfull execution is optional. Purpose: error calculation
./system/init_End/init_End.parallel || { echo "Failed initialising endTime state." && exit 1; }

${mpi_call} leiaLevelSetFoam -parallel

