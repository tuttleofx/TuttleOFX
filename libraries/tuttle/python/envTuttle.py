#!/usr/bin/python
import os
import sys
from ctypes import cdll

# Get TUTTLE_ROOT
TUTTLE_ROOT = os.path.dirname(os.path.dirname((os.path.realpath((__file__)))))

# Set env
os.environ['LIBAV_DATADIR'] = os.path.join(TUTTLE_ROOT, 'libav_presets')
os.environ['OFX_PLUGIN_PATH'] = os.path.join(TUTTLE_ROOT, 'plugin')
os.environ['TUTTLE_HOME'] = os.path.join( os.environ['HOME'], '.py_tuttleofx')

sys.path.append(os.path.join(TUTTLE_ROOT, 'python'))

# Load libraries
TUTTLE_LIB_DIR = os.path.join(TUTTLE_ROOT, 'lib')

for lib in os.listdir(TUTTLE_LIB_DIR):
    if ".so" in lib or ".dll" in lib:
        try:
            lib_path = os.path.join( TUTTLE_LIB_DIR, lib )
            cdll.LoadLibrary( lib_path )
        except Exception as e:
            print("ERROR: unable to load library:", lib_path)
            print(e)
