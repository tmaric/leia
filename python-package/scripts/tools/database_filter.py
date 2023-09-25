#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import pandas as pd
import os.path
from leia import studycsv
import leia

"""
"""

app_description = \
"""
Script to filter out rows or columns from a study database.
"""
def main():

    #---- Command line arguments ----------------------------------------------
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    
    parser.add_argument("database",
                        help="Database-CSV file with 2-level columns.")

    file_group = parser.add_mutually_exclusive_group(required=True)

    file_group.add_argument('-i', '--inplace', 
                            action='store_true',
                            help="Save CSV inplace."
                            )

    file_group.add_argument('-f', '--file',
                            help="Save CSV to file",
                            dest='file'
                            )
    
    # idx_group = parser.add_mutually_exclusive_group(required=True)
    # idx_group.add_argument('--rm',
    #                    help="Removes all rows matching the value. Expects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
    #                    action='append',
    #                    nargs=3,                   
    #                    )
    
    # idx_group.add_argument('-k','--keep', 
    #                    help="Removes all rows not matching the value. Expects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
    #                    nargs=3, 
    #                     )
    
    # idx_group.add_argument('-d','--drop', 
    #                 help="Drop whole column if values are unique. Expects 3 parameters: <1-lvl column name> <2-lvl column name>",
    #                 nargs=2, 
    #                 )

    # idx_group.add_argument('-df','--drop-force', 
    #                 help="Drop whole column. Expects 3 parameters: <1-lvl column name> <2-lvl column name>",
    #                 nargs=2, 
    #                 )

    filterfile_group = parser.add_mutually_exclusive_group(required=False)
    filterfile_group.add_argument('--rm-file',
                       help="Removes all cases listed in file. Accepts JSON or list",
                       )

    filterfile_group.add_argument('--keep-file',
                       help="Keep all cases listed in file. Accepts JSON or list",
                       )

    parser.add_argument('-r','--rm',
                       help="Removes all rows matching the value. Expects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
                       action='append',
                       nargs=3,                   
                       )
    
    parser.add_argument('-k','--keep', 
                       help="Removes all rows not matching the value. Expects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
                       action='append',
                       nargs=3, 
                        )
    
    parser.add_argument('-K','--keep-drop', 
                       help="Removes all rows not matching the value and drops the column. Expects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
                       action='append',
                       nargs=3,
                       dest='keepdrop' 
                        )

    args = parser.parse_args()
    #---------------------------------------------------------------------------


    study_df = pd.read_csv(args.database, header=[0,1])

    def column(ls: list):
        return (ls[0], ls[1])

    # if args.rm:
    #     if isinstance(args.rm[0], list): # then multiple --rm are passed
    #         rms = args.rm
    #     else:
    #         rms = [args.rm]
    #     for rm in rms: 
    #         database_df = studycsv.filter_rm(database_df, column(rm), rm[2])
    # elif args.keep:
    #     database_df = studycsv.filter_keep(database_df, column(args.keep), args.keep[2])
    # elif args.drop:
    #     database_df = studycsv.filter_drop(database_df, column(args.drop))
    # elif args.drop_force:
    #     database_df = studycsv.filter_drop(database_df, column(args.drop_force), force=True)
    # else:
    #     raise RuntimeError("No option for action.")

    def group_list_of_columns_values(lst):
        lst = [tuple(ls) for ls in lst]
        grouped_dict = {}
        for item in lst:
            key = item[:2]  # Take the first two items as the key
            if key in grouped_dict:
                grouped_dict[key].append(item[2])
            else:
                grouped_dict[key] = [item[2]]
        return grouped_dict

    if args.rm:
        rms = group_list_of_columns_values(args.rm)
        for column, values in rms.items():  
            study_df = studycsv.filter_rm(study_df, column, values)
    if args.keep:
        keeps = group_list_of_columns_values(args.keep)
        for column, values in keeps.items(): 
            study_df = studycsv.filter_keep(study_df, column, values, drop=False)
    if args.keepdrop:
        keeps = group_list_of_columns_values(args.keepdrop)
        for column, values in keeps.items(): 
            study_df = studycsv.filter_keep(study_df, column, values, drop=True)
            

    if args.rm_file:
        cases = leia.io.read_cases(args.rm_file)
        study_df = studycsv.filter_cases(study_df, cases, mode='rm')
    if args.keep_file:
        cases = leia.io.read_cases(args.keep_file)
        study_df = studycsv.filter_cases(study_df, cases, mode='keep')

    study_df.reset_index(drop=True)

    
    if args.inplace:
        savefile = args.database
    elif args.file:
        savefile = args.file
    else:
        raise RuntimeError('No option for saving.')

    study_df.to_csv(savefile, index=False)



if __name__ == "__main__":
    main()

