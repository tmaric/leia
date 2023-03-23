#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
from subprocess import run
from shlex import quote
import pandas as pd
import os.path
import warnings
import leia
import yaml


app_description = \
f"""
Use this script to merge and concatenate case specific postprocessing CSV data into one large database CSV file.
It also computes and add convergencerates of all error properties.

Summary:
- list endTimes of all cases into file.
- agglomerate {leia.studydir.CASE_CSVs} in all study cases to one study CSV file, which has 2-level columns structure.
- If study investigate refinement, also convergencerates are calculated and added to the CSV

Note:
Call this script from inside the directory where the concrete study cases lie.
"""

# def parse_manual(args):
#     args.studydir = '.'
#     if args.database_csv is None:
#         cwd = os.path.abspath(os.getcwd())
#         parents = cwd.split('/')
#         for parent in reversed(parents):
#             if parent.startswith("study_"):
#                 args.database_csv = f"{parent}_database.csv"
#                 print(f"Using inferred study database name: {args.database_csv}")
#                 break
#         else:
#             args.database_csv = f'study_database.csv'
#             print(f"Using study database default name: {args.database_csv}")   
#     return args

def parse_studydir(args):
    basename_studydir = os.path.basename(os.path.abspath(args.studydir))
    studyinfofile = os.path.join(args.studydir, f"{basename_studydir}.info")
    with open(studyinfofile, 'r') as file:
        info = yaml.safe_load(file)
    args.database_csv   = os.path.join(args.studydir, info["metaname"] + '_database.csv')
    args.casesfile      = os.path.join(args.studydir, info["casesfile"])
    args.variationfile  = os.path.join(args.studydir, info["pyFoam_variationfile"])
    args.endTimesfile   = os.path.join(args.studydir, info["metaname"] + '.latestTimes.txt')
    return args

def parse_arguments():
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)
    parser.add_argument("--skip-convergence",
                    action='store_true',
                    help="Skipping calculation of convergence columns.",
                    required=False,
                    )
    parser.add_argument("-n", "--name",
                        help="Provide a different database CSV file name.",
                        required=False,
                        dest="database_csv")

    parser.add_argument("studydir",
                        help="Provide the study directory with the .info yaml file inside it to fetch all meta data.",
                        metavar='STUDYDIR',
                        )

    # subparsers = parser.add_subparsers()

    # subparsers_studydir = subparsers.add_parser('studydir', help='Postprocess studies inside a studydir with a .info file.')    
    # subparsers_studydir.set_defaults(func=parse_studydir)
    # subparsers_studydir.add_argument("studydir",
    #                     help="Provide the study directory with the .info yaml file inside it to fetch all meta data.",
    #                     metavar='STUDYDIR',
    #                     )
        
    # subparsers_manual = subparsers.add_parser('manual', help='Postprocess studies inside a studydir and provide all meta info manual.')
    # subparsers_manual.set_defaults(func=parse_manual)
    # subparsers_manual.add_argument("casesfile",
    #                     help="Casesfile where each case basename is stored in a separated line.",
    #                     )
    # subparsers_manual.add_argument("variationfile",
    #                     help="Variationfile where pyFoam stores mapping of study variation number to parameter vector.",
    #                     )

    args = parser.parse_args()
    
    # return args.func(args)
    return parse_studydir(args)

def main():
    args = parse_arguments()

    print(f"Save latestTimes of concrete cases to {args.endTimesfile}")
    cmd_str_endTimes = f"study_print-latestTimes.sh {args.studydir} > {args.endTimesfile}"
    run(cmd_str_endTimes, check=True, shell=True)

    print(f"Agglomerate {leia.studydir.CASE_CSVs} of all cases with metadata and studyparameters into {args.database_csv}")
    cmd_str_agglo = f"study_agglomerate-database.py {args.studydir} {args.database_csv}"
    run(cmd_str_agglo, check=True, shell=True)

    refinementparameter = leia.studycsv.get_refinementlabel(pd.read_csv(args.database_csv, header=[0,1]))
    if not args.skip_convergence and refinementparameter is not None:
        print(f"Study investigates refinemenet. Calculate convergence rates and add them to {args.database_csv}")
        cmd_str_conver = f"database_add-convergence.py --inplace --refinement-parameter {refinementparameter[1]} {args.database_csv}"
        run(cmd_str_conver, check=True, shell=True)



if __name__ == "__main__":
    main()
