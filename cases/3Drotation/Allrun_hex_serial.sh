#!/usr/bin/env bash
set -o verbose

rm -rf 0 && cp -r 0.org 0  
touch "$(basename ${PWD}).foam" && \
set -o errexit
blockMesh 
leiaSetFields 
leiaLevelSetFoam 

