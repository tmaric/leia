from . import _config
import pandas as pd
import numpy as np
from leia import database, studycsv
import warnings



def _label(label, format_func):
    """
    Accepts string or tuple length 2 for MultiIndex labels.
    """
    if isinstance(label, str): # normal label
        return format_func(label)
    elif isinstance(label, tuple) and len(label)==2: # Multiindexlabel
        return (label[0], format_func(label[1]))
    else:
        raise TypeError("Label must be string or lenght 2 tuple for MultiInde label.")

def global_label(label):
    """
    Accepts string or tuple length 2 for MultiIndex labels.
    """
    format = lambda label: f"O({label})"
    return _label(label, format)

def local_label(label):
    """
    Accepts string or tuple length 2 for MultiIndex labels.
    """
    format = lambda label: f"O_LOCAL({label})"
    return _label(label, format)


def get_row(df):
    """
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
    def _check(df):
        # df must have two columns, zeroth representing time values and the first representing some float values
        assert df.shape[1] == 2, 'DataFrame has not exactly 2 columns!'

        # time values need to be sorted
        is_sorted = lambda a: np.all(a[:-1] <= a[1:])
        if df.shape[0] > 1: 
            assert is_sorted(df.iloc[:,0].values), 'DataFrame zeroth column, representing time values, is not sorted!'

    _check(df)
    key = database.drop_multiindex(df.columns)[1]
    strategy_str = _config.get_strategy(key)
    return _config.dict_get_values[strategy_str](df)

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
    return get_row(df).iloc[-1]


def _check_labels(labels):
    """
    Provide a list of all labels provided as parameters to a function.
    This function checks if all labels are consistent str or length 2 tuples for MultiIndex labels.
    """
    if not all([isinstance(label, str) for label in labels]) and \
        not all([isinstance(label, tuple) for label in labels]):
            raise RuntimeError(f"Columnlabels must be all strings or length 2 tuples for MultiIndex.\
                            \ncolumns = {labels}"
            )

def get_values(df, key, *, time='TIME', deltaX='DELTA_X', refinement_parameter='N_CELLS'):
    """
    Accepts columns as label or lenth 2 tuple for MultiIndex label.

    Parameter
    =========
    df: pd.DataFrame
        Full DataFrame of cases which have the same studyparamters but varying refinement parameter.
        Should have at least columns for the key property, time, deltaX and refinement_parameter
    key: str
        Column name of the property for which the convergence should be calculated.
    """
    _check_labels([key, time, deltaX, refinement_parameter])
    tmp = df.copy()
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
    if array.shape[0] < 2:
        # warnings.warn("Convergence calculation needs at least 2 rows in np.ndarray. Returning NaN.")
        return float('NaN')
    return np.polyfit(np.log10(array[:,0]), np.log10(array[:,1]), 1)[0]

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
    if array.shape[0] >= 2:
        for i in range(1, array.shape[0]):
            rows = range(i-1,i+1)
            local_array = array[rows, :]
            list_.append(calc_global_convergence(local_array))
        list_.insert(0, list_[0])
        deltaXs = array[:,0]
        return np.vstack((deltaXs, np.stack(list_))).T
    elif array.shape[0] == 1:
        return np.array([array[:,0], np.array([np.nan])]).T
    else:
        raise NotImplemented()

def _get_convergence(refinement_df, key, calc_func, *, time='TIME', deltaX='DELTA_X', refinement_parameter='N_CELLS'):
    _check_labels([key, time, deltaX, refinement_parameter])
    values_np = get_values(refinement_df, key, time=time, deltaX=deltaX, refinement_parameter=refinement_parameter)
    return calc_func(values_np)

def get_global_convergence(refinement_df, key, *, time='TIME', deltaX='DELTA_X', refinement_parameter='N_CELLS'):
    """
    Accepts columns as label or lenth 2 tuple for MultiIndex label.

    Parameters
    ==========
    df: pd.DataFrame
        Refinement DataFrame, which is a concatenated DataFrame which contains cases of on refinement group.
        Studyparameters vary only in the refinement parameter.
    key: str
        Column name of the property for which the convergence should be calculated.
    """
    return _get_convergence(refinement_df, key, calc_global_convergence, 
                            time=time, 
                            deltaX=deltaX, 
                            refinement_parameter=refinement_parameter)


def get_local_convergence(refinement_df, key, *, time='TIME', deltaX='DELTA_X', refinement_parameter='N_CELLS'):
    """
    Accepts columns as label or lenth 2 tuple for MultiIndex label.

    Parameters
    ==========
    df: pd.DataFrame
        Refinement DataFrame, which is a concatenated DataFrame which contains cases of on refinement group.
        Studyparameters vary only in the refinement parameter.
    key: str
        Column name of the property for which the convergence should be calculated.

    Returns
    -------
    pandas.DataFrame: convergence rate
    """
    convergence_np = _get_convergence(refinement_df, key, calc_local_convergence, 
                                      time=time, 
                                      deltaX=deltaX, 
                                      refinement_parameter=refinement_parameter
                                      )
    refinements = pd.Index(refinement_df[refinement_parameter].unique(), name=refinement_parameter)
    return pd.DataFrame(convergence_np, index=refinements, columns=pd.Index([deltaX, local_label(key)]))


def add_convergencerates(
    study_df: pd.DataFrame,
    *, 
    studyparameters: list,
    refinement_parameter: str,
    propertylabels: list, 
    h_label: str,
    time_label: str
    ):
    """
    For a concatenated pandas.DataFrame of cases belonging to one template case, \
    this function calculates convergencerates of some properties over refinementlevels. \
    The convergencerates will be addes as columns to the DataFrame. \
    In every row, belonging to the group of studyparameters without the h-refinement parameter, \
    the resulting convergencerate of the group will be written.

    Parameters:
    -----------
    cases_df: pandas.DataFrame
        DataFrame with postprocessing data of all cases of a study belonging to one template case. \
        Index of DataFrame are just integer. All properties have to be in columns.
    studyparameters: list(str)
        List of study parameters, which are columns in the DataFrame
    propertylabels: list(str)
        List of properties for which convergence rates should be calculated and added to the DataFrame.
    h_label: str
        Label of column which stores the representative cell size deltaX or h
    time_label: str
        Label of column which stores the time values

    Returns:
    --------
    pandas.DataFrame: Orginal DataFrame with added convergencerates columns of all provided properties. 

    """
    if h_label not in study_df.columns:
        raise RuntimeError(f"'{h_label}' not in the columns of the DataFrame")
    if refinement_parameter not in study_df.columns:
        raise RuntimeError(f"'{refinement_parameter}' not in the columns of the DataFrame")
    if time_label not in study_df.columns:
        raise RuntimeError(f"Did not find label '{time_label}' in the columns of the DataFrame")


    refinementlabel = studycsv.get_refinementlabel(study_df)
    studyparameters = list(studycsv.get_studyparameters(study_df.columns))
    studyparameters.remove(refinementlabel)
    if len(studyparameters) == 1:
        studyparameters = studyparameters[0]

    if len(studyparameters) > 0:
        refinement_gb = study_df.groupby(studyparameters, sort=False)
    else:
        refinement_gb = study_df

    for label_p, label_c, label_lc  in zip(propertylabels, map(global_label, propertylabels), map(local_label, propertylabels)):
        study_df[label_c] = float(0) # initialise new convergence column
        study_df[label_lc] = float(0) # initialise new convergence column
        if isinstance(refinement_gb, pd.core.groupby.generic.DataFrameGroupBy):
            # BEGIN: global convergence
            convergence_ser = refinement_gb.apply(
                    get_global_convergence, 
                    label_p, 
                    deltaX=h_label,
                    refinement_parameter=refinement_parameter,
                    time=time_label
                ) # pandas.Series with convergencerates of groups
            for convergence_fl, (ref_group, ref_df) in zip(convergence_ser, refinement_gb):
                study_df.loc[ref_df.index, label_c] = convergence_fl
            # END: global convergence
            # BEGIN: local convergence
            local_convergence_df = refinement_gb.apply(
                get_local_convergence, 
                label_p,                
                deltaX=h_label,
                refinement_parameter=refinement_parameter,
                time=time_label
                ) # pandas.DataFrame with local convergencerates DataFrames as entries for non-ref-studyparameter groups
            params = local_convergence_df.index.names
            for convergence_fl, param_vals in zip(local_convergence_df[label_lc], local_convergence_df.index):
                index = study_df[(study_df[params] == param_vals).all('columns')].index
                study_df.loc[index, label_lc] = convergence_fl
            # END: local convergence
        elif isinstance(refinement_gb, pd.DataFrame):
            # BEGIN: global convergence
            convergence = get_global_convergence(
                    study_df,
                    label_p, 
                    deltaX=h_label,
                    refinement_parameter=refinement_parameter,
                    time=time_label
                )
            study_df[label_c] = convergence
            # END: global convergence
            # BEGIN: local convergence
            local_convergence = get_local_convergence(
                    study_df,
                    label_p, 
                    deltaX=h_label,
                    refinement_parameter=refinement_parameter,
                    time=time_label
                )
            for convergence_fl, resolution in zip(local_convergence[label_lc], local_convergence.index):
                index = study_df[study_df[refinement_parameter] == resolution].index
                study_df.loc[index, label_lc] = convergence_fl
            # END: local convergence


    return study_df