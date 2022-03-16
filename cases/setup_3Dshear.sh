#!/usr/bin/bash

./create-study.py -c testAdvection/ -p 3Dshear.parameter -s 3Dshear 

./bulkrun 3Dshear_00 "cp system/blockMeshDict3Dshear system/blockMeshDict && \
                      cp system/fvSolution3Dshear system/fvSolution && \ 
                      cp system/decomposeParDict3Dshear system/decomposeParDict" 

