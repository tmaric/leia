#!/usr/bin/env python
# coding: utf-8

import plot_database
from argparse import ArgumentParser, RawTextHelpFormatter
import pandas as pd
import os

app_description = \
"""
Script and module for postprocessing database-CSV files. 
Creates timeplots, tables and convergence plots for specified error properties.
"""

def structs(template, study):
    # struct = (property, ylabel, title, figname)
    return [
        #     (
        #     ('case', 'E_NARROW_MEAN_GRAD_PSI'),
        #     r'$ mean(||\nabla \psi|-1|) $',
        #     template+r' signed-distance error in narrowBand',
        #     study+'_meanEGradPsi-narrowBand'
        # ),
        (
            ('case', 'E_GEOM_ALPHA'),
            r'$ E_g $',
            template+' hex-perturbed CN1'+r' $E_g = \sum_{c \in C} |\Omega_c||\alpha_c(t_e) - \alpha_c(t_0)|$',
            study+'_Eg'
        ),
        (
            ('case', 'E_VOL_ALPHA_REL'),
            r'$ E_v $',
            template+' hex-perturbed CN1'+r' $E_v(t)  = \frac{\left|\sum_{c \in C}\alpha_c(t)|\Omega_c| - \sum_{c \in C} \alpha_c(t_0)|\Omega_c|\right|}{\left|\sum_{c \in C} \alpha_c(t_0)|\Omega_c|\right|}$',
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

    study_df = study_df[study_df[('studyparameters','PROFILE')] == 'signedDistance']
    study_df.drop(('studyparameters','PROFILE'), axis='columns', inplace=True)
    
    assert study_df.index.is_unique, "Index of study_df is not unique! Would cause errors."

    savedir = os.path.dirname(study_csv)
    # savedir = f"/home/julian/Masterthesis/meetings/next/{study}"
    # savedir = f"/home/julian/.local/share/Trash/files/{study}"

    os.makedirs(savedir, exist_ok=True)

    mystructs = structs(template, study)
    plot_database.runall(study_df, mystructs, savedir)



if __name__ == '__main__':
    main()