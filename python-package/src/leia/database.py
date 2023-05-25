"""
Module database
===============

- Defines how the database.csv files are organised
- Provides functions to interact with Database in memory (pd.DataFrame) and on disk (CSV)
- (FUTURE: Just for 2-level csv files or DataFrames)

Database
--------
- uses pd.MultiIndex with two levels as columns
    - Zeroth level column label : ['database', 'studyparameters', 'case']
    - Fist level columns label :
        - 'database' : ['CASE', 'TEMPLATE', 'M_TIME', 'USER']
            'CASE'      : basename of case folder
            'TEMPLATE'  : basename of template case folder
            'M_TIME'    : mtime of case directory
            'USER'      : user who added entry to database
        - 'studyparameters' : pyFoamRunParameterVariation.py parameters
        - 'case'        : Case specific data. Ex. leiaLevelSetFoam.csv and/ or gradPsiError.csv data
- collects postprocessing data of many cases
    - different template cases
    - diffrent studyparameters columns
    - different case columns
- stores values to all times

"""

import pandas as pd
import leia
from leia.convergence import _config


# def isErrorColumn(errorcolumn):
#     errorcolumn = drop_multilabel(errorcolumn)
#     if isinstance(errorcolumn, str) and ( errorcolumn[:2] == 'E_' or 'error' in errorcolumn ):
#         return True
#     else:
#         return False

def isErrorColumn(errorcolumn):
    if isinstance(errorcolumn, str):
        return errorcolumn[:2] == 'E_' or 'error' in errorcolumn
    elif isinstance(errorcolumn, tuple) and len(errorcolumn) == 2:
        if errorcolumn[0] != 'case':
            return False
        else:
            return isErrorColumn(errorcolumn[1])
    else:
        raise TypeError("Column must be str or tuple of length 2.")


    
def drop_multiindex(index):
    columns = index.copy()
    while isinstance(columns, pd.MultiIndex):
        columns = columns.droplevel()
    return columns

def drop_multilabel(label):
    if isinstance(label, tuple) and len(label) > 1:
        label = label[-1]
    return label


def df_endTimes(study_df):
    """
    Filters out all last timesteps of a case from the study_df.
    """
    return study_df.groupby(by=('database','CASE')).nth(-1).reset_index()

def df_represantive_error_rows(study_df, errorcolumn, timecolumn = ('case','TIME')):
    """
    Returns a view on the study_df with just one row per case 
    according to the errorcolumn and its strategy for selecting representative values
    """
    assert isErrorColumn(errorcolumn), f'{errorcolumn} does not correspond to writing convention for a error column.'
    indices = []
    for _, case_df in study_df.groupby(by=[('database','CASE'),('database','M_TIME')]):
        idx = leia.convergence.get_row(case_df[[timecolumn,errorcolumn]]).name
        indices.append(idx)
    return study_df.loc[indices]

def database_smallest(df, columns, nsmallest=10):
    mi = df.columns
    database = mi[mi.get_locs(['database'])].to_list()
    studyparameters = list(leia.studycsv.get_studyparameters(df.columns))

    if columns[0] not in df.columns:
        raise RuntimeError(f"Main columns {columns[0]} is not in DataFrame. Can not sort.")
    columns = list(filter(lambda col: col in df.columns, columns))

    viewsmallest_df = df.nsmallest(nsmallest, columns)
    return viewsmallest_df[database + studyparameters + [('case','TIME')] + columns ]



