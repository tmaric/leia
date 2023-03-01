from . import _config
import pandas as pd
import numpy as np
from leia import database

def _check(df):
    # df must have two columns, zeroth representing time values and the first representing some float values
    assert df.shape[1] == 2, 'DataFrame has not exactly 2 columns!'

    # time values need to be sorted
    is_sorted = lambda a: np.all(a[:-1] <= a[1:])
    if df.shape[0] > 1: 
        assert is_sorted(df.iloc[:,0].values), 'DataFrame zeroth column, representing time values, is not sorted!'



def global_label(label):
    label = database.drop_multilabel(label)
    format = lambda label: f"O({label})"
    if isinstance(label, str):
        return format(label)
    elif isinstance(label, list):
        return [format(lab) for lab in label]
    else:
        raise TypeError("label must be string or list")

def local_label(label):
    label = database.drop_multilabel(label)
    format = lambda label: f"O_LOCAL({label})"
    if isinstance(label, str):
        return format(label)
    elif isinstance(label, list):
        return [format(lab) for lab in label]
    else:
        raise TypeError("label must be string or list")

def get_value(df):
    """
    Parameter
    =========
    df: pd.DataFrame
        Case DataFrame with just two columns.
        Zeroth columns representing time.
        First column representing some float values.

    Returns
    =======
    float
    """
    _check(df)
    key = database.drop_multiindex(df.columns)[1]
    strategy_str = _config.get_strategy(key)
    return _config.dict_get_values[strategy_str](df)

def get_values(df, key, time='TIME', deltaX='DELTA_X', refinement_parameter='N_CELLS'):
    """
    Parameter
    =========
    df: pd.DataFrame
        Full DataFrame of cases which have the same studyparamters but varying refinement parameter.
        Should have at least columns for the key property, time, deltaX and refinement_parameter
    key: str
        Column name of the property for which the convergence should be calculated.
    """
    tmp = df.copy()
    tmp.columns = database.drop_multiindex(tmp.columns)
    key = database.drop_multilabel(key)
    gb = tmp.groupby(by=refinement_parameter, sort=False)
    list_ = []
    for ref_param, case_df in gb:
        if deltaX == refinement_parameter:
            deltaX_val = ref_param
        else:
            deltaX_val = case_df[deltaX].values[0]
        list_.append(np.array([deltaX_val, get_value(case_df[[time, key]])]))
    return np.stack(list_)

def calc_global_convergence(array: np.ndarray):
    """
    Calculates the global convergence rate over all mesh refinement levels of some property.
    Parameters
    ----------
    array: np.ndarry
        Numpy array with shape (X,2). Every row correspond to a different mesh refinement level. 
        Zeroth columns represents the deltaX length.
        First columns represents the property (float) for that deltaX.

    Returns
    -------
    float: convergence rate
    """
    assert array.shape[0] >= 2, "For convergence calculation the provided np.ndarray needs at least 2 rows."
    return np.polyfit(np.log10(array[:,0]), np.log10(array[:,1]), 1)[0]

def get_global_convergence(df, key, time='TIME', deltaX='DELTA_X', refinement_parameter='N_CELLS'):
    values_np = get_values(df, key, time, deltaX, refinement_parameter)
    return calc_global_convergence(values_np)

def calc_local_convergence(array: np.ndarray):
    """
    Calculates the local convergence rates over between all mesh refinement levels of some property.
    Local convergence rates will be calculated between the current and the previous mesh refinement level.
    The coarsest mesh has no previous mesh refinement level, so it will get the same local convergence rate as the next refinement level.
    
    Parameters
    ----------
    array: np.ndarry
        Numpy array with shape (X,2). Every row correspond to a different mesh refinement level. 
        Zeroth columns represents the deltaX length.
        First columns represents the property (float) for that deltaX.
        h_label and the key for column where convergence should be calculated.

    Returns
    -------
    pandas.DataFrame: convergence rate
    """

    list_ = []
    for i in range(1, array.shape[0]):
        rows = range(i-1,i+1)
        local_array = array[rows, :]
        list_.append(calc_global_convergence(local_array))
    list_.insert(0, list_[0])
    deltaXs = array[:,0]
    return np.vstack((deltaXs, np.stack(list_))).T

def get_local_convergence(df, key, time='TIME', deltaX='DELTA_X', refinement_parameter='N_CELLS'):
    """
    Returns
    -------
    pandas.DataFrame: convergence rate
    """
    values_np = get_values(df, key, time, deltaX, refinement_parameter)
    convergence_np = calc_local_convergence(values_np)
    return pd.DataFrame(convergence_np, columns=[deltaX, f'O_LOCAL({key})'])