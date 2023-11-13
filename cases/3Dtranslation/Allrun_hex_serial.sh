#!/usr/bin/env bash
set -o verbose

restore0Dir  
touch "$(basename ${PWD}).foam" && \
set -o errexit
blockMesh 


leiaSetFields 

# Succesfull execution is optional. Purpose: error calculation
./system/init_End/init_End.serial || { echo "Failed initialising endTime state." && exit 1; }

leiaLevelSetFoam 

