#!/usr/bin/env python 

from argparse import ArgumentParser, RawTextHelpFormatter
import sys
from subprocess import call, run
from shlex import quote
import os
import os.path


usage = """
    Script starts the simulation of a parameter study with foamJob and a provided Allrun-script inside every case directory.
"""

def parse_arguments():
    parser = ArgumentParser(description=usage, formatter_class=RawTextHelpFormatter)

    # parser.add_argument('-m', '--mesh',
    #                     choices=['hex', 'perturbed', 'poly'],
    #                     help="Meshtype: hexahedral, hexahedral-perturbed or polyhedral",
    #                     default='hex',
    #                     required=False,
    #                     dest='meshtype',
    #                     )

    parser.add_argument("studydir", 
                    help=""
                    )
    
    parser.add_argument("allrun_script", 
                    help=""
                    )
    
    return parser.parse_args()
    

def main():
    args = parse_arguments()
    meta_name = os.path.basename(os.path.abspath(args.studydir))

    os.chdir(args.studydir)

    cmd = f"""
    cat {meta_name}.cases | xargs -I[] bash -c 'cd [] && foamJob ./{args.allrun_script} && sleep 1s'
    """

    run(cmd, shell=True)


if __name__ == "__main__":
    main()