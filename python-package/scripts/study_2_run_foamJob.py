#!/usr/bin/env python 

from argparse import ArgumentParser, RawTextHelpFormatter
import sys
from subprocess import run
from shlex import quote
import os
import os.path
import yaml


usage = """
    Script starts the simulations of a parameter study with foamJob and a provided Allrun-script inside every case directory.
"""

def parse_arguments():
    parser = ArgumentParser(description=usage, formatter_class=RawTextHelpFormatter)


    parser.add_argument("studydir", 
                    help=""
                    )
    
    parser.add_argument("allrun_script", 
                    help=""
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
    cat {info['casesfile']} | xargs -I[] bash -c 'cd [] && foamJob ./{args.allrun_script};  sleep 1s'
    """
    run(cmd, shell=True)


if __name__ == "__main__":
    main()