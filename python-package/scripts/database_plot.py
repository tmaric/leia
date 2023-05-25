#!/usr/bin/env python
# coding: utf-8

# In[1]:


import pandas as pd
import os.path
import os
import re
from matplotlib import rcParams
import matplotlib.pyplot as plt
from leia import database, plot, studycsv
import leia
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

# @dataclass(kw_only=True) # Uncomment if python/3.10 is available on Lichtenberg
@dataclass()
class Fig:
    title: str
    ylabel: str
    xlabel: str
    figname: str

# @dataclass(kw_only=True) # Uncomment if python/3.10 is available on Lichtenberg
@dataclass()
class Prop:
    column: tuple
    template: str
    study: str
    titlestr: str
    figstr: str
    labelstr: str
    labelstr_conv: str = None
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
                titlestr = 'signed distance error',
                figstr = 'EMeanGradPsi-narrowBand',
                labelstr = r'$ E_{\nabla\psi}(t) $',
                formula = r'$E_{\nabla\psi}(t) = \frac{1}{N_{C_{narrow}}} \sum_{c \in C_{narrow}}(|\|(\nabla \psi)_c(t)\|_2-1|)$',
                mesh=mesh,
                labelstr_conv = r"$ \max_{t \in T_h} E_{\nabla\psi}(t) $"   
            ),

        "E_GEOM_ALPHA":
            Prop(
                column = ('case', 'E_GEOM_ALPHA'),
                template = template,
                study = study,
                titlestr = 'geometrical error',
                figstr = 'Eg',
                labelstr = r'$ E_{g} $',
                formula = r' $E_g = \sum_{c \in C} |\Omega_c||\alpha_c(t^{end}) - \alpha_c(t^0)|$',
                mesh=mesh,   
            ),
        

        "E_VOL_ALPHA_REL":
            Prop(
                column = ('case', 'E_VOL_ALPHA_REL'),
                template = template,
                study = study,
                titlestr = 'volume conservation error',
                figstr = 'Ev',
                labelstr = r'$ E_{v}(t) $',
                formula = r'$E_v(t) = \frac{\left|\sum_{c \in C}\alpha_c(t)|\Omega_c| - \sum_{c \in C} \alpha_c(t^0)|\Omega_c|\right|}{\left|\sum_{c \in C} \alpha_c(t^0)|\Omega_c|\right|}$',
                mesh=mesh,
                labelstr_conv = r"$ \max_{t \in T_h} E_v(t) $"   
            ),

        "E_VOL_ALPHA_REL_TV":
            Prop(
                column = ('case', 'E_VOL_ALPHA_REL_TV'),
                template = template,
                study = study,
                titlestr = 'TV of volume conservation error',
                figstr = 'EvTV',
                labelstr = r'$TV(E_v(t^n))$',
                formula = r'$TV(E_v(t^n)) = \sum_{k=0}^n |E_v(t^k) - E_v(t^{k-1})|$',
                mesh=mesh,
                labelstr_conv = r"$TV(E_v(t^{end}))$"
            ),

        "E_VOL_ALPHA_REL_TVtime":
            Prop(
                column = ('case', 'E_VOL_ALPHA_REL_TVtime'),
                template = template,
                study = study,
                titlestr = 'TV of volume conservation error',
                figstr = 'EvTVtime',
                labelstr = r'$TV(E_v(t^n))$',
                formula = r'$TV(E_v(t^n)) = \sum_{k=0}^n \frac{|E_v(t^k) - E_v(t^{k-1})|}{\delta t}$',
                mesh=mesh,
                labelstr_conv = r"$TV(E_v(t^{end}))$"
            ),

        "E_TV":
            Prop(
                column = ('case', 'E_TV'),
                template = template,
                study = study,
                titlestr = 'total variation error',
                figstr = 'Etv',
                labelstr = r'$ E_{TV}(t) $',
                formula = r'$ E_{TV}(t) = TV(t)-TV(t^0) $, $ TV = \sum_f |\psi_N - \psi_O| $',
                mesh=mesh,
                labelstr_conv = r"$ |E_{TV}(t^{end})| $"   
            ),

        "E_TV_REL":
            Prop(
                column = ('case', 'E_TV_REL'),
                template = template,
                study = study,
                titlestr = 'relative total variation error',
                figstr = 'Etv-rel',
                labelstr = r'$ E_{TV}(t) $',
                formula = r'$ E_{TV}(t) = \frac{TV(t)-TV(t_0)}{TV(t^0)} $, $ TV = \sum_f |\psi_N - \psi_O| $',
                mesh=mesh,   
                labelstr_conv = r"$ |E_{TV}(t^{end})| $"   
            ),

        "max_error_velocity":
            Prop(
                column = ('case', 'max_error_velocity'),
                template = template,
                study = study,
                titlestr = 'maximal velocity ',
                figstr = 'E-umax',
                labelstr = r'$ \max(|\mathbf{U(t)}|) $ in m/s',
                formula = r'$ \max(|\mathbf{U}|) $',
                mesh=mesh,   
                labelstr_conv = r'$ \max(|\mathbf{U(t^{end})}|) $ in m/s',
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
    study_df.reset_index()
    return study_df



def check_properties_in_studydf(properties, study_df):
    properties = dict(filter(lambda item: item[0] in study_df.columns.levels[1], properties.items()))
    return properties

def runall(study_df, properties, savedir, **kwargs):
    timeplot(study_df, properties, savedir, **kwargs)
    nsmallest_table(study_df, properties, savedir, **kwargs)
    convergenceplot(study_df, properties, savedir, **kwargs)
    best_convergenceplot(study_df, properties, savedir, **kwargs)


def timeplot(study_df, properties, savedir, **kwargs):
    refinement_label = studycsv.get_refinementlabel(study_df)
    if refinement_label is not None:
        for prop in properties.values():
            for resolution, resolution_df in study_df.groupby(refinement_label, sort=False):
                # groupby CASE and M_TIME for concatenated databases where cases could have the same basename
                grouped_resolution_df_ls = plot.group_DataFrame(resolution_df, by=[('database','CASE'),('database','M_TIME')], maxnitems=10)
                ls_len = len(grouped_resolution_df_ls)
                for fig_number, group_df in enumerate(grouped_resolution_df_ls):
                    fig = plot.timeplot(group_df, prop, **kwargs)
                    fig.savefig(os.path.join(savedir, f'{prop.figTime.figname}_{refinement_label[1]}-{resolution}_{fig_number+1}-{ls_len}.jpg'), bbox_inches='tight')
                    fig.savefig(os.path.join(savedir, f'{prop.figTime.figname}_{refinement_label[1]}-{resolution}_{fig_number+1}-{ls_len}.pdf'), bbox_inches='tight')
                    plt.close(fig)
    else:
        for prop in properties.values():
            grouped_study_df_ls = plot.group_DataFrame(study_df, by=[('database','CASE'),('database','M_TIME')], maxnitems=10)
            ls_len = len(grouped_study_df_ls)
            for fig_number, group_df in enumerate(grouped_study_df_ls):
                fig = plot.timeplot(group_df, prop, **kwargs)
                fig.savefig(os.path.join(savedir, f'{prop.figTime.figname}_{fig_number+1}-{ls_len}.jpg'), bbox_inches='tight')
                fig.savefig(os.path.join(savedir, f'{prop.figTime.figname}_{fig_number+1}-{ls_len}.pdf'), bbox_inches='tight')
                plt.close(fig)


def nsmallest_table(study_df, properties, savedir, **kwargs):
    ## nsmallest CSV table
    refinement_label = studycsv.get_refinementlabel(study_df)
    
    for prop in properties.values():
        if refinement_label is not None:
            columns = [
                prop.column,
                ('case', f"O({prop.column[1]})"),
                ('case', f"O_LOCAL({prop.column[1]})"),
            ]
        else:
            columns = [ 
                    prop.column,
                ]
            
        error_df = database.df_represantive_error_rows(study_df, prop.column)
        result_df = database.database_smallest(error_df, columns, len(error_df))
        result_df.to_csv(os.path.join(savedir, '_'.join([prop.study, prop.figstr, 'nsmallest.csv'])), index=False)


def convergenceplot(study_df, properties, savedir, **kwargs):
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
                fig = plot.convergenceplot(group_df, prop, **kwargs) 
                fig.savefig(os.path.join(savedir, f'{prop.figConv.figname}_{fig_number+1}-{ls_len}.jpg'), bbox_inches='tight')
                fig.savefig(os.path.join(savedir, f'{prop.figConv.figname}_{fig_number+1}-{ls_len}.pdf'), bbox_inches='tight')
                # fig.savefig(os.path.join(savedir, f'{prop.figConv.figname}_{fig_number+1}-{ls_len}.jpg'))
                # fig.savefig(os.path.join(savedir, f'{prop.figConv.figname}_{fig_number+1}-{ls_len}.pdf'))
                plt.close(fig)

def best_convergenceplot(study_df, properties, savedir, **kwargs):
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
        
        fig = plot.convergenceplot(best_study_df, prop, **kwargs) 
        fig.savefig(os.path.join(savedir, f'{prop.figBestConv.figname}.jpg'), bbox_inches='tight')
        fig.savefig(os.path.join(savedir, f'{prop.figBestConv.figname}.pdf'), bbox_inches='tight')
        plt.close(fig)


plot_func_dict = {
    'time'      : timeplot,
    'conv'      : convergenceplot,
    'bestconv'  : best_convergenceplot,
    'table'     : nsmallest_table,
}

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

    parser.add_argument('--plot',
                        # choices=['time', 'loglogtime', 'table', 'conv', 'bestconv'],
                        choices=plot_func_dict.keys(),
                        help="Plots to choose from. Default: plot all.",
                        default=plot_func_dict.keys(),
                        nargs='*',
                        required=False,
                        )
    
    parser.add_argument('--method',
                        choices=['plot', 'semilogy', 'loglog'],
                        help="Plotting method for time plots.",
                        default=None,
                        required=False,
                        )

    parser.add_argument('--legend',
                        choices=['below', 'right'],
                        help="Legend position different from default.",
                        default=None,
                        required=False,
                        )
 
    parser.add_argument('--cmap',
                        help="Matplotlib colormap for lines in convergenceplot. Default 'tab10'",
                        default='tab10',
                        required=False,
                        )

    parser.add_argument('-s','--sorted',
                        help="Sort the lines in the convergence plots according the the studyparameters before plotting",
                        action='store_true',
                        required=False,
                        )

    parser.add_argument('--deltaX',
                        choices=['min', 'max', 'mean'],
                        help="Characteristic grid spacing h / deltaX for convergence plots. Default 'mean'",
                        default='mean',
                        required=False,
                        )

    parser.add_argument('--rm',
                       help="Removes all rows matching the value. Expects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
                       action='append',
                       nargs=3,                   
                       )
    
    parser.add_argument('--keep', 
                       help="Removes all rows not matching the value. Expects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
                       action='append',
                       nargs=3, 
                        )
    
    parser.add_argument('--keep-drop', 
                       help="Removes all rows not matching the value and drops the column. Expects 3 parameters: <1-lvl column name> <2-lvl column name> <value>",
                       action='append',
                       nargs=3,
                       dest='keepdrop' 
                        )
    
    parser.add_argument('-d','--savedir',
                        help="Directory where plots are saved. Default: dirname(studyCSV)",
                        required=False,
                        default=None,
                        )
    
    args = parser.parse_args()

    study_csv = args.studyCSV
    template = os.path.basename(study_csv).split('_')[1]
    study = os.path.basename(study_csv).rpartition('_')[0]
    study_df = pd.read_csv(study_csv, header=[0,1])
    assert study_df.index.is_unique, "Index of study_df is not unique! Would cause errors."


    def column(ls: list):
        return (ls[0], ls[1])
    if args.rm:
        if isinstance(args.rm[0], list): # then multiple --rm are passed
            rms = args.rm
        else:
            rms = [args.rm]
        for rm in rms: 
            study_df = studycsv.filter_rm(study_df, column(rm), rm[2])
    if args.keep:
        if isinstance(args.keep[0], list): # then multiple --keep are passed
            keeps = args.keep
        else:
            keeps = [args.keep]
        for keep in keeps: 
            study_df = studycsv.filter_keep(study_df, column(keep), keep[2], drop=False)
    if args.keepdrop:
        if isinstance(args.keepdrop[0], list): # then multiple --keep are passed
            keeps = args.keepdrop
        else:
            keeps = [args.keepdrop]
        for keep in keeps: 
            study_df = studycsv.filter_keep(study_df, column(keep), keep[2], drop=True)
    study_df.reset_index(drop=True)

    if args.savedir is None:
        args.savedir = os.path.abspath(os.path.dirname(study_csv))
    os.makedirs(args.savedir, exist_ok=True)

    kwargs = dict()
    kwargs['legend'] = args.legend
    kwargs['cmap'] = args.cmap
    if args.sorted:
        kwargs['sorted'] = True
    
    if args.deltaX:
        map_ = {
            'min': ('case','DELTA_X'),
            'max': ('case','MAX_DELTA_X'),
            'mean': ('case','MEAN_DELTA_X'),
        }
        kwargs['deltaX'] = map_[args.deltaX]

    if args.method:
        kwargs['method'] = args.method

    study_df = leia.derived_properties.append_TV(study_df, ('case','E_VOL_ALPHA_REL'), ('case','E_VOL_ALPHA_REL_TV'))
    # study_df = leia.derived_properties.append_TVtime(study_df, ('case','E_VOL_ALPHA_REL'), ('case','E_VOL_ALPHA_REL_TVtime'))
    
    properties = property_dict(template, study, mesh=args.mesh)
    properties = check_properties_in_studydf(properties, study_df)

    if 'table' in args.plot and not any(map(lambda col: bool(re.match('O_.*', col[1])), study_df.columns)):
        print('Calc convergence') 
        study_df = leia.convergence.add_convergencerates(
            study_df, 
            studyparameters = study_df.columns[study_df.columns.get_loc('studyparameters')], 
            refinement_parameter = studycsv.get_refinementlabel(study_df), 
            propertylabels = list(filter(database.isErrorColumn, study_df.columns)), 
            h_label = kwargs['deltaX'],
            time_label = ('case', 'TIME')
            )

    if 'time' in args.plot:
        timeplot(study_df, time_property_dict(template, study, mesh=args.mesh), args.savedir, **kwargs)
    for plot_str in args.plot:
        plot_func = plot_func_dict[plot_str]
        plot_func(study_df, properties, args.savedir, **kwargs)


if __name__ == '__main__':
    main()