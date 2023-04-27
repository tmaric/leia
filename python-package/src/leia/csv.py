"""
CSV and DataFrame related function. Just for 1-level columns
And common function would coul be used for 2-level CSV, DataFrames as well
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


refinement_labels = ['N_CELLS', 'MAX_CELL_SIZE']


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