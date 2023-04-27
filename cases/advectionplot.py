import pandas as pd
import numpy as np
pd.set_option("display.precision", 8)
from matplotlib import pyplot as plt
import os
from math import pi, log
from matplotlib import rcParams
from numpy import polyfit
import glob, warnings

rcParams["text.usetex"] = True
rcParams["figure.dpi"] = 200
rcParams["font.size"] = 18

def get_studyname(study_pattern):
    studyname = study_pattern.rstrip("0123456789?[-]*")
    return studyname

# def agglomerate_dframe(study_pattern="",csv_filename="",dframe_name=""):

#     csv_files = glob.glob(os.path.join(study_pattern+'*', csv_filename))

#     csv_files.sort()
#     # Read all "csv_filename.csv" files into a pandas.DataFrame
#     dframes = []
#     for csv_file in csv_files:
#         dframes.append(pd.read_csv(csv_file, header=0)) 
#     final_dframe = pd.concat(dframes, ignore_index=True)
#     final_dframe.to_csv(dframe_name + ".csv", index=False)
#     return final_dframe

def agglomerate_dframe(study_pattern="",csv_filename="",dframe_name=""):

    cases = glob.glob(study_pattern+'*')
    cases.sort()
    agglomerate_dframe_2(cases,csv_filename,dframe_name)

def agglomerate_dframe_2(cases,csv_filename="",dframe_name=""):

    cases.sort()
    csv_files = []
    for case in cases:
        file = os.path.join(case, csv_filename)
        if os.path.isfile(file) and os.path.getsize(file):
            csv_files.append(file)
        else:
            warnings.warn(f'Skip {file}. Does not exis or is empty')
    # Read all "csv_filename.csv" files into a pandas.DataFrame
    if len(csv_files) == 0:
        return None
    dframes = []
    for csv_file in csv_files:
        dframes.append(pd.read_csv(csv_file, header=0)) 
    final_dframe = pd.concat(dframes, ignore_index=True)
    final_dframe.to_csv(dframe_name + ".csv", index=False)
    return final_dframe

def plot_marker_error(study_pattern, exact_volume, csv_filename="leiaSetFields.csv"):
        
        marker_dframe = agglomerate_dframe(study_pattern, csv_filename,
                                           dframe_name="marker")

        volume_error = abs(marker_dframe["VOL_ALPHA_0"] - exact_volume) / exact_volume 

        fig, ax = plt.subplots()

        ax.plot(marker_dframe["H"], volume_error, '-x', label="Marker field")
        h_01 = [marker_dframe["H"].iloc[0],marker_dframe["H"].iloc[-1]]
        error2nd_01 = [volume_error.iloc[0], 
        volume_error.iloc[0]*(h_01[1]/h_01[0])**2]
        error1st_01 = [volume_error.iloc[0], 
        volume_error.iloc[0]*(h_01[1]/h_01[0])]
        ax.plot(h_01,error2nd_01,"k--",label="second-order")
        ax.plot(h_01,error1st_01,"r:",label="first-order")
        ax.set_ylabel("$|V(t_0) - V_e|/|V_e|$")
        ax.set_xlabel("h")
        ax.legend()

        ax.loglog()
        xtick_values = marker_dframe["H"].to_numpy()
        xtick_labels = [f'${value:.1e}$' for value in xtick_values]
        ax.set_xticks(xtick_values, minor=False) 
        ax.set_xticklabels(xtick_labels)
        ax.minorticks_off()

        if (not os.path.exists("figures")):
            os.mkdir("figures")

        fig_name = study_pattern.split('_')[0] + "-marker-error-t0.pdf"
        fig_name = os.path.join("figures", fig_name)

        plt.savefig(fig_name, bbox_inches="tight")

