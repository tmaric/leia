#! /usr/bin/env bash

set -v

 pyFoamRunParameterVariation.py --list-variations ../../3Ddeformation ../../3Ddeformation.parameter > 3Ddeformation_variation.txt

 gather-study-data.py -v 3Ddeformation_variation.txt -f 3Ddeformation_gathered ../../study_case_mesh_BC_div_source_h/3Ddeformation_hex_BC-div-source-h_00000_3Ddeformation/leiaLevelSetFoam.csv 

 convergencerates.py 3Ddeformation_gathered.csv 

