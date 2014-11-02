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

if [[ ${BUILD_DISABLED} == 1 ]];
then
    echo "Coverity is not executed on this build variant."
    exit 0;
fi

scons check_libs=0 $J CC=$CC CXX=$CXX compiler=$CC tuttleHost ofxplugins

