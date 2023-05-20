#! /usr/bin/env bash

# If changes were made in ./Macros, call this script to copy the latex packages into your local environment. 
# Otherwise doxygen might not be able to render latex formulas.

latex -v > /dev/null 2>&1 # check if tex live is installed
if [[ $? -eq 0 ]]; then
	pkg_path="$(kpsewhich -var-value=TEXMFHOME)/tex/latex/commonstuff/"
	mkdir -p $pkg_path
	cp -av ./Macros/* $pkg_path
else
	echo "Could not find path to local latex packages."
fi
