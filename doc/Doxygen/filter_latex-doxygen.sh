#!/usr/bin/env bash

# This scripts filters latex code into doxygen code.
# Currently, it translates the syntax of math and other environments into doxygen compliant syntax.
# \verb, \gls commands are deleted and their content inserted as plain text.
# Usage:
#		1. Insert your latex text into the doxygen description of your source code.
# 	2. Put this script in to your PATH and use the VS Code extension 'Filter Text' to call this script on the selected doxygen text. 

readonly SCRIPT_PATH=$(realpath "${BASH_SOURCE[0]}")
readonly DIR_PATH="$(dirname $SCRIPT_PATH)"

## latex -> doxygen
#sed 's/\$/\\f$/g' 														# replace inline math environment
#sed 's/\\begin{\([a-z*]*\)}/\\f{\1}{/' 			# replace begin of some environment
#sed 's/\\end{\([a-z*]*\)}/\\f}/' 						# replace end of some environment
#sed '/^%$/d' 																# delete comment lines
#sed 's/%.*$//' 															# delete comments at the end of a line
#sed 's/\\\(gls\){\([a-zA-Z]*\)}/\2/g' 				# delete \gls command, but not its content
#sed 's/\\\(verb\)|\([a-zA-Z]*\)|/\2/g' 			# delete \verb command, but not its content
#sed 's/(\\ref{eq.*})//g'											# delete \ref equations
#sed 's/\\cite{\([a-zA-Z0-9_]*\)}/\\cite \1/'	# replace \cite command

## final command, as concatenation
if [[ ! -t 0 ]]; then # stdin is provided

sed '
s/\$/\\f$/g;
s/\\begin{\(equation\|verbatim\)}/\\f{\1}{/;
s/\\end{\(equation\|verbatim\)}/\\f}/;
/^%/d;
s/%.*$//;
s/\\\(gls\){\([a-zA-Z]*\)}/\2/g;
s/\\\(verb\)|\([^|]*\)|/\2/g;
s/(\\ref{eq.*})//g;
s/\\cite{\([a-zA-Z0-9_]*\)}/\\cite \1/
' <&0

else
	echo "Provide STDIN."
fi

### final command, as concatenation
#if [[ $# -ge 1 ]]; then # input file is provided
#elif [[ ! -t 0 ]]; then # stdin is provided
#else
#	echo "Provide a INPUTFILE or STDIN."
#fi
