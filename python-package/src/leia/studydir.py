"""
Module which provides functionalities for the new studydir disk datastructure.
A studydir contains at least a .info file which has the same prefix as the basename of the studydir.
It also contains a template case and a .parameter file.

The module name studydir would be probably better, but studydir is already a widely used variable.
"""


import os.path
import warnings

CASE_CSVs = [
    'leiaLevelSetFoam.csv', 
    'gradPsiError.csv', 
    'TVerror.csv', 
    'postProcessing/velocity_data.csv'
    ]


def cases_ls(casesfile):
    with open(casesfile, 'r', encoding='utf-8') as file:
        cases_ls = file.readlines()
    return [ case.removesuffix('\n') for case in cases_ls ]

def filter_existing_cases(cases):
    cases_existing = list(filter(os.path.isdir, cases))
    for case in cases:
        if case not in cases_existing:
            warnings.warn(f"Skipping {case}. Not existing.")
    return cases_existing