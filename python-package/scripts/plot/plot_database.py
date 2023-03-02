#!/usr/bin/env python
# coding: utf-8

# In[1]:


import pandas as pd
import os.path
import os
from matplotlib import rcParams
import matplotlib.pyplot as plt
from leia import database, plot, studycsv
from argparse import ArgumentParser, RawTextHelpFormatter
rcParams["text.usetex"] = True
rcParams["figure.dpi"] = 200
rcParams["font.size"] = 14

app_description = \
"""
Script and module for postprocessing database-CSV files. 
Creates timeplots, tables and convergence plots for specified error properties.
"""

def structs(template, study):
    # struct = (property, ylabel, title, figname)
    return [
        # (
        #     ('case', 'E_MEAN_GRAD_PSI'), 
        #     r'$ mean(||\nabla \psi|-1|) $', 
        #     template+r' $mean(E_{gradPsi})$', 
        #     template+"_meanEGradPsi"
        # ),
        # (
        #     ('case', 'E_NARROW_MAX_GRAD_PSI'),
        #     r'$ max(||\nabla \psi|-1|) $',
        #     template+r' $max(E_{gradPsi})$ in narrowBand',
        #     template+'_maxEGradPsi-narrowBand'
        # ),
        # (
        #     ('case', 'E_NARROW_MEAN_GRAD_PSI'),
        #     r'$ mean(||\nabla \psi|-1|) $',
        #     template+r' signed-distance error in narrowBand',
        #     study+'_meanEGradPsi-narrowBand'
        # ),
        (
            ('case', 'E_GEOM_ALPHA'),
            r'$ E_g $',
            template+' geometrical error'+r' $E_g = \sum_{c \in C} |\Omega_c||\alpha_c(t_e) - \alpha_c(t_0)|$',
            study+'_Eg'
        ),
        (
            ('case', 'E_VOL_ALPHA_REL'),
            r'$ E_v $',
            template+' volume conservation error'+r' $E_v(t)  = \frac{\left|\sum_{c \in C}\alpha_c(t)|\Omega_c| - \sum_{c \in C} \alpha_c(t_0)|\Omega_c|\right|}{\left|\sum_{c \in C} \alpha_c(t_0)|\Omega_c|\right|}$',
            study+'_Ev'
        ),
    ]


def main():
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)
    parser.add_argument("studyCSV", help="The database-CSV file to postprocess.")
    args = parser.parse_args()

    study_csv = args.studyCSV

    template = os.path.basename(study_csv).split('_')[1]
    study = os.path.basename(study_csv).rpartition('_')[0]

    study_df = pd.read_csv(study_csv, header=[0,1])
    
    assert study_df.index.is_unique, "Index of study_df is not unique! Would cause errors."

    savedir = os.path.dirname(study_csv)
    # savedir = f"/home/julian/Masterthesis/meetings/next/{study}"
    # savedir = f"/home/julian/.local/share/Trash/files/{study}"

    os.makedirs(savedir, exist_ok=True)

    mystructs = structs(template, study)
    runall(study_df, mystructs, savedir)

# def isRefinementStudy(study_df):
#     return ('studyparameters','N_CELLS') in study_df.columns

def runall(study_df, structs, savedir):
    timeplot(study_df, structs, savedir)
    nsmallest_table(study_df, structs, savedir)
    convergenceplot(study_df, structs, savedir)
    # best_convergenceplot(study_df, structs, savedir)


def timeplot(study_df, structs, savedir):
    # if isRefinementStudy(study_df):
    refinement_label = studycsv.get_refinementlabel(study_df)
    if refinement_label is not None:
        for struct in structs:
            for resolution, resolution_df in study_df.groupby(refinement_label, sort=False):
                # groupby CASE and M_TIME for concatenated databases where cases could have the same basename
                grouped_resolution_df_ls = plot.group_DataFrame(resolution_df, by=[('database','CASE'),('database','M_TIME')], maxnitems=10)
                ls_len = len(grouped_resolution_df_ls)
                for fig_number, group_df in enumerate(grouped_resolution_df_ls):
                    fig = plot.timeplot(group_df, struct)
                    fig.savefig(os.path.join(savedir, f'{struct[3]}_{refinement_label[1]}-{resolution}_{fig_number+1}-{ls_len}.jpg'), bbox_inches='tight')
                    fig.savefig(os.path.join(savedir, f'{struct[3]}_{refinement_label[1]}-{resolution}_{fig_number+1}-{ls_len}.pdf'), bbox_inches='tight')
                    plt.close(fig)
    else:
        for struct in structs:
            grouped_study_df_ls = plot.group_DataFrame(study_df, by=[('database','CASE'),('database','M_TIME')], maxnitems=10)
            ls_len = len(grouped_study_df_ls)
            for fig_number, group_df in enumerate(grouped_study_df_ls):
                fig = plot.timeplot(group_df, struct)
                fig.savefig(os.path.join(savedir, f'{struct[3]}_{fig_number+1}-{ls_len}.jpg'), bbox_inches='tight')
                fig.savefig(os.path.join(savedir, f'{struct[3]}_{fig_number+1}-{ls_len}.pdf'), bbox_inches='tight')
                plt.close(fig)

def nsmallest_table(study_df, structs, savedir):
    ## nsmallest CSV table
    refinement_label = studycsv.get_refinementlabel(study_df)
    
    for struct in structs:
        if refinement_label is not None:
            columns = [
                struct[0][1],
                f"O({struct[0][1]})",
                f"O_LOCAL({struct[0][1]})",
            ]
        else:
            columns = [ 
                    struct[0][1],
                ]
            
        error_df = database.df_represantive_error_rows(study_df, struct[0])
        result_df = database.database_smallest(error_df, columns, len(error_df))
        result_df.to_csv(os.path.join(savedir, f'{struct[3]}_nsmallest.csv'), index=False)


def convergenceplot(study_df, structs, savedir):
    ## Convergence Plot
    refinement_label = studycsv.get_refinementlabel(study_df)

    if refinement_label is not None:
        mi = study_df.columns
        studyparameters = list(mi[mi.get_loc_level('studyparameters')[0]])
        studyparameters.remove(refinement_label)

        for struct in structs:
            grouped_study_df_ls = plot.group_DataFrame(study_df, by=studyparameters, maxnitems=10)
            ls_len = len(grouped_study_df_ls)
            for fig_number, group_df in enumerate(grouped_study_df_ls):
                fig = plot.convergenceplot(group_df, struct) 
                fig.savefig(os.path.join(savedir, f'{struct[3]}_convergence_{fig_number+1}-{ls_len}.jpg'), bbox_inches='tight')
                fig.savefig(os.path.join(savedir, f'{struct[3]}_convergence_{fig_number+1}-{ls_len}.pdf'), bbox_inches='tight')
                plt.close(fig)

# def best_convergenceplot(study_df, structs, savedir):
#     nbest = 10

#     ## nsmallest CSV table
#     for struct in structs:
#         if isRefinementStudy(study_df):
#             columns = [
#                 struct[0][1],
#                 f"O({struct[0][1]})",
#                 f"O_LOCAL({struct[0][1]})",
#             ]
#         else:
#             columns = [ 
#                     struct[0][1],
#                 ]
            
#         error_df = database.df_represantive_error_rows(study_df, struct[0])
#         result_df = database.database_smallest(error_df, columns, len(error_df))

#         # result_df.drop_duplicates([('database','CASE'),('database','M_TIME')], inplace=True)

#         list_ = list(zip(result_df[('database','CASE')], result_df[('database','M_TIME')]))


if __name__ == '__main__':
    main()