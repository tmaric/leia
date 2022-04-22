import pandas as pd
import numpy as np
pd.set_option("display.precision", 8)
from matplotlib import pyplot as plt
import os
from math import pi, log
from matplotlib import rcParams
from numpy import polyfit

rcParams["text.usetex"] = True
rcParams["figure.dpi"] = 200
rcParams["font.size"] = 18


def agglomerate_dframe(study_pattern="",csv_filename="",dframe_name=""):

    # Find all "csv_filename.csv" files in study_pattern*/ folders
    csv_files = [os.path.join(folder, csv_filename) 
                 for folder in os.listdir(os.curdir) 
                 if os.path.isfile(os.path.join(folder, csv_filename))
                 and study_pattern in folder]
    csv_files.sort()
    # Read all "csv_filename.csv" files into a pandas.DataFrame
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

        plt.plot(marker_dframe["H"], volume_error, '-x', label="Marker field")
        h_01 = [marker_dframe["H"].iloc[0],marker_dframe["H"].iloc[-1]]
        error2nd_01 = [volume_error.iloc[0], 
        volume_error.iloc[0]*(h_01[1]/h_01[0])**2]
        error1st_01 = [volume_error.iloc[0], 
        volume_error.iloc[0]*(h_01[1]/h_01[0])]
        plt.plot(h_01,error2nd_01,"k--",label="second-order")
        plt.plot(h_01,error1st_01,"r:",label="first-order")
        plt.ylabel("$|V(t_0) - V_e|/|V_e|$")
        plt.xlabel("h")
        plt.legend()
        plt.loglog()

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

    study = study_pattern.rstrip("0")
    study = study.replace("_", " ")

    advection_dframe.to_csv(os.path.join("data", study + "-dframe.csv"))

    resolutions = advection_dframe["DELTA_X"].unique()

    if (not os.path.exists("figures")):
        os.mkdir("figures")


    # Ev max plot
    Evmax = []
    for resolution in resolutions:
        advection_data = advection_dframe[advection_dframe["DELTA_X"] == resolution]
        Evmax.append(advection_data["E_VOL_ALPHA"].max())
        plt.plot(advection_data["TIME"], advection_data["E_VOL_ALPHA"], 
                 label="%d cells / diameter " % (2*radius* (1 / resolution)))

    # First and last h values for convergence-order computation.
    h_01 = [advection_dframe["DELTA_X"].iloc[0],advection_dframe["DELTA_X"].iloc[-1]]

    title = "%s $Ev$" % study
    plt.title(title)
    plt.xlabel("time in seconds")
    plt.ylabel("$E_v$")
    plt.legend()
    plt.savefig("%s-volume-conservation-evolution.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.savefig("./figures/%s-volume-conservation-evolution.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()

    plt.title(title)
    plt.plot(resolutions, Evmax, 'x-')

    Ev_error2nd_01 = [Evmax[0], Evmax[0]*(h_01[1]/h_01[0])**2]
    Ev_error1st_01 = [Evmax[0], Evmax[0]*(h_01[1]/h_01[0])]

    plt.plot(h_01,Ev_error2nd_01,"k--",label="second-order")
    plt.plot(h_01,Ev_error1st_01,"r:",label="first-order")

    plt.loglog()
    plt.ylabel("$\max(Ev)$")
    plt.xlabel("$h$")
    plt.legend()
    plt.savefig("%s-volume-conservation-convergence.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()


    Eg = []
    for resolution in resolutions:
        advection_data = advection_dframe[advection_dframe["DELTA_X"] == resolution]
        Eg.append(advection_data["E_GEOM_ALPHA"].iloc[-1])
    
    plt.loglog()
    plt.title("%s Eg" % study)
    plt.ylabel("$E_g$")
    plt.xlabel("$h$")
    plt.plot(resolutions, Eg, 'x-')


    convergence_coeffs = polyfit(np.log(resolutions), np.log(Eg), 1)
    print("Convergence order = %f " % convergence_coeffs[0])

    Eg_error2nd_01 = [Eg[0], Eg[0]*(h_01[1]/h_01[0])**2]
    Eg_error1st_01 = [Eg[0], Eg[0]*(h_01[1]/h_01[0])]

    plt.plot(h_01,Eg_error2nd_01,"k--",label="second-order")
    plt.plot(h_01,Eg_error1st_01,"r:",label="first-order")

    plt.savefig("%s-geometric-error-convergence.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.savefig("./figures/%s-geometric-error-convergence.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.legend()
    plt.show()


    maxCFL = []
    for resolution in resolutions:
        advection_data = advection_dframe[advection_dframe["DELTA_X"] == resolution]
        maxCFL.append(advection_data["MAX_CFL"].iloc[-1])
        plt.plot(advection_data["TIME"], advection_data["MAX_CFL"], 
                label="%d cells / diameter " % (2*radius * (1 / resolution)))
    
    plt.title("%s CFL" % study)
    plt.ylabel("$CFL$")
    plt.xlabel("time in seconds")
    plt.legend()
    plt.savefig("%s-CFL-evolution.pdf" % study.replace(" ", "") , bbox_inches='tight')
    plt.show()
