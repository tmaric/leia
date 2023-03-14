#!/usr/bin/env python 

from argparse import ArgumentParser, RawTextHelpFormatter
import sys
from subprocess import run
from shlex import quote
import os
import os.path
import yaml


usage = """
    Script submits siulation cases inside a studydir to SLURM.
    Provide a the relative path to a SLURM script from inside a concrete case.
"""

def parse_arguments():
    parser = ArgumentParser(description=usage, formatter_class=RawTextHelpFormatter)

    parser.add_argument("studydir", 
                    help=""
                    )
    
    parser.add_argument("slurm_allrun_script", 
                    help="",
                    dest='allrun_script'
                    )
    
    return parser.parse_args()


def main():
    args = parse_arguments()

    basename_studydir = os.path.basename(os.path.abspath(args.studydir))
    studyinfofile = os.path.join(args.studydir, f"{basename_studydir}.info")
    with open(studyinfofile, 'r') as file:
        info = yaml.safe_load(file)

    os.chdir(args.studydir)

    cmd = f"""
    cat {info['casesfile']} | xargs -I[] bash -c 'cd [] && sbatch {args.allrun_script};  sleep 1s'
    """

    run(cmd, shell=True)


if __name__ == "__main__":
    main()