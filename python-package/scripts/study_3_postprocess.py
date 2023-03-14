#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
from subprocess import run
from shlex import quote
import pandas as pd
import os.path
import warnings
from leia import database
import yaml

merge_csv = ['leiaLevelSetFoam.csv', 'gradPsiError.csv', 'TVerror.csv']

app_description = \
f"""
Use this script to merge and concatenate case specific postprocessing CSV data into one large database CSV file.
It also computes and add convergencerates of all error properties according to the called script `convergencerates.py`.

Summary:
- merges {merge_csv} in all study cases into merged.csv
- gather-study-data.py of all cases related to a pyFoam variation study
- calcs convergencerates and add them as columns if 'N_CELLS' was a study parameter
- concatenates the DataFrame to an existing database CSV or creates a new one"

Note:
Call this script from inside the directory where the concrete study cases lie.
"""

def merge_csv_in_cases(casesfile, csv_list):
    with open(casesfile, 'r', encoding='utf-8') as file:
            cases = file.readlines() 
    cases = [ case_str.removesuffix('\n') for case_str in cases]
    
    for case in cases:
        if not os.path.isdir(case):
            warnings.warn(f"Skipping {case}. Is no directory")
            continue
        merged_df = pd.DataFrame(columns=['TIME'])
        for csv in csv_list:
            csv_path = os.path.join(case, csv)
            if not os.path.isfile(csv_path):
                warnings.warn(f"Skipping {csv_path}. Is no file")
                continue
            df = pd.read_csv(os.path.join(case, csv))
            merged_df = pd.merge(merged_df, df, how='outer', on='TIME')
        merged_df.to_csv(os.path.join(case, "merged.csv"), index=False)
    return "merged.csv"

def parse_manual(args):
    if args.database_csv is None:
        cwd = os.path.abspath(os.getcwd())
        parents = cwd.split('/')
        for parent in reversed(parents):
            if parent.startswith("study_"):
                args.database_csv = f"{parent}_database.csv"
                print(f"Using inferred study database name: {args.database_csv}")
                break
        else:
            args.database_csv = f'study_database.csv'
            print(f"Using study database default name: {args.database_csv}")   
    return args

def parse_studydir(args):
    basename_studydir = os.path.basename(os.path.abspath(args.studydir))
    studyinfofile = os.path.join(args.studydir, f"{basename_studydir}.info")
    with open(studyinfofile, 'r') as file:
        info = yaml.safe_load(file)
    args.database_csv   = os.path.join(args.studydir, info["metaname"] + '_database.csv')
    args.casesfile      = os.path.join(args.studydir, info["casesfile"])
    args.variationfile  = os.path.join(args.studydir, info["pyFoam_variationfile"])
    return args

def main():

    #---- Command line arguments ----------------------------------------------
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
    
    subparsers = parser.add_subparsers()

    subparsers_studydir = subparsers.add_parser('studydir', help='Postprocess studies inside a studydir with a .info file.')    
    subparsers_studydir.set_defaults(func=parse_studydir)
    subparsers_studydir.add_argument("studydir",
                        help="Provide the study directory with the .info yaml file inside it to fetch all meta data.",
                        metavar='STUDYDIR',
                        )
        
    subparsers_manual = subparsers.add_parser('manual', help='Postprocess studies inside a studydir and provide all meta info manual.')
    subparsers_manual.set_defaults(func=parse_manual)
    subparsers_manual.add_argument("casesfile",
                        help="Casesfile where each case basename is stored in a separated line.",
                        )
    subparsers_manual.add_argument("variationfile",
                        help="Variationfile where pyFoam stores mapping of study variation number to parameter vector.",
                        )

    args = parser.parse_args()
    args = args.func(args)

    study_csv = 'tmp.csv'

             

    #---------------------------------------------------------------------------

    

    print("Merge all postprocessing CSV in all concrete cases.")
    merged_csv_str = merge_csv_in_cases(args.casesfile, merge_csv)


    with open(args.casesfile, 'r', encoding='utf-8') as file:
            firstcase = file.readline().removesuffix('\n')
    instance_case_csv = os.path.join(firstcase, merged_csv_str)

    print("Gather merged.csv from concrete cases and assemble one big CSV.")
    cmd_str_gather = f"gather-study-data.py -v {quote(args.variationfile)} -f {quote(study_csv.removesuffix('.csv'))} {instance_case_csv}"
    run(cmd_str_gather, check=True, shell=True)

    print("Sort big CSV according to case folder names and TIME.")
    cmd_str_sort = f"database_sort.py -i {quote(study_csv)}"
    run(cmd_str_sort, check=True, shell=True)

    refinementparameter = database.get_refinementparameter(pd.read_csv(study_csv))
    if not args.skip_convergence and refinementparameter is not None:
        print("Study investigates refinemenet. Calculate convergence rates and add to CSV.")
        cmd_str_conver = f"convergencerates.py --write -r {refinementparameter} {quote(study_csv)}"
        run(cmd_str_conver, check=True, shell=True)
    
    print(f"Set up 2-level column structure and concatenate to {args.database_csv}")
    cmd_str_add = f"add-csv-to-database.py {quote(study_csv)} {quote(args.database_csv)}"
    run(cmd_str_add, check=True, shell=True)



if __name__ == "__main__":
    main()
