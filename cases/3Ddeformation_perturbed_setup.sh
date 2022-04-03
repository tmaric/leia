#!/usr/bin/env bash

./create-study.py -c testAdvection/ -p 3Ddeformation.parameter -s 3Ddeformation_perturbed 

./bulkrun 3Ddeformation_perturbed_00 "cp system/blockMeshDict3Ddeformation system/blockMeshDict && \
                      cp system/fvSolution3Ddeformation system/fvSolution && \
                      cp system/controlDict3Ddeformation system/controlDict && \
                      cp system/decomposeParDict3Ddeformation system/decomposeParDict" 

