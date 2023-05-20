#!/usr/bin/env python 

from argparse import ArgumentParser, RawTextHelpFormatter
import sys
from subprocess import call, run
from shlex import quote
import os
import os.path
import time
from leia.studycsv import time_format

usage = """
Creates the directory study_<STUDYNAME> for a pyFoam study, copies the template case, parameter files into it and 
creates the study_<STUDYNAME>.info file with some meta data in it.
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

    parser.add_argument("-c", "--case", 
                    dest="case",
                    help="Template case", 
                    required=True,
                    metavar="CASE")

    parser.add_argument("-p", "--parameter-file", 
                    dest="paramfile", 
                    help="PyFoam parameter file", 
                    required=True,
                    metavar="PARAMFILE")

    parser.add_argument("-s", "--study-name", 
                    dest="studyname", 
                    help="Name of the parameter study", 
                    required=True,
                    metavar="STUDYNAME")
    
    return parser.parse_args()

def create_studydir(studyname: str) -> str:
    studydir = f"study_{studyname}"
    try:
        os.mkdir(studydir)
    except FileExistsError:
        raise FileExistsError(f"Study directoy {studydir} already exists. Choose different studyname.")
    return studydir

def cp_templatecase_paramfile_to_studydir(args):
    cmd = f"""
        cp -a {quote(args.case)} {os.path.join(args.studydir, args.case)};
        cp -a {args.paramfile} default.parameter {args.studydir}/
    """
    run(cmd, shell=True, check=True)
    

def create_infofile(args):
    infofile = os.path.basename(args.studydir + '.info')
    cmd = f"""(
            echo "# YAML";
            echo "";
            echo "studydir: {args.studydir}";
            echo "metaname: {args.metaname}";
            echo "studyname: {args.studydir.removeprefix('study_')}";
            echo "templatecase: {args.case}";
            echo "parameterfile: {args.paramfile}";
            echo "creationtime: {time.strftime(time_format)}";
            echo "";
        ) > {infofile}"""
    run(cmd, shell=True, check=True)

    

def main():
    args = parse_arguments()
    args.studydir = create_studydir(args.studyname)
    args.metaname = os.path.basename(args.studydir)

    # Copies the template case and parameter file into the study. Freezes information of all non-studyparameters to the study.
    cp_templatecase_paramfile_to_studydir(args)

    os.chdir(args.studydir)

    create_infofile(args)
    print(f"Created empty study: {args.studydir}/")


if __name__ == "__main__":
    main()