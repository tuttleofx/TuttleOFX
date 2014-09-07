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

# (wget --quiet https://www.dropbox.com/s/0wkebzn5zyshlh8/testfiles.tar && tar -xf testfiles.tar) &


echo "TRAVIS_OS_NAME: $TRAVIS_OS_NAME"
echo "TRAVIS_BRANCH: $TRAVIS_BRANCH"
echo "BUILD_MODE: $BUILD_MODE"
echo "CC: $CC"
echo "CXX: $CXX"
$CC --version

if [[ "$TRAVIS_OS_NAME" == "osx" && "$CC" == "gcc" ]];
then
  echo "On OSX: gcc/g++ is a link to clang.";
  echo "https://github.com/travis-ci/travis-ci/issues/2423"
  echo "Use gcc-4.8 to really use gcc."
  ls -l /usr/bin
  export CC=gcc-4.8; export CXX=g++-4.8;
  echo "CC: $CC";
  echo "CXX: $CXX";
  $CC --version
fi

export BUILD_CMD="scons CC=$CC CXX=$CXX compiler=$CC mode=$BUILD_MODE ignore_configure_errors=1"

if [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    export BUILD_CMD="$BUILD_CMD incdir_python=/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7 incdir_python_numpy=/usr/local/Cellar/numpy/1.8.2/lib/python2.7/site-packages/numpy/core/include"
fi

if [[ ${BUILD_TARGET} == "host" ]]; then
    # Don't build sam-info because of its dependencies.
    $BUILD_CMD $J tuttleHost sam-do sam-ls sam-cp sam-mv sam-rm sam-plugins sam-check sam-diff
    # Build the binding alone, as it needs a lot of memory.
    $BUILD_CMD pyTuttle
elif [[ ${BUILD_TARGET} == "plugin" ]]; then
    $BUILD_CMD $J Invert
    # Build some plugins alone, as they need a lot of memory.
    $BUILD_CMD Text Sobel Blur
    $BUILD_CMD $J ofxplugins
fi

