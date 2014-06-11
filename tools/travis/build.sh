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

(wget --quiet https://www.dropbox.com/s/0wkebzn5zyshlh8/testfiles.tar && tar -xf testfiles.tar) &

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    scons $J CC=$CC CXX=$CXX compiler=$CC
    scons $J CC=$CC CXX=$CXX compiler=$CC test
elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    scons $J CC=$CC CXX=$CXX compiler=$CC incdir_python=/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7 incdir_python_numpy=/usr/local/Cellar/numpy/1.8.1/lib/python2.7/site-packages/numpy/core/include
    scons $J CC=$CC CXX=$CXX compiler=$CC incdir_python=/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7 incdir_python_numpy=/usr/local/Cellar/numpy/1.8.1/lib/python2.7/site-packages/numpy/core/include test
fi
