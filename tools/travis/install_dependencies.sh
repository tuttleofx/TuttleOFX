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

    wget --quiet https://www.dropbox.com/s/7qwo3jzmdsugtis/bottles.tar &
    WGET_PID=$!

    # sudo install_xquartz &
    # XQ_INSTALL_PID=$!

    brew update
    brew tap homebrew/python
    brew tap homebrew/science

    wait $WGET_PID
    tar -xvf bottles.tar

    echo "Install official bottles"
    brew install bottles/ilmbase-2.1.0.mavericks.bottle.tar.gz
    brew install bottles/openexr-2.1.0.mavericks.bottle.tar.gz
    # cmake is installed by default on travis
    # brew install bottles/cmake-2.8.12.2.mavericks.bottle.2.tar.gz
    brew install bottles/jasper-1.900.1.mavericks.bottle.tar.gz
    brew install bottles/little-cms2-2.6.mavericks.bottle.tar.gz
    # brew install bottles/doxygen-1.8.7.mavericks.bottle.tar.gz
    brew install gmp  # bottles/gmp-6.0.0a.mavericks.bottle.tar.gz
    brew install mpfr  # bottles/mpfr-3.1.2-p8.mavericks.bottle.tar.gz
    brew install libmpc  # bottles/libmpc-1.0.2.mavericks.bottle.tar.gz
    brew install isl  # bottles/isl-0.12.2.mavericks.bottle.tar.gz
    brew install cloog  # bottles/cloog-0.18.1.mavericks.bottle.1.tar.gz
    brew unlink gcc  # need to get gcc installed by homebrew
    brew install gcc  # bottles/gcc-4.8.3_1.mavericks.bottle.tar.gz
    brew install bottles/webp-0.4.0_1.mavericks.bottle.tar.gz
    brew install glew  # bottles/glew-1.10.0.mavericks.bottle.tar.gz
    brew install bottles/qt-4.8.6.mavericks.bottle.5.tar.gz
    brew install bottles/freetype-2.5.3_1.mavericks.bottle.1.tar.gz
    brew install bottles/pcre-8.35.mavericks.bottle.tar.gz
    brew install bottles/swig-3.0.2.mavericks.bottle.tar.gz
    brew install x264  # bottles/x264-r2412.mavericks.bottle.1.tar.gz
    brew install bottles/faac-1.28.mavericks.bottle.tar.gz || true
    brew install bottles/lame-3.99.5.mavericks.bottle.tar.gz || true
    brew install bottles/xvid-1.3.2.mavericks.bottle.tar.gz || true
    brew install ffmpeg  # bottles/ffmpeg-2.2.3.mavericks.bottle.tar.gz || true
    brew install imagemagick  # bottles/imagemagick-6.8.9-1.mavericks.bottle.tar.gz || true
    brew install bottles/libcaca-0.99b19.mavericks.bottle.tar.gz || true
    brew install scons  # bottles/scons-2.3.1.mavericks.bottle.3.tar.gz || true

    # selfbuilt bottles
    brew uninstall boost && brew install bottles/boost-1.55.0_2.mavericks.bottle.4.tar.gz
    brew install bottles/aces_container-1.0.mavericks.bottle.tar.gz
    brew install bottles/ctl-1.5.2.mavericks.bottle.tar.gz
    brew install bottles/jpeg-turbo-1.3.1.mavericks.bottle.tar.gz
    brew install bottles/libraw-0.15.4.mavericks.bottle.tar.gz
    brew install bottles/seexpr-1.0.1.mavericks.bottle.tar.gz
    brew install tbb  # bottles/tbb-4.2.4.mavericks.bottle.tar.gz
    brew install bottles/suite-sparse-4.2.1.mavericks.bottle.tar.gz
    brew install numpy  # bottles/numpy-1.8.1.mavericks.bottle.tar.gz || true
    brew install bottles/openjpeg-1.5.1_1.mavericks.bottle.tar.gz
    # brew install bottles/cfitsio-3.360.mavericks.bottle.tar.gz || true
    brew install bottles/szip-2.1.mavericks.bottle.tar.gz
    brew install bottles/hdf5-1.8.13.mavericks.bottle.tar.gz
    brew install bottles/field3d-1.4.3.mavericks.bottle.tar.gz
    brew install bottles/opencolorio-1.0.9.mavericks.bottle.tar.gz
    brew install bottles/openimageio-1.4.8.mavericks.bottle.tar.gz

    cp tools/sconf/macos_homebrew.sconf host.sconf

    # wait $XQ_INSTALL_PID || true

    echo "End dependencies installation."

fi
