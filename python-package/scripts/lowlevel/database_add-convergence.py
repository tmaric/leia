#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import pandas as pd
from leia import convergence, studycsv

app_description = \
f"""
This script takes a study CSV file with concatenated postprocessing data of a parameter study.
The CSV file hase a 2-level columns labels. Where the first level is : {studycsv.firstlvl_sublabels}
The script determine columns which represent error properties by matching the first two label letters against 'E_'.
For these error properties the convergencerates over 'DELTA_X' will be calculated and \
    added to each row corresponding to studyparameter group.
"""
def parse_arguments():
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    parser.add_argument("CSV",
                        help="Path to the CSV file, which contains all postprocessing data of all cases of a study belonging to one template case like 3Ddeformation")

    parser.add_argument("-t", "--time-label",
                        help="Column label for the time values. In 1-lvl column group: 'case'.",
                        required=False,
                        default="TIME",
                        dest="time_label")

    parser.add_argument("-d", "--deltaX-label",
                        help="Column label for the represantative cell length deltaX. In 1-lvl column group: 'case'.",
                        required=False,
                        default="DELTA_X",
                        dest="deltaX_label")

    parser.add_argument("-r", "--refinement-parameter",
                        help="Column label of the refinement parameter of the study. In 1-lvl column group: 'studyparameters'.",
                        required=False,
                        default="N_CELLS",
                        dest="refinement_parameter")

    group = parser.add_mutually_exclusive_group(required=True)

    group.add_argument('-i', '--inplace', 
                       action='store_true',
                       help="Save sorted CSV inplace."
                       )

    group.add_argument('-f', '--file',
                       help="Save sorted CSV to file",
                       dest='file'
                       )
    return parser.parse_args()

def main():

    #---- Command line arguments ----------------------------------------------
    

    args = parse_arguments()

    time_label              = ('case', args.time_label)
    deltaX_label            = ('case', args.deltaX_label)
    refinement_parameter    = ('studyparameters', args.refinement_parameter)
    #--------------------------------------------------------------------------

    study_df = pd.read_csv(args.CSV, header=[0,1])
    
    if deltaX_label not in study_df.columns:
        raise RuntimeError(f"'{deltaX_label}' not in the columns of the DataFrame")
    if refinement_parameter not in study_df.columns:
        raise RuntimeError(f"'{refinement_parameter}' not in the columns of the DataFrame")
    if time_label not in study_df.columns:
        raise RuntimeError(f"Did not find label '{time_label}' in the columns of the DataFrame")

    errorlabels = list(filter(lambda label: label[0] == 'case' and label[1][:2] == "E_", study_df.columns))
    study_df = add_convergencerates(study_df, 
                                    studyparameters = study_df.columns[study_df.columns.get_loc('studyparameters')], 
                                    refinement_parameter = refinement_parameter, 
                                    propertylabels = errorlabels, 
                                    h_label = deltaX_label,
                                    time_label = time_label
                                    )

    if args.inplace:
        savefile = args.CSV
    elif args.file:
        savefile = args.file
    else:
        raise RuntimeError('No option for saving.')
    
    study_df.to_csv(savefile, index=False)


def noRefinement_parameters(studyparameters, refinement_parameter):
    # studyparameters without h-refinement parameter
    noRef_parameters = studyparameters.copy()
    noRef_parameters.remove(refinement_parameter)
    return noRef_parameters


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
    refinementlabel = studycsv.get_refinementlabel(study_df)
    mi = study_df.columns
    studyparameters = list(mi[mi.get_loc_level('studyparameters')[0]])
    cases_gb = study_df.groupby(studyparameters, sort=False)
    studyparameters.remove(refinementlabel)
    if len(studyparameters) == 1:
        studyparameters = studyparameters[0]

    refinement_gb = study_df.groupby(studyparameters, sort=False)

    for label_p, label_c  in zip(propertylabels, map(convergence.global_label, propertylabels)):
        study_df[label_c] = float(0) # initialise new convergence column
        convergence_ser = refinement_gb.apply(
                convergence.get_global_convergence, 
                label_p, 
                deltaX=h_label,
                refinement_parameter=refinement_parameter,
                time=time_label
            ) # pandas.Series with convergencerates of groups
        for group, df_indices in zip(refinement_gb, tuple(refinement_gb.indices.values())):
            # df_indices: list
            #   List of row indices of the orginal cases_df DataFrame which belong to a group of studyparameters without h-refinement
            group_index = group[0]
            study_df.loc[df_indices, label_c] = convergence_ser.at[group_index]

    for label_p, label_c  in zip(propertylabels, map(convergence.local_label, propertylabels)):
        study_df[label_c] = float(0) # initialise new convergence column
        local_convergence_df = refinement_gb.apply(
            convergence.get_local_convergence, 
            label_p,                
            deltaX=h_label,
            refinement_parameter=refinement_parameter,
            time=time_label
            ) # pandas.DataFrame with local convergencerates DataFrames as entries for non-ref-studyparameter groups
        
        for index in cases_gb.indices.keys():
            row_numbers = cases_gb.indices[index]
            index = index[:-1]
            local_convergence_group_df = local_convergence_df.loc[index]
            value = local_convergence_group_df[local_convergence_group_df[h_label] == study_df.loc[row_numbers[0],h_label]][convergence.local_label(label_p)].values
            assert len(value) == 1
            study_df.loc[row_numbers, label_c] = value[0]

    return study_df


if __name__ == "__main__":
    main()