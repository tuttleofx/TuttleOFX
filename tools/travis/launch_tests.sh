#!/usr/bin/env bash

# boost unit tests
git clone https://github.com/tuttleofx/TuttleOFX-data.git ../../testBin/TuttleOFX-data
export OFX_PLUGIN_PATH=/tmp/tuttle-ci/OFX
make run_tests

# nosetests
export PYTHONPATH=/tmp/tuttle-ci/lib/python2.7.3/site-packages
cd libraries/tuttle/pyTest
git clone https://github.com/tuttleofx/TuttleOFX-data.git
nosetests
