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
Adds a 1-level CSV file to an existing database-CSV file or creates a new database-CSV file.
Makes a union of all columns.
"""


def main():

    #---- Command line arguments ----------------------------------------------
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    parser.add_argument("CSV",
                        help="CSV file with 1-level coumns which should be added to the database-CSV")
    
    parser.add_argument("database",
                        help="Database-CSV file with 2-level columns.")



    args = parser.parse_args()
    #---------------------------------------------------------------------------

    newdata_df = pd.read_csv(args.CSV)
    newdata_columns_read = newdata_df.columns.to_list() # Assumes columns have just one level
    newdata_column_groups = database.groupcolumns(newdata_columns_read)
    newdata_columns_database = newdata_column_groups['database']
    newdata_columns_studyparameters = newdata_column_groups['studyparameters']
    newdata_columns_case = newdata_column_groups['case']


    # reorder columns: database, studyparameters, case
    newdata_df = newdata_df[newdata_columns_database + newdata_columns_studyparameters + newdata_columns_case]

    # Set up Multiindex for 2-level columns
    mi_lv1 = ['database', 'studyparameters', 'case']
    mi_list_lv2 = [newdata_columns_database, newdata_columns_studyparameters, newdata_columns_case]
    mi_list_tuples = []
    for lv1, mi_lv2 in zip(mi_lv1, mi_list_lv2):
        for lv2 in mi_lv2:
            mi_list_tuples.append((lv1, lv2))

    
    mi = pd.MultiIndex.from_tuples(mi_list_tuples, names=('', ''))
    newdata_df.columns = mi


    try:
        database_df = pd.read_csv(args.database, header=[0,1])

        # Check if the first level column names match.
        assert len(mi.levels[0]) == len(mi_lv1) and all(mi.levels[0] == pd.Index(sorted(mi_lv1))), \
            "Read Database does not have necessary lvl 1 columns names."
        # Check if the length of the database columns match
        assert list(zip(*mi))[0].count(mi_lv1[0]) == len(database.database_columns), \
            f"{mi_lv1[0]} does not apear {len(database.database_columns)}-times in MultiIndex columns."
        # Check if the database lvl 2 columns match
        assert set(list(zip(*mi))[1][:len(database.database_columns)]) == set(database.database_columns), \
            f"New CSV assembled {mi_lv1[0]}-lvl-2 columns: {set(list(zip(*mi))[1][:len(database.database_columns)])} \n\
                does not match {set(database.database_columns)} database.csv {mi_lv1[0]}-lvl-2 columns."


    except FileNotFoundError:
        print("Database file not found.")
        print("Create new database.csv")
        database_df = pd.DataFrame()

    database_df = pd.concat(
                    [database_df, newdata_df],
                    axis=0,
                    join="outer",
                    ignore_index=True,
                    keys=None,
                    levels=None,
                    names=None,
                    verify_integrity=False,
                    copy=False,
                )
            
    print("Database DataFrame:")
    print(database_df)

    database_df.to_csv(args.database, index=False)



if __name__ == "__main__":
    main()

