#! /usr/bin/env bash

set -v

 pyFoamRunParameterVariation.py --list-variations ../../3Drotation ../../3Drotation.parameter > 3Drotation_variation.txt

 gather-study-data.py -v 3Drotation_variation.txt -f 3Drotation_gathered ../../study_case_mesh_BC_div_source_h/3Drotation_hex_BC-div-source-h_00000_3Drotation/leiaLevelSetFoam.csv 

 convergencerates.py 3Drotation_gathered.csv 

