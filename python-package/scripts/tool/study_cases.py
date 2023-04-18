#!/usr/bin/env python
# coding: utf-8

from subprocess import run
from shlex import quote
from argparse import ArgumentParser, RawTextHelpFormatter
import sys
import os.path
import yaml
import json

app_description = \
"""
Lists cases of a study.
Default: Get cases from casesfile inside <STUDYDIR>/<STUDYDIR>.info
"""

def parse_arguments():
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)
    parser.add_argument("STUDYDIR",
                        help="Study directory",
                        default='./'
                        )

    group = parser.add_mutually_exclusive_group(required=False)
    group.add_argument('--from-studydir', 
                       action='store_true',
                       help="Print present cases of the top level study directory."
                       )
    group.add_argument('--from-json', 
                       action='store_true',
                       help="Print cases which are listed inside the case-variation json file."
                       )
    
    args = parser.parse_args()
    assert os.path.isdir(args.STUDYDIR), f"{args.STUDYDIR} is no directory!"
    return args

def default(casesfile):
    return run(f"cat {casesfile}", shell=True).returncode

def from_studydir(studydir):
    return run(f'cd {studydir} && find -maxdepth 1 -type d -name "*_[0-9][0-9][0-9][0-9][0-9]_*" | sort | xargs -n1 basename',
        shell=True
        ).returncode
    
def from_json(jsonfile):
    with open(jsonfile, 'r') as file:
        jsondict = json.load(file)
    print(*jsondict.keys(), sep='\n')
    return 0

def main():
    args = parse_arguments()

    if args.from_studydir:
        status = from_studydir(args.STUDYDIR)
        sys.exit(status)

    basename_studydir = os.path.basename(os.path.abspath(args.STUDYDIR))
    args.infofile = os.path.join(args.STUDYDIR, f"{basename_studydir}.info")
    with open(args.infofile, 'r') as file:
        info = yaml.safe_load(file)
    
    if args.from_json:
        status = from_json(os.path.join(args.STUDYDIR, info['json_variationfile']))
        sys.exit(status)
    else: #default
        status = default(os.path.join(args.STUDYDIR, info['casesfile']))
        sys.exit(status)


if __name__ == "__main__":
    main()