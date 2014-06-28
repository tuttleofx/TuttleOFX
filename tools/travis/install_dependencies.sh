#!/usr/bin/env bash

set -e
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
    cp tools/sconf/ubuntu_travis.sconf host.sconf

    sudo add-apt-repository -y ppa:boost-latest/ppa
    sudo add-apt-repository -y ppa:irie/openimageio
    sudo add-apt-repository -y ppa:kubuntu-ppa/backports
    sudo apt-get update -qq

    sudo apt-get install -qq swig swig2.0 libboost1.55-all-dev python-dev python-numpy libfreetype6-dev libXt-dev libbz2-dev liblcms-dev libopenctl0.8 libltdl-dev libpng-dev libcaca-dev libjpeg-dev libglew-dev libtiff-dev libilmbase-dev libopenexr-dev libMagickCore-dev libraw-dev libopenjpeg-dev libglui-dev libglew-dev graphviz graphviz-dev python-nose python-imaging libtinyxml-dev libyaml-cpp-dev libopenimageio-dev libturbojpeg libxmu-dev yasm

    cd $TRAVIS_BUILD_DIR
    git clone git://github.com/MarcAntoine-Arnaud/libav.git
    cd libav
    ./configure --enable-shared && make $J && sudo make install

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/ampas/aces_container/archive/v1.0.tar.gz -O /tmp/aces_container-1.0.tar.gz
    tar -xzvf /tmp/aces_container-1.0.tar.gz
    mkdir aces_container-1.0/build
    cd aces_container-1.0/build
    cmake .. && make $J && sudo make install

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/ampas/CTL/archive/ctl-1.5.2.tar.gz -O /tmp/ctl-1.5.2.tar.gz
    tar -xzvf /tmp/ctl-1.5.2.tar.gz
    mkdir CTL-ctl-1.5.2/build
    cd CTL-ctl-1.5.2/build
    cmake .. && make $J && sudo make install

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/wdas/SeExpr/archive/rel-1.0.1.tar.gz -O /tmp/SeExpr-1.0.1.tar.gz
    tar -xzvf /tmp/SeExpr-1.0.1.tar.gz
    mkdir SeExpr-rel-1.0.1/build
    cd SeExpr-rel-1.0.1/build
    cmake .. && make $J && sudo make install

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/imageworks/OpenColorIO/archive/v1.0.9.tar.gz -O /tmp/ocio-1.0.9.tar.gz
    tar -xzvf /tmp/ocio-1.0.9.tar.gz
    mkdir OpenColorIO-1.0.9/build
    cd OpenColorIO-1.0.9/build
    cmake .. && make $J && sudo make install

elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then
    install_xquartz(){
        XQUARTZ_VERSION=2.7.6

        wget --quiet http://xquartz.macosforge.org/downloads/SL/XQuartz-${XQUARTZ_VERSION}.dmg
        hdiutil mount XQuartz-${XQUARTZ_VERSION}.dmg
        sudo installer -store -pkg /Volumes/XQuartz-${XQUARTZ_VERSION}/XQuartz.pkg -target /
        hdiutil unmount /Volumes/XQuartz-${XQUARTZ_VERSION}
    }

    wget --quiet https://www.dropbox.com/s/7qwo3jzmdsugtis/bottles.tar &
    WGETPID=$!

    install_xquartz &

    brew update
    brew tap homebrew/python
    brew tap homebrew/science

    wait $WGETPID
    tar -xvf bottles.tar

    # official bottles
    brew install bottles/ilmbase-2.1.0.mavericks.bottle.tar.gz
    brew install bottles/openexr-2.1.0.mavericks.bottle.tar.gz
    brew install bottles/cmake-2.8.12.2.mavericks.bottle.2.tar.gz
    brew install bottles/jasper-1.900.1.mavericks.bottle.tar.gz
    brew install bottles/little-cms2-2.6.mavericks.bottle.tar.gz
    brew install bottles/doxygen-1.8.7.mavericks.bottle.tar.gz
    brew install bottles/gmp-6.0.0a.mavericks.bottle.tar.gz
    brew install bottles/mpfr-3.1.2-p8.mavericks.bottle.tar.gz
    brew install bottles/libmpc-1.0.2.mavericks.bottle.tar.gz
    brew install bottles/isl-0.12.2.mavericks.bottle.tar.gz
    brew install bottles/cloog-0.18.1.mavericks.bottle.1.tar.gz
    brew install bottles/gcc-4.8.3_1.mavericks.bottle.tar.gz
    brew install bottles/webp-0.4.0_1.mavericks.bottle.tar.gz
    brew install bottles/glew-1.10.0.mavericks.bottle.tar.gz
    brew install bottles/qt-4.8.6.mavericks.bottle.5.tar.gz
    brew install bottles/freetype-2.5.3_1.mavericks.bottle.1.tar.gz
    brew install bottles/pcre-8.35.mavericks.bottle.tar.gz
    brew install bottles/swig-3.0.2.mavericks.bottle.tar.gz
    brew install bottles/x264-r2412.mavericks.bottle.1.tar.gz
    brew install bottles/faac-1.28.mavericks.bottle.tar.gz
    brew install bottles/lame-3.99.5.mavericks.bottle.tar.gz
    brew install bottles/xvid-1.3.2.mavericks.bottle.tar.gz
    brew install bottles/ffmpeg-2.2.3.mavericks.bottle.tar.gz
    brew install bottles/imagemagick-6.8.9-1.mavericks.bottle.tar.gz
    brew install bottles/libcaca-0.99b19.mavericks.bottle.tar.gz
    brew install bottles/scons-2.3.1.mavericks.bottle.3.tar.gz

    # selfbuilt bottles
    brew install bottles/boost-1.55.0_2.mavericks.bottle.4.tar.gz
    brew install bottles/aces_container-1.0.mavericks.bottle.tar.gz
    brew install bottles/ctl-1.5.2.mavericks.bottle.tar.gz
    brew install bottles/jpeg-turbo-1.3.1.mavericks.bottle.tar.gz
    brew install bottles/libraw-0.15.4.mavericks.bottle.tar.gz
    brew install bottles/seexpr-1.0.1.mavericks.bottle.tar.gz
    brew install bottles/tbb-4.2.4.mavericks.bottle.tar.gz
    brew install bottles/suite-sparse-4.2.1.mavericks.bottle.tar.gz
    brew install bottles/numpy-1.8.1.mavericks.bottle.tar.gz
    brew install bottles/openjpeg-1.5.1_1.mavericks.bottle.tar.gz
    brew install bottles/cfitsio-3.360.mavericks.bottle.tar.gz
    brew install bottles/szip-2.1.mavericks.bottle.tar.gz
    brew install bottles/hdf5-1.8.13.mavericks.bottle.tar.gz
    brew install bottles/field3d-1.4.3.mavericks.bottle.tar.gz
    brew install bottles/opencolorio-1.0.9.mavericks.bottle.tar.gz
    brew install bottles/openimageio-1.4.8.mavericks.bottle.tar.gz

    cp tools/sconf/macos_homebrew.sconf host.sconf

fi
