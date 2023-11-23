#!/usr/bin/env bash

. ${WM_PROJECT_DIR}/bin/tools/RunFunctions

set -o verbose

restore0Dir  
touch "$(basename ${PWD}).foam" && \
set -o errexit
blockMesh 


leiaSetFields 
leiaLevelSetFoam 

