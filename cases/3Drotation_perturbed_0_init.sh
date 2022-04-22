#!/usr/bin/env bash

./create-study.py -c testAdvection/ -p 3Drotation_perturbed.parameter -s 3Drotation_perturbed

./bulkrun 3Drotation_perturbed_00 "cp system/blockMeshDict3Drotation system/blockMeshDict && \
                      cp system/fvSolution3Drotation system/fvSolution && \
                      cp system/controlDict3Drotation system/controlDict && \
                      cp system/decomposeParDict3Drotation system/decomposeParDict" 

