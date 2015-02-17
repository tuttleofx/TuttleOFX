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

### Configure ###
export CONFIGURE_CMD="cmake .. -DCMAKE_INSTALL_PREFIX=/tmp/tuttle-ci -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH -DCMAKE_CC_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_BUILD_TYPE=$BUILD_MODE"
if [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    export CONFIGURE_CMD="$CONFIGURE_CMD incdir_python=/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7 incdir_python_numpy=/usr/local/Cellar/numpy/1.8.2/lib/python2.7/site-packages/numpy/core/include"
fi

# Ask cmake to search in all homebrew packages
CMAKE_PREFIX_PATH=$(echo /usr/local/Cellar/*/* | sed 's/ /;/g')

mkdir -p build_travis
cd build_travis
$CONFIGURE_CMD

### Build ###
export BUILD_CMD="make $J VERBOSE=$VERBOSE -k"
export INSTALL_CMD="make install"

if [[ ${BUILD_TARGET} == "host" ]]; then
    # Don't build sam-info because of its dependencies.
    $BUILD_CMD tuttleHost sam-do sam-ls sam-cp sam-mv sam-rm sam-plugins sam-check sam-diff
    # Build the binding alone, as it needs a lot of memory.
    $BUILD_CMD _tuttle
elif [[ ${BUILD_TARGET} == "plugin" ]]; then
    $BUILD_CMD Invert
    # Build some plugins alone, as they need a lot of memory.
    $BUILD_CMD Text Sobel Blur
    $BUILD_CMD ofxplugins
fi

### Install ###
$INSTALL_CMD
