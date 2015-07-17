#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then

    if [ ! -d "${DEPENDENCIES_INSTALL}/lib/" ]; then

        cd $TRAVIS_BUILD_DIR
        wget https://www.ffmpeg.org/releases/ffmpeg-2.2.9.tar.bz2
        bunzip2 ffmpeg-2.2.9.tar.bz2
        tar -xf ffmpeg-2.2.9.tar
        cd ffmpeg-2.2.9
        ./configure --prefix=${DEPENDENCIES_INSTALL} --enable-shared --disable-static --disable-yasm && make -j${CI_NODE_TOTAL} && make install

        cd $TRAVIS_BUILD_DIR
        wget https://github.com/ampas/aces_container/archive/v1.0.tar.gz -O /tmp/aces_container-1.0.tar.gz
        tar -xzf /tmp/aces_container-1.0.tar.gz
        mkdir aces_container-1.0/build
        cd aces_container-1.0/build
        cmake .. -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make -j${CI_NODE_TOTAL} && make install

        cd $TRAVIS_BUILD_DIR
        wget https://github.com/ampas/CTL/archive/ctl-1.5.2.tar.gz -O /tmp/ctl-1.5.2.tar.gz
        tar -xzf /tmp/ctl-1.5.2.tar.gz
        mkdir CTL-ctl-1.5.2/build
        cd CTL-ctl-1.5.2/build
        cmake .. -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make -j${CI_NODE_TOTAL} && make install

        cd $TRAVIS_BUILD_DIR
        wget https://github.com/wdas/SeExpr/archive/rel-1.0.1.tar.gz -O /tmp/SeExpr-1.0.1.tar.gz
        tar -xzf /tmp/SeExpr-1.0.1.tar.gz
        mkdir SeExpr-rel-1.0.1/build
        cd SeExpr-rel-1.0.1/build
        cmake .. -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make -j${CI_NODE_TOTAL} && make install

        cd $TRAVIS_BUILD_DIR
        wget https://github.com/imageworks/OpenColorIO/archive/v1.0.9.tar.gz -O /tmp/ocio-1.0.9.tar.gz
        tar -xzf /tmp/ocio-1.0.9.tar.gz
        mkdir OpenColorIO-1.0.9/build
        cd OpenColorIO-1.0.9/build
        cmake .. -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make -j${CI_NODE_TOTAL} && make install

    else
        echo 'Using cached directory.';
    fi


elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then

    brew update
    brew tap homebrew/python
    brew tap homebrew/science

    echo "Install TuttleOFX dependencies"
    brew install python
    echo "Pip install nose for test"
    pip install nose
    echo "Install brew packages"
    brew install swig ilmbase openexr jasper little-cms2 glew freetype fontconfig ffmpeg imagemagick libcaca aces_container ctl jpeg-turbo libraw seexpr openjpeg opencolorio openimageio numpy

fi
