#!/usr/bin/env bash

./create-study.py -c testAdvection/ -p 3Ddeformation_tet.parameter -s 3Ddeformation_tet

./bulkrun 3Ddeformation_tet_00 "cp ../testAdvection/box*.stl . && 
                      cp system/fvSolution3Ddeformation system/fvSolution && \
                      cp system/controlDict3Ddeformation_poly system/controlDict && \
                      cp system/decomposeParDict3Ddeformation system/decomposeParDict" 

