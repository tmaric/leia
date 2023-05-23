"""
Module connects postprocessing properties/ columns of CSV files in this project with strategies, \
    functions to get represantive values for convergence calculations. 
"""

from ._getvalue import *


DEFAULT =                       'max'
DICT = {
    'E_GEOM_ALPHA'              : 'endTime',
    'E_GEOM_ALPHA_REL'          : 'endTime',
    
    'E_VOL_ALPHA_REL_TV'        : 'endTime',
    'E_VOL_ALPHA_REL_TVtime'    : 'endTime',

    "E_TV"                      : 'absendTime',
    "E_TV_REL"                  : 'absendTime',
    
}

def get_strategy(label):
    return DICT.get(label, DEFAULT)

dict_get_values = {
    'max': get_maxvalue,
    'endTime': get_endvalue,
    'absendTime': get_absendvalue,
}