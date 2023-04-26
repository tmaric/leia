#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import pandas as pd


app_description = \
"""
Script sorts CSV file by the 0-level columns ['CASE', 'TIME']. 
"""

def main():

    #---- Command line arguments ----------------------------------------------
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    parser.add_argument("CSV",
                        help="CSV file will be sorted.",
                        )

    parser.add_argument('-m', '--multiindex', 
                       action='store_true',
                       help="CSV columns is a 2-level multiindex object.",
                       required=False,
                       )

    group = parser.add_mutually_exclusive_group(required=True)

    group.add_argument('-i', '--inplace', 
                       action='store_true',
                       help="Save sorted CSV inplace."
                       )

    group.add_argument('-f', '--file',
                       help="Save sorted CSV to file",
                       dest='file'
                       )

    args = parser.parse_args()


    #---------------------------------------------------------------------------


    if not args.multiindex:
        header=0
        by=['CASE', 'TIME']
    else:
        header=[0,1]
        by=[('database','CASE'), ('case','TIME')]

    if args.inplace:
        savefile = args.CSV
    elif args.file:
        savefile = args.file
    else:
        raise RuntimeError('No option for saving.')

    df = pd.read_csv(args.CSV, header=header)
    df.sort_values(by=by, inplace=True)
    df.to_csv(savefile, index=False)

if __name__ == "__main__":
    main()