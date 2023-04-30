#! /usr/bin/env bash

# If changes were made in ./Macros, call this script to copy the latex packages into your local environment. 
# Otherwise doxygen might not be able to render latex formulas.

latex -v > /dev/null 2>&1 # check if tex live is installed
if [[ $? -eq 0 ]]; then
	mkdir -p $HOME/texmf/tex/latex/commonstuff
	cp -av ./Macros/* $HOME/texmf/tex/latex/commonstuff/
else
	echo "Could not find path to local latex packages."
fi