def plot_advection_errors(study_pattern, 
                          csv_filename="leiaLevelSetFoam.csv", 
                          radius=0.15): 

    advection_dframe = agglomerate_dframe(study_pattern, csv_filename)

    if (not os.path.exists("data")):
        os.mkdir("data")

    study = get_studyname(study_pattern)
    study = study.replace("_", " ")

    advection_dframe.to_csv(os.path.join("data", study + "-dframe.csv"))

    resolutions = advection_dframe["DELTA_X"].unique()

    if (not os.path.exists("figures")):
        os.mkdir("figures")


    # Shared figure and axis objects 
    fig, ax = plt.subplots()

    # Ev OVER TIME PLOT
    Evmax = []
    time_min = 1e06 
    time_max = 0.
    for resolution in resolutions:
        advection_data = advection_dframe[advection_dframe["DELTA_X"] == resolution]
        Evmax.append(advection_data["E_VOL_ALPHA"].max())
        time_min = min(time_min, advection_data["TIME"].min())
        time_max = max(time_max, advection_data["TIME"].max())
        ax.plot(advection_data["TIME"], advection_data["E_VOL_ALPHA"], 
                label="%d cells / diameter " % (2*radius* (1. / resolution)))

    # First and last h values for convergence-order computation.
    h_01 = [advection_dframe["DELTA_X"].iloc[0],advection_dframe["DELTA_X"].iloc[-1]]

    title = "%s $Ev$" % study
    plt.title(title)
    ax.semilogy()
    ax.hlines(1e-03, time_min, time_max, 'k') 
    plt.xlabel("time in seconds")
    plt.ylabel("$E_v$")
    ax.legend()
    plt.savefig("./figures/%s-volume-conservation-evolution.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()

    # Ev MAX OVER DELTA_X PLOT 
    # Set xticks to resolutions for next plots

    fig, ax = plt.subplots()
    ax.loglog()
    xtick_values = resolutions
    xtick_labels = [f'${value:.1e}$' for value in resolutions]
    ax.set_xticks(xtick_values, minor=False) 
    ax.set_xticklabels(xtick_labels)
    ax.minorticks_off()

    plt.title(title)
    ax.plot(resolutions, Evmax, 'x-')

    Ev_error2nd_01 = [Evmax[0], Evmax[0]*(h_01[1]/h_01[0])**2]
    Ev_error1st_01 = [Evmax[0], Evmax[0]*(h_01[1]/h_01[0])]

    ax.plot(h_01,Ev_error2nd_01,"k--",label="second-order")
    ax.plot(h_01,Ev_error1st_01,"r:",label="first-order")

    plt.ylabel("$\max(Ev)$")
    plt.xlabel("$h$")
    ax.legend()
    plt.savefig("./figures/%s-volume-conservation-convergence.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()


    # Eg - DELTA_X plot
    Eg = []
    for resolution in resolutions:
        advection_data = advection_dframe[advection_dframe["DELTA_X"] == resolution]
        Eg.append(advection_data["E_GEOM_ALPHA"].iloc[-1])

    fig, ax = plt.subplots()
    
    plt.title("%s Eg" % study)
    plt.ylabel("$E_g$")
    plt.xlabel("$h$")
    ax.plot(resolutions, Eg, 'x-')

    convergence_coeffs = polyfit(np.log(resolutions), np.log(Eg), 1)
    print("Convergence order = %f " % convergence_coeffs[0])

    Eg_error2nd_01 = [Eg[0], Eg[0]*(h_01[1]/h_01[0])**2]
    Eg_error1st_01 = [Eg[0], Eg[0]*(h_01[1]/h_01[0])]

    ax.plot(h_01,Eg_error2nd_01,"k--",label="second-order")
    ax.plot(h_01,Eg_error1st_01,"r:",label="first-order")

    ax.loglog()
    xtick_values = resolutions
    xtick_labels = [f'${value:.1e}$' for value in resolutions]
    ax.set_xticks(xtick_values, minor=False) 
    ax.set_xticklabels(xtick_labels)
    ax.minorticks_off()

    ax.legend()
    plt.savefig("./figures/%s-geometric-error-convergence.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()

    # CFL / time plot
    maxCFL = []
    fig, ax = plt.subplots()
    for resolution in resolutions:
        advection_data = advection_dframe[advection_dframe["DELTA_X"] == resolution]
        maxCFL.append(advection_data["MAX_CFL"].iloc[-1])
        ax.plot(advection_data["TIME"], advection_data["MAX_CFL"], 
                label="%d cells / diameter " % (2*radius * (1 / resolution)))
    
    plt.title("%s CFL" % study)
    plt.ylabel("$CFL$")
    plt.xlabel("time in seconds")
    ax.legend()
    plt.savefig("./figures/%s-CFL-evolution.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()

def plot_gradpsi_errors(study_pattern, 
                          csv_filename="gradPsiError.csv",
                          radius=0.15): 

    # gradPsi_dframe = agglomerate_dframe(study_pattern, csv_filename)
    cases = glob.glob(os.path.join(study_pattern+'*', csv_filename))
    cases.sort()

    if (not os.path.exists("data")):
        os.mkdir("data")
        
    study = get_studyname(study_pattern)
    study = study.replace("_", " ")

    if (not os.path.exists("figures")):
        os.mkdir("figures")

    list_df = []
    list_mean_resolution = []
    list_max_E_gradPsi = []
    list_mean_E_gradPsi = []
    list_mean_E_narrowGradPsi = []
    list_max_E_narrowGradPsi = []
    for case in cases:
        list_df.append(pd.read_csv(case, header=0))
        list_mean_resolution.append(list_df[-1]['MEAN_DELTA_X'].mean())
        list_mean_E_gradPsi.append(list_df[-1]['MEAN_E_GRAD_PSI'].mean())
        list_max_E_gradPsi.append(list_df[-1]['MAX_E_GRAD_PSI'].max())
        list_mean_E_narrowGradPsi.append(list_df[-1]['MEAN_E_NARROW_GRAD_PSI'].mean())
        list_max_E_narrowGradPsi.append(list_df[-1]['MAX_E_NARROW_GRAD_PSI'].max())

    h_01 = [min(list_mean_resolution), max(list_mean_resolution)]

    # Shared figure and axis objects 
    fig, ax = plt.subplots()

    # E_gradPsi OVER TIME PLOT
    time_min = 1e06 
    time_max = 0.
    for df, resolution in zip(list_df, list_mean_resolution):
        time_min = min(time_min, df["TIME"].min())
        time_max = max(time_max, df["TIME"].max())
        ax.plot(df["TIME"], df["MEAN_E_GRAD_PSI"], 
                label="%d cells / diameter " % (2*radius* (1. / resolution)))

    title = "%s Error $gradPsi$" % study
    plt.title(title)
    ax.semilogy()
    ax.hlines(1e-03, time_min, time_max, 'k') 
    plt.xlabel("time in seconds")
    plt.ylabel("$ E_{gradPsi} $")
    ax.legend()
    plt.savefig("./figures/%s-grad-psi-evolution.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()

    # Shared figure and axis objects 
    fig, ax = plt.subplots()

    # E_narrow_gradPsi OVER TIME PLOT
    time_min = 1e06 
    time_max = 0.
    for df, resolution in zip(list_df, list_mean_resolution):
        time_min = min(time_min, df["TIME"].min())
        time_max = max(time_max, df["TIME"].max())
        ax.plot(df["TIME"], df["MEAN_E_NARROW_GRAD_PSI"], 
                label="%d cells / diameter " % (2*radius* (1. / resolution)))

    title = "%s Error $gradPsi$" % study
    plt.title(title)
    ax.semilogy()
    ax.hlines(1e-03, time_min, time_max, 'k') 
    plt.xlabel("time in seconds")
    plt.ylabel("$ E_{narrow-gradPsi} $")
    ax.legend()
    plt.savefig("./figures/%s-narrow-grad-psi-evolution.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()

    # E_gradPsi MAX OVER DELTA_X PLOT 
    # Set xticks to resolutions for next plots

    fig, ax = plt.subplots()
    ax.loglog()
    xtick_values = list_mean_resolution
    xtick_labels = [f'${value:.1e}$' for value in list_mean_resolution]
    ax.set_xticks(xtick_values, minor=False) 
    ax.set_xticklabels(xtick_labels)
    ax.minorticks_off()

    plt.title(title)
    ax.plot(list_mean_resolution, list_mean_E_gradPsi, 'x-')

    Ev_error2nd_01 = [list_mean_E_gradPsi[0], list_mean_E_gradPsi[0]*(h_01[1]/h_01[0])**2]
    Ev_error1st_01 = [list_mean_E_gradPsi[0], list_mean_E_gradPsi[0]*(h_01[1]/h_01[0])]

    ax.plot(h_01,Ev_error2nd_01,"k--",label="second-order")
    ax.plot(h_01,Ev_error1st_01,"r:",label="first-order")

    plt.ylabel(r"$\max(E_{gradPsi})$")
    plt.xlabel("$h$")
    ax.legend()
    plt.savefig("./figures/%s-grad-psi-convergence.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()

    # E_narrowGradPsi MAX OVER DELTA_X PLOT 
    # Set xticks to resolutions for next plots

    fig, ax = plt.subplots()
    ax.loglog()
    xtick_values = list_mean_resolution
    xtick_labels = [f'${value:.1e}$' for value in list_mean_resolution]
    ax.set_xticks(xtick_values, minor=False) 
    ax.set_xticklabels(xtick_labels)
    ax.minorticks_off()

    plt.title(title)
    ax.plot(list_mean_resolution, list_mean_E_narrowGradPsi, 'x-')

    Ev_error2nd_01 = [list_mean_E_narrowGradPsi[0], list_mean_E_narrowGradPsi[0]*(h_01[1]/h_01[0])**2]
    Ev_error1st_01 = [list_mean_E_narrowGradPsi[0], list_mean_E_narrowGradPsi[0]*(h_01[1]/h_01[0])]

    ax.plot(h_01,Ev_error2nd_01,"k--",label="second-order")
    ax.plot(h_01,Ev_error1st_01,"r:",label="first-order")

    plt.ylabel(r"$\max(E_{narrow-gradPsi})$")
    plt.xlabel("$h$")
    ax.legend()
    plt.savefig("./figures/%s-narrow-grad-psi-convergence.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()