#!/usr/bin/env bash

./create-study.py -c testAdvection/ -p 3Dshear_poly.parameter -s 3Dshear_poly 

./bulkrun 3Dshear_poly_00 "cp ../testAdvection/box-shear-edges.stl . && \
                           cp system/fvSolution3Dshear system/fvSolution && \
                           cp system/controlDict3Dshear_poly system/controlDict && \
                           cp system/decomposeParDict3Dshear system/decomposeParDict" 

