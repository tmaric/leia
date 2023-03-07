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
from dataclasses import dataclass
rcParams["text.usetex"] = True
rcParams["figure.dpi"] = 200
rcParams["font.size"] = 14

app_description = \
"""
Script and module for postprocessing database-CSV files. 
Creates timeplots, tables and convergence plots for specified error properties.
"""



@dataclass(kw_only=True)
class Fig:
    title: str
    ylabel: str
    xlabel: str
    figname: str

@dataclass(kw_only=True)
class Prop:
    column: tuple
    template: str
    study: str
    titlestr: str
    figstr: str
    labelstr: str
    formula: str = ''
    mesh: str = ''

    def __post_init__(self):
        self.title = ' '.join([self.template, self.titlestr, self.formula])
        self.figname= '_'.join([self.study])
        self.figTime = Fig(
            title=' '.join([self.template, self.mesh, self.titlestr, self.formula]),
            ylabel=' '.join([self.labelstr]),
            xlabel='time in s',
            figname='_'.join([self.study, self.figstr, 'timeplot'])
        )
        self.figConv = Fig(
            title=' '.join([self.template, self.mesh, self.titlestr, self.formula]),
            ylabel=' '.join([self.labelstr]),
            xlabel=r'$h$ in m',
            figname='_'.join([self.study, self.figstr, 'convergenceplot'])
        )
        self.figBestConv = Fig(
            title=' '.join([self.template, self.mesh, self.titlestr, self.formula]),
            ylabel=' '.join([self.labelstr]),
            xlabel=r'$h$ in m',
            figname='_'.join([self.study, self.figstr, 'best-convergenceplot'])
        )

def property_dict(template, study, mesh=''):
    return {

        "E_NARROW_MEAN_GRAD_PSI":
            Prop(
                column = ('case', 'E_NARROW_MEAN_GRAD_PSI'),
                template = template,
                study = study,
                titlestr = 'signed-distance error in narrowBand',
                figstr = 'EMeanGradPsi-narrowBand',
                labelstr = r'$ E_{\nabla\psi} $',
                formula = r'$E_{\nabla\psi} = mean(||\nabla \psi|-1|) $',
                mesh=mesh,   
            ),

        "E_GEOM_ALPHA":
            Prop(
                column = ('case', 'E_GEOM_ALPHA'),
                template = template,
                study = study,
                titlestr = 'geometrical error',
                figstr = 'Eg',
                labelstr = r'$ E_{g} $',
                formula = r' $E_g = \sum_{c \in C} |\Omega_c||\alpha_c(t_e) - \alpha_c(t_0)|$',
                mesh=mesh,   
            ),
        

        "E_VOL_ALPHA_REL":
            Prop(
                column = ('case', 'E_VOL_ALPHA_REL'),
                template = template,
                study = study,
                titlestr = 'volume conservation error',
                figstr = 'Ev',
                labelstr = r'$ E_{v} $',
                formula = r'$E_v(t) = \frac{\left|\sum_{c \in C}\alpha_c(t)|\Omega_c| - \sum_{c \in C} \alpha_c(t_0)|\Omega_c|\right|}{\left|\sum_{c \in C} \alpha_c(t_0)|\Omega_c|\right|}$',
                mesh=mesh,   
            ),
    }

def time_property_dict(template, study, mesh=''):
    return {

        'MAX_CFL':
            Prop(
                column = ('case', 'MAX_CFL'),
                template = template,
                study = study,
                titlestr = 'max(CFL)',
                figstr = 'CFLmax',
                labelstr = r'$ CFL $',
                formula = '',
                mesh=mesh,   
            ),
    }

def filter_studydf(study_df, column, value):
    study_df = study_df.loc[study_df[column] == value]
    study_df = study_df.drop(column, axis='columns', inplace=False)
    return study_df

def main():
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)
    parser.add_argument("studyCSV", help="The database-CSV file to postprocess.")
   
    parser.add_argument('--mesh',
                        choices=['hex', 'hex-perturbed', 'poly'],
                        help="Adds mesh type information into figure titles."\
                            + "\nMeshtype: hexahedral, hexahedral-perturbed or polyhedral",
                        default='',
                        required=False,
                        dest='mesh',
                        )
    
    parser.add_argument('--filter',
                        help="Drop a whole column and keep rows which match <value>" \
                            + "\nExpects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
                        required=False,
                        action='append',
                        nargs=3, 
                        )
    
    args = parser.parse_args()

    study_csv = args.studyCSV

    template = os.path.basename(study_csv).split('_')[1]
    study = os.path.basename(study_csv).rpartition('_')[0]

    study_df = pd.read_csv(study_csv, header=[0,1])

    if args.filter is not None:
        for fi in args.filter:
            study_df = filter_studydf(study_df, column=(fi[0], fi[1]), value=fi[2])

    
    assert study_df.index.is_unique, "Index of study_df is not unique! Would cause errors."

    savedir = os.path.abspath(os.path.dirname(study_csv))
    # savedir = f"/home/julian/Masterthesis/meetings/next/{study}"
    # savedir = f"/home/julian/.local/share/Trash/files/{study}"

    os.makedirs(savedir, exist_ok=True)

    properties = property_dict(template, study, mesh=args.mesh)
    runall(study_df, properties, savedir)

    # run just timeplot for some properties
    timeplot(study_df, time_property_dict(template, study, mesh=args.mesh), savedir)


