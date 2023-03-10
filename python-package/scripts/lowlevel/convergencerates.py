#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import pandas as pd
import numpy as np
from leia.database import groupcolumns
from leia import convergence

app_description = \
"""
This script takes a CSV file with postprocessing data of a parameter study of one OpenFoam template case.
Requirements for the CSV file:
    - seperator = ','
    - the n study parameters are the first n columns
    - the n study parameters contain one parameter which represents mesh refinement with the label 'N_CELLS'
    - the n+1 column stores time values and is labeled as 'TIME'
    - one column stores 'DELTA_X', which is necessary to calculate the convergence rate

The script determine columns which represent error properties by matching the first two label letters against 'E_'.
For these error properties the convergencerates over 'DELTA_X' will be calculated and \
    added to each row corresponding to studyparameter group.
"""

def main():

    #---- Command line arguments ----------------------------------------------
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    parser.add_argument("CSV",
                        help="Path to the CSV file, which contains all postprocessing data of all cases of a study belonging to one template case like 3Ddeformation")

    parser.add_argument("-t", "--time-label",
                        help="Column label for the time values",
                        required=False,
                        default="TIME",
                        dest="time_label")

    parser.add_argument("-d", "--deltaX-label",
                        help="Column label for the represantative cell length deltaX",
                        required=False,
                        default="DELTA_X",
                        dest="deltaX_label")

    parser.add_argument("-r", "--refinement-parameter",
                        help="Column label of the refinement parameter of the study",
                        required=False,
                        default="N_CELLS",
                        dest="refinement_parameter")

    parser.add_argument("-w", '--write', action='store_true',
                        required=False,
                        default=False,
                        dest="write",
                        )

    # parser.add_argument("-n", "--nParameters",
    #                     help="Number of parameters of the study",
    #                     required=True,
    #                     dest="nParameters")

    # parser.add_argument("-o", "--output-file",
    #                     help="If one wishes to write the output to a file",
    #                     required=False,
    #                     default="",
    #                     dest="test")

    args = parser.parse_args()

    time_label = args.time_label
    deltaX_label = args.deltaX_label
    refinement_parameter = args.refinement_parameter
    #--------------------------------------------------------------------------

    cases_df = pd.read_csv(args.CSV)
    columns = cases_df.columns.to_list()
    
    if deltaX_label not in columns:
        raise RuntimeError(f"'{deltaX_label}' not in the columns of the DataFrame")
    if refinement_parameter not in columns:
        raise RuntimeError(f"'{refinement_parameter}' not in the columns of the DataFrame")
    if time_label not in columns:
        raise RuntimeError(f"Did not find label '{time_label}' in the columns of the DataFrame")

    columns_groups = groupcolumns(columns)
    studyparameters = columns_groups['studyparameters']

    errorlabels = list(filter(lambda label: label[:2] == "E_", cases_df.columns))
    cases_df = add_convergencerates(cases_df, studyparameters, refinement_parameter, errorlabels, deltaX_label)


    if args.write == True:
        cases_df.to_csv(args.CSV, index=False)
    # else: # show 5 best instances
    #     print(f"\n# {args.CSV}")
    #     for label in convergencelabels(errorlabels):
    #         show_nBest_convergenerates(
    #             cases_df[noRefinement_parameters(studyparameters, refinement_parameter) + convergencelabels(errorlabels)], 
    #             5,  
    #             label
    #             )
    #     print(f"\n#{'-'*90}\n")
    else:
        print(cases_df)

# def show_nBest_convergenerates(cases_df, n: int, convergencelabel):

#     df = cases_df.drop_duplicates()
#     result = df.nlargest(n, convergencelabel)

#     print(f"\n## {n}-largest {convergencelabel}")
#     print(result.to_string(index=False))


def noRefinement_parameters(studyparameters, refinement_parameter):
    # studyparameters without h-refinement parameter
    noRef_parameters = studyparameters.copy()
    noRef_parameters.remove(refinement_parameter)
    return noRef_parameters


# def convergencelabels(labels):
#     """Returns a list of convergence labels from a list of property labels"""
#     return [convergence.global_label(label) for label in labels]

# def local_convergencelabels(labels):
#     """Returns a list of convergence labels from a list of property labels"""
#     return [convergence.local_label(label) for label in labels]


