#!/usr/bin/env bash

./create-study.py -c testAdvection/ -p 3Drotation_tet.parameter -s 3Drotation_tet

./bulkrun 3Drotation_tet_00 "cp ../testAdvection/box-edges.stl . && \
                      cp system/fvSolution3Drotation system/fvSolution && \
                      cp system/controlDict3Drotation_poly system/controlDict && \
                      cp system/decomposeParDict3Drotation system/decomposeParDict" 

