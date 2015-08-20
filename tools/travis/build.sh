#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# Create directory of build
mkdir -p ${TUTTLE_BUILD}
cd ${TUTTLE_BUILD}

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    # Ask cmake to search in all dependencies we've installed manually
    export CMAKE_PREFIX_PATH=$DEPENDENCIES_INSTALL
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    # Ask cmake to search in all homebrew packages
    export CMAKE_PREFIX_PATH=$(echo /usr/local/Cellar/*/* | sed 's/ /;/g')
fi

# Build tuttle
cmake --version
cmake .. -DCMAKE_INSTALL_PREFIX=${TUTTLE_INSTALL} -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH -DTUTTLE_EXPERIMENTAL=True -DWITHOUT_NUMPY=True -DTUTTLE_DEPLOY_DEPENDENCIES=True -DTUTTLE_PYTHON_VERSION=${PYTHON_VERSION}
make -j${CI_NODE_TOTAL} -k
make install
cd ..
