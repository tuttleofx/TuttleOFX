#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# set env
export OFX_PLUGIN_PATH=${TUTTLEOFX_INSTALL}/OFX
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${DEPENDENCIES_INSTALL}/lib:${TUTTLEOFX_INSTALL}/lib
export PYTHONPATH=${TUTTLEOFX_INSTALL}/lib/python${PYTHON_VERSION}/site-packages:$PYTHONPATH
export PATH=$PATH:${TUTTLEOFX_INSTALL}/bin

# clone data for tests
git clone https://github.com/tuttleofx/TuttleOFX-data.git

# nosetests
ln -s ${TRAVIS_BUILD_DIR}/TuttleOFX-data ${TRAVIS_BUILD_DIR}/libraries/tuttle/pyTest/TuttleOFX-data
export NOSETESTS_BIN=nosetests
if [[ ${PYTHON_VERSION} == 3.* ]]; then
    export NOSETESTS_BIN=nosetests3
fi
${NOSETESTS_BIN} ${TRAVIS_BUILD_DIR}/libraries/tuttle/pyTest -v

# boost unit tests
ln -s ${TRAVIS_BUILD_DIR}/TuttleOFX-data ${TRAVIS_BUILD_DIR}/testBin/TuttleOFX-data
cd ${TUTTLEOFX_BUILD}
make run_tests

# sam tests
if [[ ${PYTHON_VERSION} == 2.* ]]; then
    ${TRAVIS_BUILD_DIR}/applications/test/samCommands.sh
fi
