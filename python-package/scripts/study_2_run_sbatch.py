#!/usr/bin/env python 

from argparse import ArgumentParser, RawTextHelpFormatter
import sys
from subprocess import run
from shlex import quote
import os
import os.path
import leia


usage = """
Submitts the simulations of the STUDYDIR study to Slurm using `sbatch`.
It submits the specified Slurm ALLRUN script for all cases listed in <STUDYDIR>.cases.
The ALLRUN script is in each case.
"""

def parse_arguments():
    parser = ArgumentParser(description=usage, formatter_class=RawTextHelpFormatter)

    parser.add_argument("studydir",  
                    help="Study directory with the template case, parameter file and the info file inside.",
                    metavar='STUDYDIR')
    
    parser.add_argument("allrun", 
                    help="Slurm script that will be submitted and contains preprocessing, mesh building and execution.",
                    metavar='ALLRUN')
    
    return parser.parse_args()


def main():
    args = parse_arguments()

    basename_studydir = os.path.basename(os.path.abspath(args.studydir))
    studyinfofile = os.path.join(args.studydir, f"{basename_studydir}.info")
    info = leia.io.read_info(studyinfofile)

    os.chdir(args.studydir)

    cmd = f"""
    cat {info['casesfile']} | xargs -I[] bash -c 'cd [] && sbatch {args.allrun};  sleep 1s'
    """

    run(cmd, shell=True)


if __name__ == "__main__":
    main()