# def calc_convergence(df: pd.DataFrame, key: str, h_label: str):
#     """
#     Calculates the convergence rate over mesh refinement of some property.
#     Parameters
#     ----------
#     df: pandas.DataFrame
#         DataFrame where every row correspond to a different mesh refinement level. 
#         df needs columns for h_label and the key for column where convergence should be calculated.
#     key: str
#         name of the column in the DataFrame for which convergence rate should be calculated

#     Returns
#     -------
#     float: convergence rate
#     """
#     return np.polyfit(np.log(df[h_label]), np.log(df[key]), 1)[0]

# def calc_local_convergence(df: pd.DataFrame, key: str, h_label: str):
#     """
#     Calculates the local convergence rates with mesh refinement of some property. \
#         Local convergence rates will be calculated between the current and the previous mesh refinement level.
#         The coarsest mesh has no previous mesh refinement level, so it will get the same local convergence rate as the next refinement level.
#     Parameters
#     ----------
#     df: pandas.DataFrame
#         DataFrame where every row correspond to a different mesh refinement level. 
#         df needs columns for h_label and the key for column where convergence should be calculated.
#     key: str
#         name of the column in the DataFrame for which convergence rate should be calculated

#     Returns
#     -------
#     pandas.DataFrame: convergence rate
#     """
#     hs = df[h_label].values
#     vals = df[key].values
#     local_convergences = pd.DataFrame(hs, columns=[h_label])
#     local_convergences['local_convergence'] = None
#     for i in range(1,len(df)):
#         h, val = hs[i-1:i+1], vals[i-1:i+1]
#         local_convergences.loc[i, 'local_convergence'] = np.polyfit(np.log(h), np.log(val), 1)[0]
#     local_convergences.loc[0, 'local_convergence'] = local_convergences.loc[1, 'local_convergence']
#     return local_convergences


def add_convergencerates(
    cases_df: pd.DataFrame, 
    studyparameters: list,
    refinement_parameter: str,
    propertylabels: list, 
    h_label: str
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
        List of properties for which convergence rates shoul be calculated and added to the DataFrame.
    h_label: str
        Label of column which stores the representative cell size deltaX or h

    Returns:
    --------
    pandas.DataFrame: Orginal DataFrame with added convergencerates columns of all provided properties. 

    """

    # pandas.GroupBy object. DataFrame grouped by studyparameters    
    cases_gb = cases_df.groupby(studyparameters)

    # DataFrame of all TIME == endTime rows of all cases
    cases_endTimes_df = cases_gb.last().reset_index()

    # studyparameters without h-refinement parameter
    noRef_parameters = noRefinement_parameters(studyparameters, refinement_parameter)

    # pandas.GroupBy object. endTime DataFrame grouped by studyparameters without h-refinement
    refinement_endTimes_gb = cases_endTimes_df.groupby(noRef_parameters)
    # pandas.GroupBy object. orginal DataFrame grouped by studyparameters without h-refinement
    refinement_gb = cases_df.groupby(noRef_parameters)

    for label_p, label_c  in zip(propertylabels, convergence.global_label(propertylabels)):
        cases_df[label_c] = float(0) # initialise new convergence column
        convergence_ser = refinement_endTimes_gb.apply(
                convergence.get_global_convergence, 
                label_p, 
                deltaX=h_label,
                refinement_parameter=refinement_parameter
            ) # pandas.Series with convergencerates of groups
        for group, df_indices in zip(refinement_endTimes_gb, tuple(refinement_gb.indices.values())):
            # df_indices: list
            #   List of row indices of the orginal cases_df DataFrame which belong to a group of studyparameters without h-refinement
            group_index = group[0]
            cases_df.loc[df_indices, label_c] = convergence_ser.at[group_index]

    for label_p, label_c  in zip(propertylabels, convergence.local_label(propertylabels)):
        cases_df[label_c] = float(0) # initialise new convergence column
        local_convergence_df = refinement_endTimes_gb.apply(
            convergence.get_local_convergence, 
            label_p,                
            deltaX=h_label,
            refinement_parameter=refinement_parameter
            ) # pandas.DataFrame with local convergencerates DataFrames as entries for non-ref-studyparameter groups
        
        for index in cases_gb.indices.keys():
            row_numbers = cases_gb.indices[index]
            index = index[:-1]
            local_convergence_group_df = local_convergence_df.loc[index]
            value = local_convergence_group_df[local_convergence_group_df[h_label] == cases_df.loc[row_numbers[0],h_label]][convergence.local_label(label_p)].values
            assert len(value) == 1
            cases_df.loc[row_numbers, label_c] = value[0]


    return cases_df


if __name__ == "__main__":
    main()