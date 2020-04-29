import os

os.environ.setdefault('DATASET_DIR', '/mnt/c/Users/Cristian/Documents/UFPel/lfcodec/Datasets')
os.environ.setdefault('RESULTS_DIR', '/mnt/c/Users/Cristian/Documents/UFPel/lfcodec/Results')
os.environ.setdefault('BUILD_DIR', 'build')

from settings import *

ARGS.update({
    '-transform': ['DST', 'DCT'],
    '-qp': [15],
})
