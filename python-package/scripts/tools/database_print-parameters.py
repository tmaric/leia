#!/usr/bin/env python
# coding: utf-8

from argparse import ArgumentParser, RawTextHelpFormatter
import pandas as pd
import numpy as np
import json


class NpEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.integer):
            return int(obj)
        if isinstance(obj, np.floating):
            return float(obj)
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        return super(NpEncoder, self).default(obj)

app_description = \
"""
Print out study parameters inside the database.
"""
def main():

    #---- Command line arguments ----------------------------------------------
    parser = ArgumentParser(description=app_description, formatter_class=RawTextHelpFormatter)

    
    parser.add_argument("database",
                        help="Database-CSV file with 2-level columns.")
    
    args = parser.parse_args()
    #---------------------------------------------------------------------------


    study_df = pd.read_csv(args.database, header=[0,1])
    
    study_gb = study_df.groupby(by=('database','CASE'))
    studyparams = study_df.columns[study_df.columns.get_loc_level('studyparameters')[0]]
    
    dict_ = {}
    for case, case_df in study_gb:
        dict_[case] = {}
        for param in studyparams:
            unique_param_vals = case_df[param].unique()
            assert len(unique_param_vals) == 1, f'Studyparameter {param} is not unique in case {case}.'
            dict_[case][param[1]] = unique_param_vals[0]



    print(json.dumps(dict_, cls=NpEncoder, ensure_ascii=False, indent=4))


if __name__ == "__main__":
    main()
