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