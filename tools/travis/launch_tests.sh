#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# set env
export OFX_PLUGIN_PATH=${TUTTLEOFX_INSTALL}/OFX
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${DEPENDENCIES_INSTALL}/lib:${TUTTLEOFX_INSTALL}/lib
export PYTHONPATH=${TUTTLEOFX_INSTALL}/lib/python${PYTHON_VERSION}/site-packages:$PYTHONPATH

# clone data for tests
git clone https://github.com/tuttleofx/TuttleOFX-data.git

# nosetests
ln -s ${TRAVIS_BUILD_DIR}/TuttleOFX-data ${TRAVIS_BUILD_DIR}/libraries/tuttle/pyTest/TuttleOFX-data
if [[ ${PYTHON_VERSION} == "2.7" ]]; then
    nosetests ${TRAVIS_BUILD_DIR}/libraries/tuttle/pyTest -v
elif [[ ${PYTHON_VERSION} == "3.2" ]]; then
    nosetests3 ${TRAVIS_BUILD_DIR}/libraries/tuttle/pyTest -v
fi


# boost unit tests
ln -s ${TRAVIS_BUILD_DIR}/TuttleOFX-data ${TRAVIS_BUILD_DIR}/testBin/TuttleOFX-data
cd ${TUTTLEOFX_BUILD}
make run_tests

