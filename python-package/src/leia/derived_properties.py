import pandas as pd
import numpy as np
import copy

def _check(df):
    # df must have two columns, zeroth representing time values and the first representing some float values
    assert df.shape[1] == 2, 'DataFrame has not exactly 2 columns!'

    # time values need to be sorted
    is_sorted = lambda a: np.all(a[:-1] <= a[1:])
    if df.shape[0] > 1: 
        assert is_sorted(df.iloc[:,0].values), 'DataFrame zeroth column, representing time values, is not sorted!'

def total_variation_per_time(df: pd.DataFrame) -> pd.Series:
    """
    Calculates the total variation of a quantity $\phi$
    
    $\phi_{TV}^0 = 0$
    $\phi_{TV}^n = \phi_{TV}^{n-1} + \frac{|\phi^n - \phi^{n-1}|}{\delta_t}$
    """
    _check(df)

    time_se = df.iloc[:,0]
    prop_se = df.iloc[:,1]

    TV_se = pd.Series(np.zeros(time_se.size))

    for idx in range(time_se.size):
        if idx == 0:
            continue
        else:
            deltaT = time_se.iloc[idx] - time_se.iloc[idx-1]
            TV_se.iloc[idx] = TV_se.iloc[idx-1] \
                + np.abs(prop_se.iloc[idx] - prop_se.iloc[idx-1])/deltaT
    return TV_se

def total_variation(df: pd.DataFrame) -> pd.Series:
    """
    Calculates the total variation of a quantity $\phi$
    
    $\phi_{TV}^0 = 0$
    $\phi_{TV}^n = \phi_{TV}^{n-1} + |\phi^n - \phi^{n-1}|$
    """
    _check(df)

    time_se = df.iloc[:,0]
    prop_se = df.iloc[:,1]

    TV_se = pd.Series(np.zeros(time_se.size))

    for idx in range(time_se.size):
        if idx == 0:
            continue
        else:
            TV_se.iloc[idx] = TV_se.iloc[idx-1] \
                + np.abs(prop_se.iloc[idx] - prop_se.iloc[idx-1])
    return TV_se

def append_TVtime(study_df, column, TVcolumn):
    cases_gb = study_df.groupby(by=[('database','CASE'),('database','M_TIME')])
    # study_df[('case','E_VOL_ALPHA_REL_TV')] = float(0)
    study_df[TVcolumn] = float(0)
    for case, case_df in cases_gb:
        TV_se = total_variation_per_time(case_df[[('case','TIME'), column]])
        case_idx = case_df.index
        study_df.loc[case_idx,TVcolumn] = TV_se.array
    return study_df

def append_TV(study_df, column, TVcolumn):
    cases_gb = study_df.groupby(by=[('database','CASE'),('database','M_TIME')])
    # study_df[('case','E_VOL_ALPHA_REL_TV')] = float(0)
    study_df[TVcolumn] = float(0)
    for case, case_df in cases_gb:
        TV_se = total_variation(case_df[[('case','TIME'), column]])
        case_idx = case_df.index
        study_df.loc[case_idx,TVcolumn] = TV_se.array
    return study_df