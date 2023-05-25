"""
CSV and DataFrame related function. Just for 2-level columnlabels

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
import yaml

time_format = "%Y-%m-%d_%H%M%S"

firstlvl_sublabels = ['database', 'studyparameters', 'case']

def get_template(studydir):
    basename_studydir = os.path.basename(os.path.abspath(studydir))
    infofile = os.path.join(studydir, f"{basename_studydir}.info")
    with open(infofile, 'r') as file:
        info = yaml.safe_load(file)
    return info['templatecase']

def get_case(casedir):
    return os.path.basename(os.path.abspath(casedir))

def get_mtime(casedir):
    return time.strftime(time_format, time.localtime(os.path.getmtime(casedir)))

def get_user():
    return os.getlogin()

database_sublabels = ['TEMPLATE', 'CASE', 'USER', 'M_TIME']

refinement_labels = [
    ('studyparameters','N_CELLS'),
    ('studyparameters','RESOLUTION'), 
    ('studyparameters','MAX_CELL_SIZE'),
    ]

time_label = ('case', 'TIME')

def get_database_parameter_for_case(studydir, casedir, columnlevel=2) -> dict:
    if columnlevel == 1:
        dict_ = {
            'TEMPLATE'  : get_template(studydir),
            'CASE'      : get_case(casedir),
            'USER'      : get_user(),
            'M_TIME'    : get_mtime(casedir),
        }
        return dict_
    elif columnlevel == 2:
        raise NotImplemented("Columnlvl=2 not implemented")
    else:
        raise RuntimeError("Choose {1,2} for columnlvl.")

def get_studyparameters(columns):
    """
    Provide columns of a study DataFrame. Can be a list, Index or two level MultiIndex object.
    The columns must follow the convention of this module.
    """
    if isinstance(columns, pd.MultiIndex):
        return columns[columns.get_loc('studyparameters')]
    elif isinstance(columns, pd.Index):
        return columns[len(database_sublabels)+1 : columns.get_loc(time_label[1])]
    elif isinstance(columns, list):
        return columns[len(database_sublabels)+1 : columns.index(time_label[1])]
    else:
        raise TypeError("Columns must be list, pd.Index or pd.MultiIndex")


def get_refinementlabel(study_df):
    """
    Returns the first matching refinementparameter in the columnlabels.
    None is returned if the DataFrame has no refinementparameter.
    """
    for param in refinement_labels:
        if param in study_df.columns:
            return param
    else:
        return None
    

def get_raw_title(df):
    """
    Provide a DataFrame and get a raw label of all unique studyparameters and their values.
    """
    studyparameters = get_studyparameters(df.columns)
    list_ = []
    for param in studyparameters:
        # unique = df[param].unique()
        # if len(unique) == 1:
        list_.append(f'{param[1]}')
    return ' / '.join(list_)

def get_raw_label(df):
    """
    Provide a DataFrame and get a raw label of all unique studyparameters and their values.
    """
    studyparameters = get_studyparameters(df.columns)
    list_ = []
    for param in studyparameters:
        unique = df[param].unique()
        if len(unique) == 1:
            list_.append(f'{unique[0]}')
    return ' / '.join(list_)
    
def smallest_refinement_gb(study_df, by, deltaX=('case','DELTA_X')):
    """
    - gets study_df
    - calcs study_gb by all studyparameters exept refinement parameter
    - for each gb_df get view at finest resultion case
    - sort groups according to get_value strategy
    - return provided gb but sorted
    
    Parameter
    =========
    study_df: Groupby
    
    by: label
        label by which the groups are sorted.
    """
    refinement_label = get_refinementlabel(study_df)

    if refinement_label is None:
        return None 
    
    mi = study_df.columns
    studyparameters = list(mi[mi.get_loc('studyparameters')])
    studyparameters.remove(refinement_label)

    if isinstance(studyparameters, list) and len(studyparameters) == 1:
        studyparameters = studyparameters[0]

    study_ref_gb = study_df.groupby(by=studyparameters, sort=False)

    def key(ref_gb_item):
        ref_gr = ref_gb_item[0]
        ref_df = ref_gb_item[1]
        caselabel = ('database','CASE')
        finest_case = ref_df.loc[ref_df[deltaX].idxmin(), caselabel]
        finest_case_df = ref_df[ref_df[caselabel] == finest_case]
        value = convergence.get_value(finest_case_df[[time_label, by]])
        if np.isnan(value):
            return np.Inf
            # return float(1e6)
        else:
            return value
    
    return sorted(study_ref_gb, key=key)


def filter_keep(study_df, column, value, drop=False):
    """
    Takes a study_df and removes all rows which do not match the value to keep in the provided column.
    Also drops column for drop = True
    """
    study_df = study_df.loc[study_df[column] == value]
    if drop:
        study_df = filter_drop(study_df, column)
    study_df.reset_index()
    return study_df

def filter_rm(study_df, column, value):
    """
    Takes a study_df and removes all rows which do match the value to remove in the provided column.
    """
    study_df = study_df.loc[study_df[column] != value]
    # study_df = study_df.drop(column, axis='columns', inplace=False)
    study_df.reset_index()
    return study_df

def filter_drop(study_df, column):
    """
    Takes a study_df and drops a whole column if the values are unique.
    """
    if len(study_df[column].unique()) == 1: 
        study_df = study_df.drop(column, axis='columns', inplace=False) # inplace=False otherwise SettingWithCopyWarning
    else:
        print("Did nothing. Column values are not unique.")
    return study_df