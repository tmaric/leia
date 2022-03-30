#!/usr/bin/env bash

./create-study.py -c testAdvection/ -p 3Ddeformation_poly.parameter -s 3Ddeformation_poly

./bulkrun 3Ddeformation_poly_00 "cp ../testAdvection/box*.stl . && 
                      cp system/fvSolution3Ddeformation system/fvSolution && \
                      cp system/controlDict3Ddeformation_poly system/controlDict && \
                      cp system/decomposeParDict3Ddeformation system/decomposeParDict" 

