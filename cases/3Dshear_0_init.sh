#!/usr/bin/env bash

./create-study.py -c testAdvection/ -p 3Dshear_tet.parameter -s 3Dshear_tet

./bulkrun 3Dshear_tet_00 "cp system/blockMeshDict3Dshear system/blockMeshDict && \
                      cp system/fvSolution3Dshear system/fvSolution && \
                      cp system/controlDict3Dshear system/controlDict && \
                      cp system/decomposeParDict3Dshear system/decomposeParDict" 

