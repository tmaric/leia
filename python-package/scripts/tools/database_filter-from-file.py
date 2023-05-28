#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import pandas as pd
import os.path
import leia

"""
"""

app_description = \
"""
Script to filter out cases listed in a file from a database.
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
    
    filterfile_group = parser.add_mutually_exclusive_group(required=False)
    filterfile_group.add_argument('--rm-file',
                       help="Removes all cases listed in file. Accepts JSON or list",
                       )

    filterfile_group.add_argument('--keep-file',
                       help="Keep all cases listed in file. Accepts JSON or list",
                       )

    args = parser.parse_args()
    #---------------------------------------------------------------------------


    study_df = pd.read_csv(args.database, header=[0,1])


    if args.rm_file:
        cases = leia.io.read_cases(args.rm_file)
        study_df = leia.studycsv.filter_cases(study_df, cases, mode='rm')
    if args.keep_file:
        cases = leia.io.read_cases(args.keep_file)
        study_df = leia.studycsv.filter_cases(study_df, cases, mode='keep')

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

