#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then

    # Install python packages to run sam command line
    if [[ ${PYTHON_VERSION} == "2.7" ]]; then
        pip install --user clint argcomplete
    # elif [[ ${PYTHON_VERSION} == "3.2" ]]; then
        # easy_install3 pip --prefix=$HOME/local
        # pip3 install --user clint argcomplete
    fi

    # Use TRAVIS_JOB_ID to detect that we are in travis.
    # In that case, use a simple check to detect if the cache is already there.
    if  [ -z ${TRAVIS_JOB_ID} ] || [ ! -d "${DEPENDENCIES_INSTALL}/lib/" ]; then

        # Target versions
        FFMPEG_RELEASE=ffmpeg-2.2.9
        ACES_RELEASE=aces_container-1.0
        CTL_RELEASE=ctl-1.5.2
        SEEXPR_VERSION=1.0.1
        SEEXPR_RELEASE=SeExpr-rel-$SEEXPR_VERSION
        OCIO_VERSION=1.0.9
        OCIO_RELEASE=OpenColorIO-$OCIO_VERSION
        LIBRAW_VERSION=0.16.2
        LIBRAW_RELEASE=LibRaw-$LIBRAW_VERSION
        OIIO_VERSION=1.5.16
        OIIO_RELEASE=oiio-Release-$OIIO_VERSION


        cd $TRAVIS_BUILD_DIR
        wget https://www.ffmpeg.org/releases/$FFMPEG_RELEASE.tar.bz2
        bunzip2 $FFMPEG_RELEASE.tar.bz2
        tar -xf $FFMPEG_RELEASE.tar > /dev/null 2>&1
        cd $FFMPEG_RELEASE
        ./configure --prefix=${DEPENDENCIES_INSTALL} --enable-shared --disable-static --disable-yasm && make && make install

        cd $TRAVIS_BUILD_DIR
        wget https://github.com/ampas/aces_container/archive/v1.0.tar.gz -O /tmp/$ACES_RELEASE.tar.gz
        tar -xzf /tmp/$ACES_RELEASE.tar.gz > /dev/null 2>&1
        mkdir $ACES_RELEASE/build
        cd $ACES_RELEASE/build
        cmake .. -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make && make install

        cd $TRAVIS_BUILD_DIR
        wget https://github.com/ampas/CTL/archive/$CTL_RELEASE.tar.gz -O /tmp/$CTL_RELEASE.tar.gz
        tar -xzf /tmp/$CTL_RELEASE.tar.gz > /dev/null 2>&1
        mkdir CTL-$CTL_RELEASE/build
        cd CTL-$CTL_RELEASE/build
        cmake .. -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make && make install

        cd $TRAVIS_BUILD_DIR
        wget https://github.com/wdas/SeExpr/archive/rel-$SEEXPR_VERSION.tar.gz -O /tmp/$SEEXPR_RELEASE.tar.gz
        tar -xzf /tmp/$SEEXPR_RELEASE.tar.gz > /dev/null 2>&1
        mkdir $SEEXPR_RELEASE/build
        cd $SEEXPR_RELEASE/build
        cmake .. -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make && make install

        cd $TRAVIS_BUILD_DIR
        wget https://github.com/imageworks/OpenColorIO/archive/v$OCIO_VERSION.tar.gz -O /tmp/$OCIO_RELEASE.tar.gz
        tar -xzf /tmp/$OCIO_RELEASE.tar.gz > /dev/null 2>&1
        mkdir $OCIO_RELEASE/build
        cd $OCIO_RELEASE/build
        cmake .. -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make && make install

        cd $TRAVIS_BUILD_DIR
        wget https://github.com/LibRaw/LibRaw/archive/$LIBRAW_VERSION.tar.gz -O /tmp/$LIBRAW_RELEASE.tar.gz
        tar -xzf /tmp/$LIBRAW_RELEASE.tar.gz > /dev/null 2>&1
        mkdir $LIBRAW_RELEASE/build
        cd $LIBRAW_RELEASE/build
        ./configure --prefix=${DEPENDENCIES_INSTALL} && make && make install

        cd $TRAVIS_BUILD_DIR
        wget https://github.com/OpenImageIO/oiio/archive/Release-$OIIO_VERSION.tar.gz -O /tmp/$OIIO_RELEASE.tar.gz
        tar -xzf /tmp/$OIIO_RELEASE.tar.gz > /dev/null 2>&1
        mkdir $OIIO_RELEASE/build
        cd $OIIO_RELEASE/build
        cmake .. -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} -DCMAKE_PREFIX_PATH=${DEPENDENCIES_INSTALL} -DCMAKE_CXX_FLAGS="-D__STDC_CONSTANT_MACROS" && make && make install

    else
        echo 'Using cached directory.';
    fi


elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then

    echo "Brew update"
    brew update > /dev/null 2>&1
    brew tap homebrew/python
    brew tap homebrew/science

    echo "Brew install python & nose"
    if [[ ${PYTHON_VERSION} == "2.7" ]]; then
        brew install python
        pip install nose
        pip install clint argcomplete
    elif [[ ${PYTHON_VERSION} == "3.2" ]]; then
        brew install python3
        pip3 install nose
        pip3 install clint argcomplete
    fi

    echo "Brew install packages"
    brew install swig ilmbase openexr jasper little-cms2 glew freetype fontconfig ffmpeg imagemagick libcaca aces_container ctl jpeg-turbo libraw seexpr openjpeg opencolorio openimageio numpy

fi
