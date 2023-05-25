"""
Module provides functions for extracting represantive values from time DataFrames for convergence calculations.
Some properties need the values at endTimes, some the max or mean values over all times.

Functions must provide following interface:

    
Parameter
=========
df: pd.DataFrame
    Case DataFrame with just two columns.
    Zeroth columns representing time.
    First column representing some float values.

Returns
=======
pd.Series of shape (2,) with name being the index / row of df
"""

import pandas as pd
import numpy as np


def get_endvalue(df):
    """Returns value at endTime."""
    return df.iloc[-1,:]

def get_absendvalue(df):
    """Returns value at endTime."""
    return get_endvalue.apply(abs)

def get_maxvalue(df):
    """Returns the max value over all times."""
    return df.loc[df.iloc[:,1].idxmax()]

def get_lastvalue(df):
    """Returns the value of the last row with non-NaN value"""
    return df.loc[df.iloc[:,1].last_valid_index()]