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
Initialises a parameter study, basically using `pyFoamRunParameterVariation.py`. 
The script takes the study directory STUDYDIR created by `study_0_create.py`
and generates concrete simulation cases and some meta files. 
No meshes are generated, only the pyFoam instantiations with some extra meta data files.
"""


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
                    help="Study directory with the template case, parameter file and the info file inside.",
                    metavar='STUDYDIR')
    
    return parser.parse_args()

    
def create_parameterstudy(args):
    run([
        "pyFoamRunParameterVariation.py", "--no-execute-solver", "--no-server-process", 
        "--no-mesh-create", "--no-case-setup", "--cloned-case-prefix=%s" % args.studyname,
        "--parameter-file=default.parameter",
        "--every-variant-one-case-execution",
        "--create-database", args.case, args.paramfile
        ],
        check=True
    )

def create_pyFoam_variationfile(args):
    pyFoamVariation_file = args.metaname + '.pyFoam-variation'
    run(f"pyFoamRunParameterVariation.py --list-variation {quote(args.case)} {quote(args.paramfile)} > {pyFoamVariation_file}",
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
    cmd = f"""(
            echo "casesfile: {args.casesfile}";
            echo "pyFoam_variationfile: {args.pyFoam_variationfile}";
            echo "json_variationfile: {args.json_variationfile}";
            echo "inittime: {time.strftime(time_format)}";
            echo "";
        ) >> {args.infofile}"""
    run(cmd, shell=True, check=True)

    
def cp_extrafiles_to_concrete_cases(args, extrafiles, casesfile):
    # extrafiles = [
    #     '*.stl',
    #     'Allrun*',
    # ]
    extrafiles = [os.path.join(args.case, file) for file in extrafiles]
    extrafiles_str = ' '.join(extrafiles)
    cmd = f"""
    cat {casesfile} | xargs -I[] bash -c 'cp {extrafiles_str} []/ 2>/dev/null'
    """
    run(cmd, shell=True)
    

def main():
    args = parse_arguments()

    basename_studydir = os.path.basename(os.path.abspath(args.studydir))
    args.infofile = os.path.join(args.studydir, f"{basename_studydir}.info")
    with open(args.infofile, 'r') as file:
        info = yaml.safe_load(file)

    args.metaname = info['metaname']
    args.studyname = info['studyname']
    args.case = info['templatecase']
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