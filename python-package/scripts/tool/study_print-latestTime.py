#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import os.path
from subprocess import run
import os


app_description = \
"""
Script lists latestTimes of all cases inside the study directory.
"""

def parse_arguments():
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    parser.add_argument("STUDYDIR",
                        help="OpenFOAM case directory.",
                        )
    parser.add_argument("-L",
                        help="Max level depth",
                        dest='level',
                        required=False,
                        type=int
                        )
    args = parser.parse_args()
    assert os.path.isdir(args.STUDYDIR), f"{args.STUDYDIR} is no directory!"
    return args


def main():
    args = parse_arguments()

    os.chdir(args.STUDYDIR)

    if not args.level:
        cmd = f"""find -type d -name "*[0-9][0-9][0-9][0-9][0-9]*" -prune | sort | xargs -I[] bash -c 'echo -n "[] : " && foamLatestTime.sh []'"""
    else:
        cmd = f"""find -maxdepth {args.level} -type d -name "*[0-9][0-9][0-9][0-9][0-9]*" -prune | sort | xargs -I[] bash -c 'echo -n "[] : " && foamLatestTime.sh []'"""
    run(cmd, shell=True)


if __name__ == "__main__":
    main()