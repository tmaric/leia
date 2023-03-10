#!/usr/bin/env python 

from argparse import ArgumentParser, RawTextHelpFormatter
import sys
from subprocess import call, run
from shlex import quote
import os
import os.path
from glob import glob
import yaml
from leia.studycsv import time_format
import time

usage = """
A wrapper for pyFoamRunParameterVariation.py.
Script takes the study directory with the templatecase and parameter files inside it and
and generates concrete simulation cases and some meta files. 

Meshes are not generated and preprocessing is not done. 
Used to prepare for execution on a cluster.

Usage: ./study_1_init.py STUDYDIR"""

def parse_arguments():
    parser = ArgumentParser(description=usage, formatter_class=RawTextHelpFormatter)

    # parser.add_argument('-m', '--mesh',
    #                     choices=['hex', 'perturbed', 'poly'],
    #                     help="Meshtype: hexahedral, hexahedral-perturbed or polyhedral",
    #                     default='hex',
    #                     required=False,
    #                     dest='meshtype',
    #                     )

    parser.add_argument("studydir",  
                    help="Directory with the templatecase and parameterfile inside.",
                    metavar='STUDYDIR')
    
    return parser.parse_args()

    
def create_parameterstudy(args):
    run([
        "pyFoamRunParameterVariation.py", "--no-execute-solver", "--no-server-process", 
        "--no-mesh-create", "--no-case-setup", "--cloned-case-prefix=%s" % args.studyname,
        "--parameter-file=default.parameter",
        "--every-variant-one-case-execution",
        "--create-database", args.casedir, args.paramfile
        ],
        check=True
    )

def create_pyFoam_variationfile(args):
    pyFoamVariation_file = args.metaname + '.pyFoam-variation'
    run(f"pyFoamRunParameterVariation.py --list-variation {quote(args.casedir)} {quote(args.paramfile)} > {pyFoamVariation_file}",
        shell=True, check=True
        )
    args.pyFoam_variationfile = pyFoamVariation_file
    return pyFoamVariation_file

def create_json_variationfile(args, casesfile, pyfoamvariationfile):
    json_variationfile = args.metaname + '.json'
    run(f"study_case-parameter.py -l {casesfile} convert {pyfoamvariationfile} > {json_variationfile}",
        shell=True)
    args.json_variationfile = json_variationfile
    return json_variationfile

    
def create_concrete_casesfile(args):
    casesfile = args.metaname + '.cases'
    run(f"find -maxdepth 1 -type d -name '{args.studyname}_[0-9][0-9][0-9][0-9][0-9]*' | sort | xargs -n1 basename > {casesfile}",
        shell=True, check=True)
    args.casesfile = casesfile
    return casesfile

def add_to_infofile(args):
    infofile = os.path.basename(args.studydir + '.info')
    cmd = f"""(
            echo "casesfile: {args.casesfile}";
            echo "pyFoam_variationfile: {args.pyFoam_variationfile}";
            echo "json_variationfile: {args.json_variationfile}";
            echo "inittime: {time.strftime(time_format)}";
            echo "";
        ) >> {infofile}"""
    run(cmd, shell=True, check=True)


# def get_concrete_cases(casesfile) -> list[str]:
#     with open(casesfile, 'r', encoding='utf-8') as file:
#         cases_ls = file.readlines()
#     return cases_ls
    
def cp_extrafiles_to_concrete_cases(args, extrafiles, casesfile):
    # extrafiles = [
    #     '*.stl',
    #     'Allrun*',
    # ]
    extrafiles = [os.path.join(args.casedir, file) for file in extrafiles]
    extrafiles_str = ' '.join(extrafiles)
    cmd = f"""
    cat {casesfile} | xargs -I[] bash -c 'cp {extrafiles_str} []/ 2>/dev/null'
    """
    run(cmd, shell=True)
    

def main():
    args = parse_arguments()

    basename_studydir = os.path.basename(os.path.abspath(args.studydir))
    studyinfofile = os.path.join(args.studydir, f"{basename_studydir}.info")
    with open(studyinfofile, 'r') as file:
        info = yaml.safe_load(file)

    args.metaname = info['metaname']
    args.studyname = info['studyname']
    args.casedir = info['templatecase']
    args.paramfile = info['parameterfile']

    os.chdir(args.studydir)

    # Generate parameter study simulation cases with pyFoam
    create_parameterstudy(args)

    # Creates file which lists all concrete cases
    casesfile = create_concrete_casesfile(args)
    print(f"Created {casesfile}")

    extrafiles = [
        '*.stl',
        'Allrun*.sbatch',
    ]
    print(f"Copy extrafiles into cases.")
    cp_extrafiles_to_concrete_cases(args, extrafiles, casesfile)
    
    # Creates file with almost dictionary structure, connection the variation number to the parameter vector.
    pyFoam_variationfile = create_pyFoam_variationfile(args)
    print(f"Created {pyFoam_variationfile}")
    
    # Creates file similiar to the pyFoamVariation_file, but with case basenames and in json format. Readable by python.
    json_variationfile = create_json_variationfile(args, casesfile, pyFoam_variationfile)
    print(f"Created {json_variationfile}")

    add_to_infofile(args)


if __name__ == "__main__":
    main()