#!/usr/bin/env bash
set -o verbose

restore0Dir  
touch "$(basename ${PWD}).foam" && \
set -o errexit
pMesh


leiaSetFields 
leiaLevelSetFoam 

