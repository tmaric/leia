#!/usr/bin/env bash

./create-study.py -c testAdvection/ -p 3Drotation.parameter -s 3Drotation 

./bulkrun 3Drotation_00 "cp system/blockMeshDict3Drotation system/blockMeshDict && \
                      cp system/fvSolution3Drotation system/fvSolution && \
                      cp system/controlDict3Drotation system/controlDict && \
                      cp system/decomposeParDict3Drotation system/decomposeParDict" 

