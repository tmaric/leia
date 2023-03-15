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

import time
import os.path
import numpy as np
import pandas as pd
from itertools import zip_longest
import matplotlib.pyplot as plt
import os.path
import os
from leia import convergence
from leia.convergence import _config
import warnings

time_format = "%Y-%m-%d_%H%M%S"

database_columns = ['CASE', 'TEMPLATE', 'M_TIME', 'USER']

refinement_columns = ['N_CELLS', 'MAX_CELL_SIZE']

def isErrorColumn(errorcolumn):
    errorcolumn = drop_multilabel(errorcolumn)
    if isinstance(errorcolumn, str) and errorcolumn[:2] == 'E_':
        return True
    else:
        return False

    
def drop_multiindex(index):
    columns = index.copy()
    while isinstance(columns, pd.MultiIndex):
        columns = columns.droplevel()
    return columns

def drop_multilabel(label):
    if isinstance(label, tuple) and len(label) > 1:
        label = label[-1]
    return label

def mtime_str(file):
    return time.strftime(time_format, time.localtime(os.path.getmtime(file)))

# def groupcolumns(columns_orig):
#     columns = columns_orig.copy()
#     potential_database = database_columns.copy()
    
#     gr = {
#         'database':list(),
#         'studyparameters':list(),
#         'case':list()
#     }

#     # Assumes first columns are studyparameters
#     for i, column in enumerate(columns_orig):
#         if column in potential_database:
#             break
#         else:
#             gr['studyparameters'].append(columns.pop(0))

#     # Assumes database columns are in the list potential_database
#     for column in columns_orig[i:]:
#         if column in potential_database:
#             gr['database'].append(column)
#             columns.remove(column)

#     # Assumes rest of columns are case columns
#     gr['case'].extend(columns)

#     assert len(gr['database']) + len(gr['studyparameters']) + len(gr['case']) == len(columns_orig)
#     return gr

def get_refinementparameter(study_df):
    """
    Returns the first matching refinementparameter in the columns.
    None is returned if the DataFrame has no refinementparameter.
    """
    for param in refinement_columns:
        if param in study_df.columns:
            return param
    else:
        return None

def df_endTimes(study_df):
    """
    Filters out all last timesteps of a case from the study_df.
    """
    return study_df.groupby(by=('database','CASE')).nth(-1).reset_index()

def df_represantive_error_rows(study_df, errorcolumn):
    errorlabel = drop_multilabel(errorcolumn)
    assert isErrorColumn(errorcolumn), f'{errorcolumn} does not correspond to writing convention for a error column.'
    indices = []
    for by, case_df in study_df.groupby(by=[('database','CASE'),('database','M_TIME')]):
        strategy = _config.get_strategy(errorlabel)
        if strategy == 'max':
            indices.append(case_df[errorcolumn].idxmax())
        elif strategy == 'endTime':
            indices.append(case_df[errorcolumn].index[-1])
        else:
            raise RuntimeError(f'No index selection process defined for strategy {strategy}')
    return study_df.iloc[indices]



def database_smallest(df, columns, nsmallest=10):
    mi = df.columns
    database = mi[mi.get_locs(['database'])].to_list()
    studyparameters = mi[mi.get_locs(['studyparameters'])].to_list()
    columns = list(zip_longest([], columns, fillvalue='case'))

    if columns[0] not in df.columns:
        raise RuntimeError(f"Main columns {columns[0]} is not in DataFrame. Can not sort.")
    # for idx, col in enumerate(columns):
    #     if col not in df.columns:
    #         skip = columns.pop(idx)
    #         warnings.warn(f"Skip tabulating column {skip}.")

    columns = list(filter(lambda col: col in df.columns, columns))

    viewsmallest_df = df.nsmallest(nsmallest, columns)
    return viewsmallest_df[database + studyparameters + [('case','TIME')] + columns ]

def filter_parameter(study_df, study_dict, parameter=(('studyparameters','N_CELLS'), 400)):
    
    def filterfunc(pair, parameter=parameter):
        parameter_key = parameter[0][1]
        parameter_val = parameter[1]
        val = pair[1]
        return val[parameter_key] == parameter_val
    
    new_dict = dict(filter(filterfunc, study_dict.items()))

    parameter_key = parameter[0]
    parameter_val = parameter[1]
    new_df = study_df.copy()
    new_df = new_df[new_df[parameter_key] == parameter_val]
    return new_df, new_dict

def get_raw_label(df):
    mi = df.columns
    # studyparameters = mi[mi.get_loc_level('studyparameters')[0]]
    studyparameters = mi[mi.get_loc('studyparameters')]
    list_ = []
    for param in studyparameters:
        unique = df[param].unique()
        if len(unique) == 1:
            list_.append(f'{param[1]}: {unique[0]}')
    return ', '.join(list_)

