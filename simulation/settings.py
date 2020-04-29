# Module to configure simulation parameters
# The only required argument is EXECUTABLE
# Default values are defined inside
# core.global_settings.py. DO NOT CHANGE THAT
# FILE. All changes made here overwrite global_settings.py
# You may create aditional logic, functions, helpers, etc
# as nedded. It is recomended that you keep this file
# clean and well documented.

import os
from os.path import join, dirname, abspath

def from_env(variable, quiet=False):
    value = os.getenv(variable)
    if value is None and not quiet:
        raise KeyError(f'Variable \'{variable}\' not found')
    return value

# Path to parent directory.
BASE_DIR = dirname(dirname(abspath(__file__)))


# The path to folder containing all datasets
DATASET_DIR = from_env('DATASET_DIR')


# TODO: Get path from os.getenv()
RESULTS_DIR = from_env('RESULTS_DIR')

# List of datasets to run on this simulation.
DATASETS_TO_RUN = ['Bikes', ]

# Resolved paths for each dataset.
# The executable expects a trailing slash for folders.
DATASETS = [join(DATASET_DIR, dataset, '')
            for dataset in DATASETS_TO_RUN]
RESULTS = [join(RESULTS_DIR, dataset + '_%(SIMULATION_ID)s', '')
            for dataset in DATASETS_TO_RUN]

# List of transformation protocols to be used
TRANSFORMS = ['DCT']

# Quatizations values
QUANTIZATIONS = [0.1, 0.25, 0.5, 1, 2, 5, 7, 10, 15, 20, 50, 100]

# Argument that are used to create simulation objects.
# All list-like objects will be combined. For instance,
# two arguments X = [A, B] and Y = [1, 2, 3] will produce
# six combinations (A1, B1, A2, B2, A3, B3). To avoid that,
# provide the grouping of arguments that should behave
# as if one.
# They are resolved as follows:
#
#   1. Numeric values are inlined.
#   2. List and Tuples are iterated over, creating a
#       separate simulation object for each value.
#   3. Strings are evaluated as previous keys of ARGS.
#       If it fails to find a key, the value is inlined.
#
# Example
# ARGS = {
#     'x': 10,                        # Evaluates to "x 10"
#     'y': [10, 20, 30],              # Create one simulation for
#                                     # each value of y
#     'logfile': '~/mylog.txt'        # As there is no previous entry with
#                                     # the key `~/mylog.txt`, the string
#                                     # '~/mylog.txt' is inlined.
#                                     # It evaluates to "logfile ~/mylog.txt"
#     'other_x': 'x',                 # Because `x` is a previous key,
#                                     # 'other_x' will have the same value
#                                     # of x.
#                                     # It evaluates to "other_x 10"
# }
ARGS = {
    '-transform': TRANSFORMS,
    '-qp': QUANTIZATIONS,
    '-qx': '-qp',       # Reference to -qp
    '-qy': '-qp',       # Reference to -qp
    '-qu': '-qp',       # Reference to -qp
    '-qv': '-qp',       # Reference to -qp
    '-lfx': 625,
    '-lfy': 434,
    '-lfu': 13,
    '-lfv': 13,
    '-blx': 15,
    '-bly': 15,
    '-blu': 13,
    '-blv': 13,
    '-input': DATASETS,
    '-output': RESULTS, # This parameter uses SIMULATION_ID
}

# List of groupings. Use this setting to group arguments
# that should be combined together. Be aware that the number
# of elements for each member of a grouping should be the same.
# If their sizes differ, the smaller size will prevail,
# leading to unknown behaviour.
GROUP_TOGETHER_ARGS = (
    ('-input', '-output'),
)


BUILD_DIR = from_env('BUILD_DIR')
BUILD_DIR = join(BASE_DIR, BUILD_DIR)

# Binary
EXECUTABLE =  join(BUILD_DIR, 'lfcodec')

# Variable at module level.
SIMULATION_ID = '%(-blx)s_%(-bly)s_%(-blu)s_%(-blv)s_' \
                '%(-qx)s_%(-qy)s_%(-qu)s_%(-qv)s_' \
                '%(-transform)s'


# Redirects stdout to a file inside the result of each
# simulation. Notice that it uses the value of '-output'
# to locate the directory where results are saved. It
# also uses the module level variable SIMULATION_ID
# Because -output has a trailing slash, it suffices just
# concatenate both strings
STDOUT = '%(-output)s' + '%(SIMULATION_ID)s.txt'

# List here the variables defined at module level to parse
# before using in the simulation. All variables defined in
# ARGS are also available for referencing and parsing.
# You may reference this variables for customizing
# STDIN, STDOUT and STDERR. By default, they are always parsed.
VARIABLES_TO_PARSE = [
    'SIMULATION_ID',
    '-output',
]

# Number of parallel instaces to run the simulation
PARALLEL_INSTANCES = 6
