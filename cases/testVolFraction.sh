#! /usr/bin/bash


./create-study.py -p testVolFraction.parameter -c testVolFraction -s testVolFraction2D
./bulkrun testVolFraction2D_0000 "cp system/blockMeshDict2D system/blockMeshDict"
./bulkrun testVolFraction2D_0000 "blockMesh" 
./bulkrun testVolFraction2D_0000 "leiaTestFoamGeometry"
./create-study.py -s testVolFraction -c testVolFraction -p testVolFraction.parameter
./bulkrun testVolFraction_0000 "blockMesh"
./bulkrun testVolFraction_0000 "leiaTestFoamGeometry"
