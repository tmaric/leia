#! /usr/bin/env bash

set -v

 pyFoamRunParameterVariation.py --list-variations ../../3Dtranslation ../../3Dtranslation.parameter > 3Dtranslation_variation.txt

 gather-study-data.py -v 3Dtranslation_variation.txt -f 3Dtranslation_gathered ../../study_case_mesh_BC_div_source_h/3Dtranslation_hex_BC-div-source-h_00000_3Dtranslation/leiaLevelSetFoam.csv 

 convergencerates.py --write 3Dtranslation_gathered.csv 

 add-csv-to-database.py 3Dtranslation_gathered.csv database.csv
