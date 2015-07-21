#!/usr/bin/env bash

# set env
export OFX_PLUGIN_PATH=${TUTTLE_INSTALL}/OFX
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${DEPENDENCIES_INSTALL}/lib:${TUTTLE_INSTALL}/lib
export PYTHONPATH=${TUTTLE_INSTALL}/lib/python2.7/site-packages:$PYTHONPATH

# clone data for tests
git clone https://github.com/tuttleofx/TuttleOFX-data.git

# nosetests
ln -s ${TRAVIS_BUILD_DIR}/TuttleOFX-data ${TRAVIS_BUILD_DIR}/libraries/tuttle/pyTest/TuttleOFX-data
nosetests ${TRAVIS_BUILD_DIR}/libraries/tuttle/pyTest -v

# boost unit tests
ln -s ${TRAVIS_BUILD_DIR}/TuttleOFX-data ${TRAVIS_BUILD_DIR}/testBin/TuttleOFX-data
cd ${TUTTLE_BUILD}
make run_tests
