#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import pandas as pd
import os.path
import warnings
import leia
import yaml
import json

app_description = \
f"""
Script merges and concatenates case specific {leia.studydir.CASE_CSVs} CSV files into one large database CSV file.

Note:
Run this script from within the directory where the actual study cases reside.
"""

def merge_time_csv(CSVs):
    merged_df = pd.DataFrame(columns=['TIME'])
    for csv in CSVs:
        try:
            df = pd.read_csv(os.path.join(csv))
            if 'time' in df.columns:
                df.rename(columns={"time": "TIME"}, inplace=True)
        except (FileNotFoundError, pd.errors.EmptyDataError):
            warnings.warn(f"Skipping {csv}. No or empty file.")
            continue
        merged_df = pd.merge_ordered(merged_df, df, how='outer', on='TIME')
    return merged_df


def agglomerate_study_data(args, cases, CSVs):
    with open(args.json_variationfile, 'r') as f:
        case_studyparameters_dict = json.load(f)

    if not all(case in case_studyparameters_dict for case in cases):
        raise RuntimeError(f"{args.json_variationfile} does not contain all cases.")

    cases_df_ls = []
    
    for case in cases:
        studyparam_value_dict = case_studyparameters_dict[case]
        databasecolumn_value_dict = leia.studycsv.get_database_parameter_for_case(
                args.studydir, 
                case, 
                columnlevel=1
            )
        
        merged_csv_df       = merge_time_csv([os.path.join(case, csv) for csv in CSVs])
        database_df         = pd.DataFrame(databasecolumn_value_dict, index=range(merged_csv_df.shape[0]))
        studyparameters_df  = pd.DataFrame(studyparam_value_dict, index=range(merged_csv_df.shape[0]))

        if not args.flat_columns:
            database_df.columns         = pd.MultiIndex.from_product([['database'], database_df.columns])
            studyparameters_df.columns  = pd.MultiIndex.from_product([['studyparameters'], studyparameters_df.columns])
            merged_csv_df.columns       = pd.MultiIndex.from_product([['case'], merged_csv_df.columns])
            
        case_df = pd.concat([database_df, studyparameters_df, merged_csv_df], axis='columns', copy=False)
        cases_df_ls.append(case_df)

    agglomerated_df = pd.concat(cases_df_ls, axis='index', copy=False)
    return agglomerated_df


def parse_arguments():
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)
    parser.add_argument("studydir",  
                    help="Study directory with the templatecase, parameter file and the info file inside.",
                    metavar='STUDYDIR')
    
    parser.add_argument("studyCSV",
                    help="File where aggolomerated DataFrame will be stored.",
                    metavar='DATABASE'
                    )
    parser.add_argument("--flat-columns",
                    help="Provide if you don't want the 2-level columns structure, just one column level. ",
                    action='store_true',
                    required=False
                    )

    args = parser.parse_args()
    return args


def main():
    CSVs = leia.studydir.CASE_CSVs

    args = parse_arguments()

    basename_studydir = os.path.basename(os.path.abspath(args.studydir))
    args.infofile = os.path.join(args.studydir, f"{basename_studydir}.info")
    with open(args.infofile, 'r') as file:
        info = yaml.safe_load(file)

    args.metaname           = info['metaname']
    args.studyname          = info['studyname']
    args.templatecase       = os.path.join(args.studydir, info['templatecase'])
    args.json_variationfile = os.path.join(args.studydir, info['json_variationfile'])   
    args.casesfile          = os.path.join(args.studydir, info["casesfile"])

    cases = leia.studydir.cases_ls(args.casesfile)
    cases = leia.studydir.filter_existing_cases(cases)
    study_df = agglomerate_study_data(args, cases, CSVs)

    if args.studyCSV is not None:
        studyCSV = args.studyCSV
    else:
        studyCSV = os.path.join(args.studydir, args.metaname + '_1lvlcol.csv')
    study_df.to_csv(studyCSV, index=False)


if __name__ == "__main__":
    main()