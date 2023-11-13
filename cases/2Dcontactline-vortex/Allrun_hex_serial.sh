#!/usr/bin/env bash
set -o verbose

restore0Dir  
touch "$(basename ${PWD}).foam" && \
set -o errexit
blockMesh 


leiaSetFields 
leiaLevelSetFoam 

