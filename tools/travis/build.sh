#!/usr/bin/env bash

set -e

# enable testing locally or on forks without multi-os enabled
if [[ "${TRAVIS_OS_NAME:-false}" == false ]]; then
    if [[ $(uname -s) == "Darwin" ]]; then
        TRAVIS_OS_NAME="osx"
        VERBOSE=0
    elif [[ $(uname -s) == "Linux" ]]; then
        TRAVIS_OS_NAME="linux"
        VERBOSE=1
    fi
fi

if [[ ${COVERITY_BUILD_DISABLED} == 1 ]];
then
    echo "Coverity is not executed on this build variant."
    exit 0;
fi

# (wget --quiet https://www.dropbox.com/s/0wkebzn5zyshlh8/testfiles.tar && tar -xf testfiles.tar) &

mkdir -p build_travis
cd build_travis

# Ask cmake to search in all homebrew packages
CMAKE_PREFIX_PATH=$(echo /usr/local/Cellar/*/* | sed 's/ /;/g')

cmake -DCMAKE_INSTALL_PREFIX=/tmp/tuttle-ci -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH ..
make $J VERBOSE=$VERBOSE -k
make install

