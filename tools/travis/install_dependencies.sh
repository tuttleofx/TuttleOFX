#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# enable testing locally or on forks without multi-os enabled
if [[ "${TRAVIS_OS_NAME:-false}" == false ]]; then
    if [[ $(uname -s) == "Darwin" ]]; then
        TRAVIS_OS_NAME="osx"
    elif [[ $(uname -s) == "Linux" ]]; then
        TRAVIS_OS_NAME="linux"
    fi
fi


if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    lsb_release -a

    sudo add-apt-repository -y ppa:boost-latest/ppa
    sudo add-apt-repository -y ppa:irie/openimageio
    sudo add-apt-repository -y ppa:kubuntu-ppa/backports
    sudo apt-get update -qq

    sudo apt-get install -qq swig swig2.0 libboost1.55-all-dev python-dev python-numpy libfreetype6-dev libXt-dev libbz2-dev liblcms-dev libopenctl0.8 libltdl-dev libpng-dev libcaca-dev libjpeg-dev libglew-dev libtiff-dev libilmbase-dev libopenexr-dev libMagickCore-dev libraw-dev libopenjpeg-dev libglui-dev libglew-dev graphviz graphviz-dev python-nose python-imaging libtinyxml-dev libyaml-cpp-dev libopenimageio-dev libturbojpeg libxmu-dev yasm libmp3lame-dev libx264-dev libxvidcore-dev liblzma-dev

    cd $TRAVIS_BUILD_DIR
    wget https://www.ffmpeg.org/releases/ffmpeg-2.2.9.tar.bz2
    bunzip2 ffmpeg-2.2.9.tar.bz2
    tar -xvf ffmpeg-2.2.9.tar > /dev/null 2>&1
    cd ffmpeg-2.2.9
    ./configure --enable-shared --disable-static > /dev/null 2>&1 && make $J > /dev/null 2>&1 && sudo make install > /dev/null 2>&1

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/ampas/aces_container/archive/v1.0.tar.gz -O /tmp/aces_container-1.0.tar.gz
    tar -xzvf /tmp/aces_container-1.0.tar.gz > /dev/null 2>&1
    mkdir aces_container-1.0/build
    cd aces_container-1.0/build
    cmake .. > /dev/null 2>&1 && make $J > /dev/null 2>&1 && sudo make install > /dev/null 2>&1

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/ampas/CTL/archive/ctl-1.5.2.tar.gz -O /tmp/ctl-1.5.2.tar.gz
    tar -xzvf /tmp/ctl-1.5.2.tar.gz > /dev/null 2>&1
    mkdir CTL-ctl-1.5.2/build
    cd CTL-ctl-1.5.2/build
    cmake .. > /dev/null 2>&1 && make $J > /dev/null 2>&1 && sudo make install > /dev/null 2>&1

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/wdas/SeExpr/archive/rel-1.0.1.tar.gz -O /tmp/SeExpr-1.0.1.tar.gz
    tar -xzvf /tmp/SeExpr-1.0.1.tar.gz > /dev/null 2>&1
    mkdir SeExpr-rel-1.0.1/build
    cd SeExpr-rel-1.0.1/build
    cmake .. > /dev/null 2>&1 && make $J > /dev/null 2>&1 && sudo make install > /dev/null 2>&1

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/imageworks/OpenColorIO/archive/v1.0.9.tar.gz -O /tmp/ocio-1.0.9.tar.gz
    tar -xzvf /tmp/ocio-1.0.9.tar.gz > /dev/null 2>&1
    mkdir OpenColorIO-1.0.9/build
    cd OpenColorIO-1.0.9/build
    cmake .. > /dev/null 2>&1 && make $J > /dev/null 2>&1 && sudo make install > /dev/null 2>&1

elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    sw_vers -productVersion

    # See Travis OSX setup:
    # http://docs.travis-ci.com/user/osx-ci-environment

    # XQuartz already installed on Travis
    # install_xquartz(){
    #     echo "XQuartz start install"
    #     XQUARTZ_VERSION=2.7.6
    #     
    #     echo "XQuartz download"
    #     wget --quiet http://xquartz.macosforge.org/downloads/SL/XQuartz-${XQUARTZ_VERSION}.dmg
    #     echo "XQuartz mount dmg"
    #     hdiutil mount XQuartz-${XQUARTZ_VERSION}.dmg
    #     echo "XQuartz installer"  # sudo
    #     installer -store -pkg /Volumes/XQuartz-${XQUARTZ_VERSION}/XQuartz.pkg -target /
    #     echo "XQuartz unmount"
    #     hdiutil unmount /Volumes/XQuartz-${XQUARTZ_VERSION}
    #     echo "XQuartz end"
    # }

    # sudo install_xquartz &
    # XQ_INSTALL_PID=$!

    brew update
    brew tap homebrew/python
    brew tap homebrew/science

    echo "Install TuttleOFX dependencies"
    echo " - pip install numpy"
    pip install numpy
    # brew install numpy  # Compilation errors with gfortran
    echo " - install brew packages"
    brew install swig ilmbase openexr jasper little-cms2 glew freetype fontconfig ffmpeg imagemagick libcaca aces_container ctl jpeg-turbo libraw seexpr openjpeg opencolorio openimageio

    # wait $XQ_INSTALL_PID || true

    echo "End dependencies installation."
fi

