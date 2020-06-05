
import os

# Set required environment variables required for settings.py
os.environ.setdefault('DATASET_DIR', '/home/bosin/lfcodec/datasets')
os.environ.setdefault('RESULTS_DIR', '/home/bosin/lfcodec/results/SIM_QUANT_SEG_LUMA')
os.environ.setdefault('BINARY', 'LF_Codec_QUANT_SEG_LUMA')

from settings import *
