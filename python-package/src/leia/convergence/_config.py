"""
Module connects postprocessing properties/ columns of CSV files in this project with strategies, \
    functions to get represantive values for convergence calculations. 
"""

from ._getvalue import *


DEFAULT =                       'endTime'
DICT = {
    'E_MEAN_GRAD_PSI'           : 'max',
    'E_MAX_GRAD_PSI'            : 'max',
    'E_NARROW_MEAN_GRAD_PSI'    : 'max',
    'E_NARROW_MAX_GRAD_PSI'     : 'max',

    'E_VOL_ALPHA'               : 'max',
    'E_VOL_ALPHA_REL'           : 'max',
    
}

def get_strategy(label):
    return DICT.get(label, DEFAULT)

dict_get_values = {
    'max': get_maxvalue,
    'endTime': get_endvalue,
}