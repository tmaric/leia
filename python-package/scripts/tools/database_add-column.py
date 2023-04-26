#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
from subprocess import run
from shlex import quote
import pandas as pd
import datetime
import time
import os.path
import random
from leia import database

"""
"""

app_description = \
"""
Inserts a column with a fillvalue to an existing database-CSV file.
Inserts at 0 index relative to the 1-level group membership.
"""
def index_0_rel_group(columns_mi, lvl1_label) -> int:
    return list(columns_mi.get_loc_level(lvl1_label)[0]).index(True)

def main():

    #---- Command line arguments ----------------------------------------------
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    
    parser.add_argument("database",
                        help="Database-CSV file with 2-level columns.")

    file_group = parser.add_mutually_exclusive_group(required=True)

    file_group.add_argument('-i', '--inplace', 
                            action='store_true',
                            help="Save sorted CSV inplace."
                            )

    file_group.add_argument('-f', '--file',
                            help="Save sorted CSV to file",
                            dest='file'
                            )
    
    idx_group = parser.add_mutually_exclusive_group(required=False)
    idx_group.add_argument('--idx', 
                        help="Index where to insert the column. Index starts from zero.",
                        type=int,
                        )
    idx_group.add_argument('--same-idx', 
                            help="Index where to insert the column. Index starts from zero.",
                            dest='ref_file'
                            )

    parser.add_argument('--add',
                       help="Expects 3 parameters: <1-lvl column name> <2-lvl column name> <fillvalue>",
                       required=True,
                       action='append',
                       nargs=3, 
                                             
                       )

    args = parser.parse_args()
    #---------------------------------------------------------------------------

    newcolumns = args.add

    database_df = pd.read_csv(args.database, header=[0,1])

    if args.ref_file:
        ref_df = pd.read_csv(args.ref_file, header=[0,1])  
        ref_columns_ls = list(ref_df.columns)

    columns_mi = database_df.columns

    for newcolumn in newcolumns:
        lvl1_label = newcolumn[0]
        lvl2_label = newcolumn[1]
        fillvalue = newcolumn[2]

        if args.idx:
            insert_idx = args.idx
        elif args.ref_file:
            insert_idx = ref_columns_ls.index((lvl1_label, lvl2_label))
        else:
            # index of first occurence of 1-level label
            insert_idx = index_0_rel_group(columns_mi, lvl1_label)
        
        database_df.insert(insert_idx, (lvl1_label, lvl2_label), fillvalue)
    
    if args.inplace:
        savefile = args.database
    elif args.file:
        savefile = args.file
    else:
        raise RuntimeError('No option for saving.')

    database_df.to_csv(savefile, index=False)



if __name__ == "__main__":
    main()