def runall(study_df, properties, savedir):
    timeplot(study_df, properties, savedir)
    nsmallest_table(study_df, properties, savedir)
    convergenceplot(study_df, properties, savedir)
    best_convergenceplot(study_df, properties, savedir)


def timeplot(study_df, properties, savedir):
    refinement_label = studycsv.get_refinementlabel(study_df)
    if refinement_label is not None:
        for prop in properties.values():
            for resolution, resolution_df in study_df.groupby(refinement_label, sort=False):
                # groupby CASE and M_TIME for concatenated databases where cases could have the same basename
                grouped_resolution_df_ls = plot.group_DataFrame(resolution_df, by=[('database','CASE'),('database','M_TIME')], maxnitems=10)
                ls_len = len(grouped_resolution_df_ls)
                for fig_number, group_df in enumerate(grouped_resolution_df_ls):
                    fig = plot.timeplot(group_df, prop)
                    fig.savefig(os.path.join(savedir, f'{prop.figTime.figname}_{refinement_label[1]}-{resolution}_{fig_number+1}-{ls_len}.jpg'), bbox_inches='tight')
                    fig.savefig(os.path.join(savedir, f'{prop.figTime.figname}_{refinement_label[1]}-{resolution}_{fig_number+1}-{ls_len}.pdf'), bbox_inches='tight')
                    plt.close(fig)
    else:
        for prop in properties.values():
            grouped_study_df_ls = plot.group_DataFrame(study_df, by=[('database','CASE'),('database','M_TIME')], maxnitems=10)
            ls_len = len(grouped_study_df_ls)
            for fig_number, group_df in enumerate(grouped_study_df_ls):
                fig = plot.timeplot(group_df, prop)
                fig.savefig(os.path.join(savedir, f'{prop.figTime.figname}_{fig_number+1}-{ls_len}.jpg'), bbox_inches='tight')
                fig.savefig(os.path.join(savedir, f'{prop.figTime.figname}_{fig_number+1}-{ls_len}.pdf'), bbox_inches='tight')
                plt.close(fig)


def nsmallest_table(study_df, properties, savedir):
    ## nsmallest CSV table
    refinement_label = studycsv.get_refinementlabel(study_df)
    
    for prop in properties.values():
        if refinement_label is not None:
            columns = [
                prop.column[1],
                f"O({prop.column[1]})",
                f"O_LOCAL({prop.column[1]})",
            ]
        else:
            columns = [ 
                    prop.column[1],
                ]
            
        error_df = database.df_represantive_error_rows(study_df, prop.column)
        result_df = database.database_smallest(error_df, columns, len(error_df))
        result_df.to_csv(os.path.join(savedir, '_'.join([prop.study, prop.figstr, 'nsmallest.csv'])), index=False)


def convergenceplot(study_df, properties, savedir):
    ## Convergence Plot
    refinement_label = studycsv.get_refinementlabel(study_df)

    if refinement_label is not None:
        mi = study_df.columns
        studyparameters = list(mi[mi.get_loc_level('studyparameters')[0]])
        studyparameters.remove(refinement_label)

        for prop in properties.values():
            grouped_study_df_ls = plot.group_DataFrame(study_df, by=studyparameters, maxnitems=10)
            ls_len = len(grouped_study_df_ls)
            for fig_number, group_df in enumerate(grouped_study_df_ls):
                fig = plot.convergenceplot(group_df, prop) 
                fig.savefig(os.path.join(savedir, f'{prop.figConv.figname}_{fig_number+1}-{ls_len}.jpg'), bbox_inches='tight')
                fig.savefig(os.path.join(savedir, f'{prop.figConv.figname}_{fig_number+1}-{ls_len}.pdf'), bbox_inches='tight')
                plt.close(fig)

def best_convergenceplot(study_df, properties, savedir):
    nbest = 10

    ## Convergence Plot
    refinement_label = studycsv.get_refinementlabel(study_df)

    if refinement_label is None:
        return None
    mi = study_df.columns
    studyparameters = list(mi[mi.get_loc_level('studyparameters')[0]])
    studyparameters.remove(refinement_label)
    for prop in properties.values():
        ref_gb_ls = studycsv.smallest_refinement_gb(study_df, by=prop.column)
        if len(ref_gb_ls) > nbest:
            ref_gb_ls = ref_gb_ls[:nbest]

        best_study_df = pd.concat(map(lambda item: item[1], ref_gb_ls), ignore_index=False)
        
        fig = plot.convergenceplot(best_study_df, prop) 
        fig.savefig(os.path.join(savedir, f'{prop.figBestConv.figname}.jpg'), bbox_inches='tight')
        fig.savefig(os.path.join(savedir, f'{prop.figBestConv.figname}.pdf'), bbox_inches='tight')
        plt.close(fig)






if __name__ == '__main__':
    main()