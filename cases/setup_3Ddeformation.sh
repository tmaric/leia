#!/usr/bin/bash

./create-study.py -c testAdvection/ -p 3Ddeformation.parameter -s 3Ddeformation 

./bulkrun 3Ddeformation_00 "cp system/blockMeshDict3Ddeformation system/blockMeshDict && \
                      cp system/fvSolution3Ddeformation system/fvSolution && 
                      cp sytem/controlDict3Ddeformation system/controlDict" 

