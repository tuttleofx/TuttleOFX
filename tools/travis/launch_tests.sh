#!/usr/bin/env bash

# boost unit tests
git clone https://github.com/tuttleofx/TuttleOFX-data.git
ln -s TuttleOFX-data ../../testBin/TuttleOFX-data
export OFX_PLUGIN_PATH=${TUTTLE_INSTALL}/OFX
export LD_LIBRARY_PATH=${TUTTLE_INSTALL}/lib
make run_tests

# nosetests
export PYTHONPATH=${TUTTLE_INSTALL}/lib/python2.7.3/site-packages
cd libraries/tuttle/pyTest
ln -s ../../../TuttleOFX-data TuttleOFX-data
nosetests
