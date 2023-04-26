#!/usr/bin/env python
# coding: utf-8

from subprocess import run
from shlex import quote
from argparse import ArgumentParser, RawTextHelpFormatter
import sys
import os.path

def get_cases(args):
    # cases from file
    # if args.list is not None and not '-':
    if args.list is not None:
        with open(args.list, 'r', encoding='utf-8') as file:
            cases = file.readlines()  
    # cases list from stdin 
    else:
        cases = sys.stdin.readlines()
    cases = [string.removesuffix("\n") for string in cases]
    cases = [os.path.basename(string) for string in cases]
    cases.sort()
    return cases

def get_parameter_from_pyFoamOutput(pyFoamOutput):
    # parameter = pyFoamOutput.split('\n')
    # data = pyFoamOutput[10:-1]
    parameter = [':' + string.partition(':')[2] for string in pyFoamOutput]
    return parameter

def merge_cases_with_parameter(cases, parameter):
    assert len(parameter) == len(cases), f"Parameter list length {len(parameter)} does not match cases list length {len(cases)}."
    merged = []
    for case, par in zip(cases, parameter):
        merged.append(f"'{case}' " + par + ',')
    return merged

def create(args):
    cases = get_cases(args)
    pyFoam_process = run(f"pyFoamRunParameterVariation.py --list-variation {quote(args.templateCase)} {quote(args.parameterFile)}",
        shell=True,
        capture_output=True,
        encoding='utf-8'
        )
    parameter = get_parameter_from_pyFoamOutput(pyFoam_process.stdout.split('\n')[10:-1])
    output = merge_cases_with_parameter(cases, parameter)
    return output

def convert(args):
    cases = get_cases(args)
    with open(args.pyFoamOutput, 'r', encoding='utf-8') as file:
        pyFoamOutput = file.readlines()[10:]
        pyFoamOutput = [string.removesuffix("\n") for string in pyFoamOutput]
    parameter = get_parameter_from_pyFoamOutput(pyFoamOutput)
    output = merge_cases_with_parameter(cases, parameter)
    return output

app_description = \
"""
Outputs `pyFoamRunParameterVariation.py --list-variation` but with the case path instead of the varation number.
The script reads a list of cases from stdin and merges it with the `pyFoamRunParameterVariation --list-variation` output.
The list of cases uses newline as separator. 
"""

def main():
    #---- Command line arguments ----------------------------------------------
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)
    subparsers = parser.add_subparsers()
    parser.add_argument("-l", "--cases-list",
                        help="Ignore stdin and read a list of cases from a file.",
                        required=False,
                        default=None,
                        dest="list")

    parser_create = subparsers.add_parser('create', help='Creates `pyFoamRunParameterVariation.py --list-variation` output itself and merges it with the cases list.')
    parser_create.set_defaults(func=create)
    parser_create.add_argument("templateCase",
                        help="The templatecase which will be passed to pyFoamRunParameterVariation.py",
                        )
    parser_create.add_argument("parameterFile",
                        help="The parameterfile which will be passed to pyFoamRunParameterVariation.py",
                        )

    parser_convert = subparsers.add_parser('convert', help='Converts existing `pyFoamRunParameterVariation --list-variation` output from a file into the case-variation format.') 
    parser_convert.set_defaults(func=convert)
    parser_convert.add_argument("pyFoamOutput",
                        help="File with `pyFoamRunParameterVariation --list-variation` output",
                        )
    
    args = parser.parse_args()
    output = args.func(args)

    # Python dictionary and json compliant output format
    print("{")
    for linenumber, line in zip(range(len(output)-1), output):
        print(line.replace("'", '"'))
    print(output[-1].replace("'", '"').rstrip(','))
    print("}")


if __name__ == "__main__":
    main()