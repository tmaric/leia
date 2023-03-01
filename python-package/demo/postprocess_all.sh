#! /bin/bash
set -e
./postprocess_3Ddeformation.sh*
./postprocess_3Drotation.sh*
./postprocess_3Dshear.sh*
./postprocess_3Dtranslation.sh*

