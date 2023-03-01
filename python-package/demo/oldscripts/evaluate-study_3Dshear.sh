#! /usr/bin/env bash

set -v

 pyFoamRunParameterVariation.py --list-variations ../../3Dshear ../../3Dshear.parameter > 3Dshear_variation.txt

 gather-study-data.py -v 3Dshear_variation.txt -f 3Dshear_gathered ../../study_case_mesh_BC_div_source_h/3Dshear_hex_BC-div-source-h_00000_3Dshear/leiaLevelSetFoam.csv 

 convergencerates.py 3Dshear_gathered.csv 

