""" Sam launcher
    Set environment variables and launch specified sam TOOL

    Install with sam.sh (or .bat) in TuttleOFX root directory

"""
import sys
import os
import subprocess

PLATFORM = sys.platform

# Get TUTTLE_ROOT env var
try:
    TUTTLE_ROOT = os.environ["TUTTLE_ROOT"]
except KeyError :
    sys.exit("ERROR : You must define TUTTLE_ROOT environment variable !\n"
             "TUTTLE_ROOT should contain the path to TuttleOFX installation.")

# Get TUTTLE_HOME env var
try:
    TUTTLE_HOME = os.environ["TUTTLE_HOME"]
except KeyError :
    sys.exit("ERROR : You must define TUTTLE_HOME environment variable !\n"
             "TUTTLE_HOME should contain the path to a proper user directory "
             "(for cache and log).")

# Set env var
if PLATFORM == 'win32':
    SEPARATOR = ';'
    BIN_VAR = 'Path'
    LIB_VAR = 'Path'
elif PLATFORM == 'darwin':
    SEPARATOR = ':'
    BIN_VAR = 'PATH'
    LIB_VAR = ' DYLD_LIBRARY_PATH'
else:
    SEPARATOR = ':'
    BIN_VAR = 'PATH'
    LIB_VAR = 'LD_LIBRARY_PATH'

# OFX plugin path
os.environ["OFX_PLUGIN_PATH"] = os.path.join(TUTTLE_ROOT, "plugin")

# AudioVideo presets
if "LIBAV_DATADIR" in os.environ:
    LIBAV_PATH = SEPARATOR + os.environ["LIBAV_DATADIR"]
else:
    LIBAV_PATH = ""
os.environ["LIBAV_DATADIR"] = (os.path.join(TUTTLE_ROOT, "libav_presets")
                               + LIBAV_PATH)

# Bin and lib paths
os.environ[BIN_VAR] = (os.path.join(TUTTLE_ROOT, "bin") + SEPARATOR
                      + os.environ[BIN_VAR])
os.environ[LIB_VAR] = (os.path.join(TUTTLE_ROOT, "lib") + SEPARATOR
                      + os.environ[LIB_VAR])
os.environ[LIB_VAR] = (os.path.join(TUTTLE_ROOT, "runtime_libs") + SEPARATOR
                      + os.environ[LIB_VAR])

# Create cache directory
CACHE_DIR = os.path.join(TUTTLE_HOME, "TuttleCache")
if not os.path.exists(CACHE_DIR):
    os.makedirs(CACHE_DIR)

ARGC = len(sys.argv)

if ARGC == 1:
# launch sam only
    os.system(os.path.join(TUTTLE_ROOT, "bin", "sam"))
    sys.exit()

# Get TOOL
TOOL = sys.argv[1]
if TOOL.startswith('-'):
    os.system(os.path.join(TUTTLE_ROOT, "bin", "sam") + " " + TOOL)
    sys.exit()

# Replace \ by /
PARAMS = [param.replace('\\', '/') for param in sys.argv[2:]]

# Launch sam command
subprocess.call(["sam-" + TOOL] + PARAMS)
