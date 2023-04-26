#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import pandas as pd
import os.path
from leia import studycsv

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
    
    idx_group = parser.add_mutually_exclusive_group(required=True)
    idx_group.add_argument('--rm',
                       help="Removes all rows matching the value. Expects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
                       action='append',
                       nargs=3,                   
                       )
    
    idx_group.add_argument('--keep', 
                       help="Removes all rows not matching the value. Expects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
                       nargs=3, 
                        )
    
    idx_group.add_argument('--drop', 
                    help="Drop whole column if values are unique. Expects 3 parameters: <1-lvl column name> <2-lvl column name>",
                    nargs=2, 
                    )

    args = parser.parse_args()
    #---------------------------------------------------------------------------


    database_df = pd.read_csv(args.database, header=[0,1])

    def column(ls: list):
        return (ls[0], ls[1])

    if args.rm:
        if isinstance(args.rm[0], list): # then multiple --rm are passed
            rms = args.rm
        else:
            rms = [args.rm]
        for rm in rms: 
            database_df = studycsv.filter_rm(database_df, column(rm), rm[2])
    elif args.keep:
        database_df = studycsv.filter_keep(database_df, column(args.keep), args.keep[2])
    elif args.drop:
        database_df = studycsv.filter_drop(database_df, column(args.drop))
    else:
        raise RuntimeError("No option for action.")

    
    if args.inplace:
        savefile = args.database
    elif args.file:
        savefile = args.file
    else:
        raise RuntimeError('No option for saving.')

    database_df.to_csv(savefile, index=False)



if __name__ == "__main__":
    main()

