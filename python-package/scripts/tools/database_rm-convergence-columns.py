#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import pandas as pd
import os.path
from leia import studycsv
import re

"""
"""

app_description = \
"""
Removes deprecated convergence columns 'O(*)' and 'O_LOCAL(*)'.
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
    
    args = parser.parse_args()
    #---------------------------------------------------------------------------


    study_df = pd.read_csv(args.database, header=[0,1])
    
    study_df.drop(inplace=True, columns= filter(lambda col: bool(re.match('O[(_].*', col[1])), study_df.columns))
    
    if args.inplace:
        savefile = args.database
    elif args.file:
        savefile = args.file
    else:
        raise RuntimeError('No option for saving.')

    study_df.to_csv(savefile, index=False)



if __name__ == "__main__":
    main()
