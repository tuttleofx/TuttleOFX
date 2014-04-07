#!/bin/bash
# Launcher for Linux
# Install with sam.py in TuttleOFX root directory
export TUTTLE_ROOT=`dirname $0`

export TUTTLE_HOME=$HOME/.tuttleOFX
mkdir $TUTTLE_HOME -p
export SAM_PATH=$TUTTLE_ROOT/bin
export SAM_OPTIONS=--color
python $TUTTLE_ROOT/sam.py "$@"
