#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
from subprocess import run
from shlex import quote
import pandas as pd
import os.path
import warnings
import leia


app_description = \
f"""
Script merges and concatenates case specific CSV files into one large study database CSV file.
Furthermore, latest time folders of all cases are listed.

Case specific CSV files:
{leia.studydir.CASE_CSVs}

Note:
Run this script from within the directory where the actual study cases reside.
"""


def parse_studydir(args):
    basename_studydir = os.path.basename(os.path.abspath(args.studydir))
    studyinfofile = os.path.join(args.studydir, f"{basename_studydir}.info")
    info = leia.io.read_info(studyinfofile)
    args.database_csv   = os.path.join(args.studydir, info["metaname"] + '_database.csv')
    args.casesfile      = os.path.join(args.studydir, info["casesfile"])
    args.variationfile  = os.path.join(args.studydir, info["pyFoam_variationfile"])
    args.endTimesfile   = os.path.join(args.studydir, info["metaname"] + '.latestTimes.txt')
    return args

def append_info(args):
    basename_studydir = os.path.basename(os.path.abspath(args.studydir))
    studyinfofile = os.path.join(args.studydir, f"{basename_studydir}.info")
    info = leia.io.read_info(studyinfofile)
    info['studydatabase.csv'] = args.database_csv
    info['endtimesfile'] = args.endTimesfile
    leia.io.write_info(info, studyinfofile)


def parse_arguments():
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    # parser.add_argument("--skip-convergence",
    #                 action='store_true',
    #                 help="Skip calculation of convergence rates",
    #                 required=False,
    #                 )
    parser.add_argument("-n", "--name",
                        help="Provide a different database CSV file name. Default: <STUDYDIR>_database.csv",
                        required=False,
                        dest="database_csv",
                        metavar='DATABASE')

    parser.add_argument("studydir",  
                    help="Study directory with the template case, parameter file and the info file inside.",
                    metavar='STUDYDIR')

    args = parser.parse_args()

    return parse_studydir(args)

def main():
    args = parse_arguments()

    print(f"Save latestTimes of concrete cases to {args.endTimesfile}")
    cmd_str_endTimes = f"study_print-latestTime.py {args.studydir} > {args.endTimesfile}"
    run(cmd_str_endTimes, check=True, shell=True)

    print(f"Agglomerate {leia.studydir.CASE_CSVs} of all cases with metadata and studyparameters into {args.database_csv}")
    cmd_str_agglo = f"study_agglomerate-database.py {args.studydir} {args.database_csv}"
    run(cmd_str_agglo, check=True, shell=True)

    append_info(args)

    # refinementparameter = leia.studycsv.get_refinementlabel(pd.read_csv(args.database_csv, header=[0,1]))
    # if not args.skip_convergence and refinementparameter is not None:
    #     print(f"Study investigates refinemenet. Calculate convergence rates and add them to {args.database_csv}")
    #     cmd_str_conver = f"database_add-convergence.py --inplace --refinement-parameter {refinementparameter[1]} {args.database_csv}"
    #     run(cmd_str_conver, check=True, shell=True)



if __name__ == "__main__":
    main()
