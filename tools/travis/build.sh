#!/usr/bin/env bash

set -e

# enable testing locally or on forks without multi-os enabled
if [[ "${TRAVIS_OS_NAME:-false}" == false ]]; then
    if [[ $(uname -s) == "Darwin" ]]; then
        TRAVIS_OS_NAME="osx"
    elif [[ $(uname -s) == "Linux" ]]; then
        TRAVIS_OS_NAME="linux"
    fi
fi

if [[ ${COVERITY_BUILD_DISABLED} == 1 ]];
then
    echo "Coverity is not executed on this build variant."
    exit 0;
fi

# (wget --quiet https://www.dropbox.com/s/0wkebzn5zyshlh8/testfiles.tar && tar -xf testfiles.tar) &

mkdir build_travis
cd build_travis
cmake -DCMAKE_INSTALL_PREFIX=/tmp/tuttle-ci .. 2> /dev/null
make $J VERBOSE=1 
make install
