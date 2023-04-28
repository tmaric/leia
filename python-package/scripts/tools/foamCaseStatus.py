#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import os.path
from subprocess import run
from glob import glob
import sys


app_description = \
"""
Inspects log files ['log', 'out.[0-9]*', 'err.[0-9]*'] of a OpenFOAM case and prints if a case COMPLETE or its state.
"""

def get_logfiles(case):
    """
    Finds stdout and stderr log files in case and returns last (newest) ones.  
    """
    
    possible_out = ['log', 'out.[0-9]*']
    possible_err = ['log', 'err.[0-9]*']
    
    for po in possible_out:
        outs = sorted(glob(os.path.join(case,po)))
        if outs:
            break
    else:
        outs = [None]
    
    for pe in possible_err:
        errs = sorted(glob(os.path.join(case,pe)))
        if errs:
            break
    else:
        errs = [None]
    
    return outs[-1], errs[-1]

def parse_arguments():
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    parser.add_argument("CASE",
                        help="OpenFOAM case directory.",
                        )
    args = parser.parse_args()
    assert os.path.isdir(args.CASE), f"{args.CASE} is no directory!"
    args.CASEbasename = os.path.basename(os.path.abspath(args.CASE))
    args.out, args.err = get_logfiles(args.CASE)
    return args
    
def lastlines_of_file(file, nlines=15):
    """
    Get the last lines in one string of a file. 
    """
    if not os.path.isfile(file):
        return None
    completed_process = run(f"tail -n {nlines} {file}", shell=True, check=True, capture_output=True, encoding='utf-8')
    return completed_process.stdout

def latestTime_endTime(case):
    """
    Get the last timedir time and the endTime from the controlDict.  
    """
    latestTime = run(f"foamLatestTime.sh {case}", shell=True, check=True, capture_output=True, encoding='utf-8').stdout
    latestTime = float(latestTime)
    # endTime = run(f"foamDictionary -entry endTime -value {os.path.join(case, 'system/controlDict')}",  # Issue: Rounds to the 5th digit
    #             shell=True, 
    #             check=True, 
    #             capture_output=True, 
    #             encoding='utf-8'
    #             ).stdout
    endTime = run(f"sed -n '/^endTime\>/p' {os.path.join(case, 'system/controlDict')} | grep -o '[0-9.]*", 
                shell=True, 
                check=True, 
                capture_output=True, 
                encoding='utf-8'
                ).stdout
    endTime = float(endTime)
    return (latestTime, endTime)



def main():
    args = parse_arguments()
    
    latestTime, endTime = latestTime_endTime(args.CASE)
    if latestTime == endTime:
        print("COMPLETE")
        sys.exit(0)
        
    if args.out is None or args.err is None:
        print("No log files")
        sys.exit(1)
        
    out = lastlines_of_file(args.out)
    err = lastlines_of_file(args.err)
        
    if "Floating point exception" in err:
        print("Floating point exception")
        sys.exit(2)
    elif "Segmentation fault" in err:
        print("Segmentation fault")
        sys.exit(3)
    elif " End " in out:
        print("END, but latestTime != endTime")
        sys.exit(4)
    else:
        print("Probably still running")
        sys.exit(5)


if __name__ == "__main__":
    main()

