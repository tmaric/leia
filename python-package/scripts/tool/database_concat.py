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
Concatenates multiple database-CSV file into one.
Performs a union with all columns.
"""


def main():

    #---- Command line arguments ----------------------------------------------
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    
    parser.add_argument("database",
                        help="Database-CSV file with 2-level columns.",
                        nargs='*'
                        )
    parser.add_argument('-f', '--file',
                       help="Save concatenated database to file",
                       dest='file',
                       required=True
                       )


    args = parser.parse_args()
    #---------------------------------------------------------------------------

    paths       =   args.database
    databases   =   [ pd.read_csv(path, header=[0,1]) for path in paths]

    #check for empty columns
    # for (path, database_df) in zip(paths, databases):
    #     empty_cols = [col for col in database_df.columns if database_df[col].isnull().all()]
    #     if len(empty_cols) != 0:
    #         raise RuntimeError(f"{path} has empty columns {empty_cols}.\nFill or delete columns.")
        
    # check for equal columns and order
    reference_idx = databases[0].columns
    for path, database_df in zip(paths, databases):
        current_idx = database_df.columns
        unequal_columns = [(i,j) for i, j in zip(reference_idx, current_idx) if i != j]
        if len(unequal_columns) > 0:
            curr_cols = list(zip(*unequal_columns))[1]
            ref_cols = list(zip(*unequal_columns))[0]
            raise RuntimeError(
                f"{path} column {curr_cols[0]} does not match with column {ref_cols[0]} of {paths[0]}.")

    database_df = pd.concat(
                    databases,
                    axis=0,
                    join="outer",
                    ignore_index=True,
                    keys=None,
                    levels=None,
                    names=None,
                    verify_integrity=False,
                    copy=False,
                )


    database_df.to_csv(args.file, index=False)



if __name__ == "__main__":
    main()

