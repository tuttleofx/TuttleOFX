#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then

    sudo add-apt-repository -y ppa:boost-latest/ppa
    sudo add-apt-repository -y ppa:irie/openimageio
    sudo add-apt-repository -y ppa:kubuntu-ppa/backports
    sudo apt-get update -qq

    sudo apt-get install -qq cmake swig swig2.0 libboost1.55-all-dev python-dev python-numpy libfreetype6-dev libXt-dev libbz2-dev liblcms-dev libopenctl0.8 libltdl-dev libpng-dev libcaca-dev libjpeg-dev libglew-dev libtiff-dev libilmbase-dev libopenexr-dev libMagickCore-dev libraw-dev libopenjpeg-dev libglui-dev libglew-dev graphviz graphviz-dev python-nose python-imaging libtinyxml-dev libyaml-cpp-dev libopenimageio-dev libturbojpeg libxmu-dev yasm libmp3lame-dev libx264-dev libxvidcore-dev liblzma-dev

    cd $TRAVIS_BUILD_DIR
    wget https://www.ffmpeg.org/releases/ffmpeg-2.2.9.tar.bz2
    bunzip2 ffmpeg-2.2.9.tar.bz2
    tar -xf ffmpeg-2.2.9.tar
    cd ffmpeg-2.2.9
    ./configure --enable-shared --disable-static && make -j${CI_NODE_TOTAL} && sudo make install

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/ampas/aces_container/archive/v1.0.tar.gz -O /tmp/aces_container-1.0.tar.gz
    tar -xzf /tmp/aces_container-1.0.tar.gz
    mkdir aces_container-1.0/build
    cd aces_container-1.0/build
    cmake .. && make -j${CI_NODE_TOTAL} && sudo make install

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/ampas/CTL/archive/ctl-1.5.2.tar.gz -O /tmp/ctl-1.5.2.tar.gz
    tar -xzf /tmp/ctl-1.5.2.tar.gz
    mkdir CTL-ctl-1.5.2/build
    cd CTL-ctl-1.5.2/build
    cmake .. && make -j${CI_NODE_TOTAL} && sudo make install

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/wdas/SeExpr/archive/rel-1.0.1.tar.gz -O /tmp/SeExpr-1.0.1.tar.gz
    tar -xzf /tmp/SeExpr-1.0.1.tar.gz
    mkdir SeExpr-rel-1.0.1/build
    cd SeExpr-rel-1.0.1/build
    cmake .. && make -j${CI_NODE_TOTAL} && sudo make install

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/imageworks/OpenColorIO/archive/v1.0.9.tar.gz -O /tmp/ocio-1.0.9.tar.gz
    tar -xzf /tmp/ocio-1.0.9.tar.gz
    mkdir OpenColorIO-1.0.9/build
    cd OpenColorIO-1.0.9/build
    cmake .. && make -j${CI_NODE_TOTAL} && sudo make install

elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then

    brew update
    brew tap homebrew/python
    brew tap homebrew/science

    echo "Install TuttleOFX dependencies"
    brew install python
    echo " - pip install nose for test"
    pip install nose
    echo " - install brew packages"
    brew install swig ilmbase openexr jasper little-cms2 glew freetype fontconfig ffmpeg imagemagick libcaca aces_container ctl jpeg-turbo libraw seexpr openjpeg opencolorio openimageio numpy

    echo "End dependencies installation."
fi
