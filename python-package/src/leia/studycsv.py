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

time_format = "%Y-%m-%d_%H%M%S"

firstlvl_sublabels = ['database', 'studyparameters', 'case']

database_sublabels = ['CASE', 'TEMPLATE', 'M_TIME', 'USER']

refinement_labels = [('studyparameters','N_CELLS'), ('studyparameters','MAX_CELL_SIZE')]

time_label = ('case', 'TIME')


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
    studyparameters = list(mi[mi.get_loc_level('studyparameters')[0]])
    studyparameters.remove(refinement_label)

    if isinstance(studyparameters, list) and len(studyparameters) == 1:
        studyparameters = studyparameters[0]

    study_ref_gb = study_df.groupby(by=studyparameters, sort=False)

    def key(ref_gb_item):
        ref_gr = ref_gb_item[0]
        ref_df = ref_gb_item[1]
        finest_deltaX = ref_df[deltaX].min()
        finest_case_df = ref_df[ref_df[deltaX] == finest_deltaX]
        return convergence.get_value(finest_case_df[[time_label, by]])
    
    return sorted(study_ref_gb, key=key)