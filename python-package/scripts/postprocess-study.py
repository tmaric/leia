#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
from subprocess import run
from shlex import quote
import pandas as pd
import os.path
import warnings
from leia import database

app_description = \
"""
Use this script to merge and concatenate case specific postprocessing CSV data into one large CSV file.
It also computes and add convergencerates of all error properties according to the called script `convergencerates.py`.

Summary:
- merges leiaLevelSetFoam.csv, gradPsierror.csv in all study cases into merged.csv
- gather-study-data.py of all cases related to a pyFoam variation study
- calcs convergencerates and add them as columns if 'N_CELLS' was a study parameter
- concatenates the DataFrame to an existing database CSV or creates a new one"

Note:
Call this script from the directory where the study cases are.
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
         

def main():

    #---- Command line arguments ----------------------------------------------
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    parser.add_argument("variationfile",
                        help="Variationfile where pyFoam stores mapping of study variation number to parameter vector.",
                        )

    parser.add_argument("-l", "--casesfile",
                        help="Casesfile where each case basename is stored in a separated line.",
                        required=True,
                        dest="casesfile")
    
    parser.add_argument("--skip-convergence",
                    action='store_true',
                    help="Skipping calculation of convergence columns.",
                    required=False,
                    )


    parser.add_argument("-s", "--study-csv",
                        help="Study-CSV file where the whole study postprocessing data will be stored. Convergencerates will be added as columns.",
                        required=False,
                        default="tmp.csv",
                        dest="study_csv")

    parser.add_argument("-d", "--database-csv",
                        help="Path to the postprocessing CSV file in a case instance of the study, like `3Ddeformation_study_00000_3Ddeformation/leiaLevelSetFoam.csv`",
                        required=False,
                        dest="database_csv")

    args = parser.parse_args()

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
             

    #---------------------------------------------------------------------------

    merge_csv = ['leiaLevelSetFoam.csv', 'gradPsiError.csv', 'TVerror.csv']

    print("Merge all postprocessing CSV in all concrete cases.")
    merged_csv_str = merge_csv_in_cases(args.casesfile, merge_csv)


    with open(args.casesfile, 'r', encoding='utf-8') as file:
            firstcase = file.readline().removesuffix('\n')
    instance_case_csv = os.path.join(firstcase, merged_csv_str)

    print("Gather merged.csv from concrete cases and assemble one big CSV.")
    cmd_str_gather = f"gather-study-data.py -v {quote(args.variationfile)} -f {quote(args.study_csv.removesuffix('.csv'))} {instance_case_csv}"
    run(cmd_str_gather, check=True, shell=True)

    print("Sort big CSV according to case folder names and TIME.")
    cmd_str_sort = f"sort-csv.py -i {quote(args.study_csv)}"
    run(cmd_str_sort, check=True, shell=True)

    refinementparameter = database.get_refinementparameter(pd.read_csv(args.study_csv))
    if not args.skip_convergence and refinementparameter is not None:
        print("Study investigates refinemenet. Calculate convergence rates and add to CSV.")
        cmd_str_conver = f"convergencerates.py --write -r {refinementparameter} {quote(args.study_csv)}"
        run(cmd_str_conver, check=True, shell=True)
    
    print(f"Set up 2-level column structure and concatenate to {args.database_csv}")
    cmd_str_add = f"add-csv-to-database.py {quote(args.study_csv)} {quote(args.database_csv)}"
    run(cmd_str_add, check=True, shell=True)



if __name__ == "__main__":
    main()
