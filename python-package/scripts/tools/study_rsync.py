#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
from subprocess import run
import os.path


app_description = \
"""
rsync the a study directory without the content of concreate simulation cases.
"""

def main():

    #---- Command line arguments ----------------------------------------------
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    parser.add_argument("source")

    parser.add_argument("destination")

    parser.add_argument("-f", "--flatten",
                        action='store_true',
                        help='If source is a directory, in destination move all CSV and TXT files from subdirectories to the top level.',
                        required=False,                        
                        )

    args = parser.parse_args()

    #---------------------------------------------------------------------------

    excludes =[
        "*_[0-9][0-9][0-9][0-9][0-9]_*",
        "*tmp*"
    ]

    excludes_flags = " ".join(["--exclude=" + ex for ex in excludes])

    rsync_cmd = f"rsync -avu {excludes_flags} --prune-empty-dirs {args.source} {args.destination}"

    run(rsync_cmd, shell=True, check=True)

    destination_studydir = os.path.join(args.destination, os.path.basename(args.source))
    if args.flatten and os.path.isdir(destination_studydir):
        flatten_cmd = f"""
            cd {destination_studydir} &&
            dest=$(pwd) &&
            find -mindepth 2 -type f \( -name "*.csv" -o -name "*.txt" \) | sort | 
                xargs -I[] mv -v [] $dest/
            """
        run(flatten_cmd, shell=True, check=True)

if __name__ == "__main__":
    main()