import numpy as np
import pandas as pd
from itertools import zip_longest
import matplotlib.pyplot as plt
import os.path
import os
from leia import convergence, database, studycsv
import math
from itertools import cycle



def detox_label(label: str):
    dict_ = {
        '$': ''
    }
    for key in dict_:
        label = label.replace(key, dict_[key])
    return label

def legendlabel(dict, key):
    return detox_label(f"{key}: {dict[key]}")

def timeplot(study_df, struct):
    prop = struct[0]
    ylabel = struct[1]
    title = struct[2]
    case_gb = study_df.groupby(('database','CASE'), sort=False)
    fig, ax = plt.subplots()
    for case, case_df  in case_gb:
        label = f"{case}: {database.get_raw_label(case_df)}"
        label = detox_label(label)
        ax.plot(case_df[('case','TIME')].values, case_df[prop].values, 'x', label=label)
    ax.legend(loc='upper center', bbox_to_anchor=(0.5,-0.12))
    ax.set_ylabel(ylabel)
    ax.set_xlabel('TIME in seconds')
    ax.grid('on')
    ax.set_title(title)
    return fig

def convergenceplot(study_df, struct):
    prop = struct[0]
    ylabel = struct[1]
    title = struct[2]
    
    refinementlabel = studycsv.get_refinementlabel(study_df)
    mi = study_df.columns
    studyparameters = list(mi[mi.get_loc_level('studyparameters')[0]])
    studyparameters.remove(refinementlabel)
    if len(studyparameters) == 1:
        studyparameters = studyparameters[0]

    convergence_gb = study_df.groupby(studyparameters, sort=False)

    def unique_1significant(values):
        unique_ = []
        compare = []
        for val in values:
            val_round = float(f"{val:.0e}")
            if val_round not in compare:
                unique_.append(val)
                compare.append(val_round)
        return unique_

    # filter out similiar resolutions that can occur on perturbed meshes. Would overload xticks
    # study_resolutions = study_df[('case', 'DELTA_X')].apply(lambda value: float(f"{value:.1e}")).unique()
    study_resolutions = study_df[('case', 'DELTA_X')].unique()
    
    # better filter
    study_resolutions = unique_1significant(study_resolutions)


    
    fig, ax = plt.subplots()
    ax.loglog()
    xtick_values = study_resolutions
    xtick_labels = [f'${value:.1e}$' for value in study_resolutions]
    ax.set_xticks(xtick_values, minor=False) 
    ax.set_xticklabels(xtick_labels)
    ax.minorticks_off()
    plt.title(f"{title}")
    plt.ylabel(ylabel)
    plt.xlabel("$h$")
    
    convergence_ref = 1e6

    for id, (parameters, convergence_df) in enumerate(convergence_gb):
        res_val_np = convergence.get_values(convergence_df, prop, refinement_parameter=refinementlabel[1])
        resolutions = res_val_np[:,0]
        values = res_val_np[:,1]
        convergence_ref = min(convergence_ref, values[0])
        leg_label = database.get_raw_label(
                                convergence_df.drop(studycsv.get_refinementlabel(convergence_df), axis='columns')
                    )
        leg_label = detox_label(leg_label)
        ax.plot(resolutions, values, marker='x',label=leg_label)
    
    # REPRESENTATIVE_GROUP = convergence_df
    # h_01 = [REPRESENTATIVE_GROUP[('case',"DELTA_X")].iloc[0],REPRESENTATIVE_GROUP[('case',"DELTA_X")].iloc[-1]]
    h_01 = [np.max(study_resolutions), np.min(study_resolutions)]
    Ev_error2nd_01 = [convergence_ref, convergence_ref*(h_01[1]/h_01[0])**2]
    Ev_error1st_01 = [convergence_ref, convergence_ref*(h_01[1]/h_01[0])]
    ax.plot(h_01,Ev_error2nd_01,"k--",label="second-order")
    ax.plot(h_01,Ev_error1st_01,"r:",label="first-order")
    
    ax.legend(loc='center left', bbox_to_anchor=(1,0.5))
    # ax.legend(loc='upper center', bbox_to_anchor=(0.5,-0.12))
    return fig

#-- START function block: Splitting many lines on multiple plots -----------------------------------

def nitems_per_group(nitems: int, maxnitems=10) -> list[int]:
    """
    Function distributes the total number of items into groups, so that each group has maximal items \
    but does not exceed maxitemspergroup.

    Parameter
    ========
    nitems: int
        Number of total lines which should be distributed.
    Returns
    =======
    list: Number of items for each group 
    """
    ngroups = int(np.ceil(nitems/ maxnitems))
    nitems_in_group = [0] * ngroups
    iter_ = cycle(range(len(nitems_in_group)))
    for i in range(nitems):
        nitems_in_group[next(iter_)] += 1
    return nitems_in_group

def group_list(ls: list, maxnitems=10) -> list[list]:
    """
    From a provided list this function builds a 1-level nested list. 
    It groups the original list according to the function nitems_per_group().
    """
    nitems = len(ls)
    nitems_in_group = nitems_per_group(nitems, maxnitems=maxnitems)
    return [ls[i-n:i] for n,i in zip(nitems_in_group, np.cumsum(nitems_in_group))]

def group_DataFrame(study_df: pd.DataFrame, by, maxnitems=10) -> list[pd.DataFrame]:
    """
    From a provided agglomerated pd.DataFrame this function builds a 1-level nested list. 
    It groups the original DataFrame according to the function nitems_per_group().
    """
    if isinstance(by, list) and len(by) == 1:
        by = by[0]
    
    by_df_ls = group_list(list(study_df.groupby(by=by, sort=False).__iter__()), maxnitems=maxnitems)
    df_ls = []
    for group in by_df_ls:
        by, dfs = list(zip(*group))
        index_concat = []
        for df in dfs:
            index_concat.extend(df.index.values)
        df_ls.append(study_df.loc[index_concat])
    return df_ls

#-- END function block: Splitting many lines on multiple plots -----------------------------------

