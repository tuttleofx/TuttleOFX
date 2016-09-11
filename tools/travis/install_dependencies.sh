#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# Use TRAVIS_JOB_ID environment variable to detect if the script is run on Travis CI.
if [ -n ${TRAVIS_JOB_ID} ]; then
    if  [ ${TRAVIS_OS_NAME} == "linux" ]; then
        # Install python packages to run sam command line
        # On travis we need to install some python packages not available in the https://github.com/travis-ci/apt-package-whitelist
        if [[ ${PYTHON_VERSION} == 2* ]]; then
            pip install --user clint argcomplete
        elif [[ ${PYTHON_VERSION} == 3* ]]; then
            # For pip3: https://github.com/travis-ci/travis-ci/issues/1528		
            curl -Ss https://raw.github.com/pypa/pip/master/contrib/get-pip.py | python3 --user

            pip3 install --user clint argcomplete
        fi
        # If the cache of dependencies exists
        if [ -d "${DEPENDENCIES_INSTALL}/lib/" ]; then
            echo 'Using cached directory.';
            exit 0
        fi
    fi
fi

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then

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
    OIIO_VERSION=1.6.13
    OIIO_RELEASE=oiio-Release-$OIIO_VERSION

    cd $TRAVIS_BUILD_DIR
    wget https://www.ffmpeg.org/releases/$FFMPEG_RELEASE.tar.bz2
    bunzip2 $FFMPEG_RELEASE.tar.bz2
    tar -xf $FFMPEG_RELEASE.tar > /dev/null 2>&1
    cd $FFMPEG_RELEASE
    ./configure --prefix=${DEPENDENCIES_INSTALL} --enable-shared --disable-static --disable-yasm --disable-debug && make && make install

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/ampas/aces_container/archive/v1.0.tar.gz -O $ACES_RELEASE.tar.gz
    tar -xzf $ACES_RELEASE.tar.gz > /dev/null 2>&1
    mkdir $ACES_RELEASE/build
    cd $ACES_RELEASE/build
    cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make && make install/strip

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/ampas/CTL/archive/$CTL_RELEASE.tar.gz -O $CTL_RELEASE.tar.gz
    tar -xzf $CTL_RELEASE.tar.gz > /dev/null 2>&1
    mkdir CTL-$CTL_RELEASE/build
    cd CTL-$CTL_RELEASE/build
    cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make && make install/strip

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/wdas/SeExpr/archive/rel-$SEEXPR_VERSION.tar.gz -O $SEEXPR_RELEASE.tar.gz
    tar -xzf $SEEXPR_RELEASE.tar.gz > /dev/null 2>&1
    mkdir $SEEXPR_RELEASE/build
    cd $SEEXPR_RELEASE/build
    cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make && make install/strip

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/imageworks/OpenColorIO/archive/v$OCIO_VERSION.tar.gz -O $OCIO_RELEASE.tar.gz
    tar -xzf $OCIO_RELEASE.tar.gz > /dev/null 2>&1
    mkdir $OCIO_RELEASE/build
    cd $OCIO_RELEASE/build
    cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make && make install/strip

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/LibRaw/LibRaw/archive/$LIBRAW_VERSION.tar.gz -O $LIBRAW_RELEASE.tar.gz
    tar -xzf $LIBRAW_RELEASE.tar.gz > /dev/null 2>&1
    mkdir $LIBRAW_RELEASE/build
    cd $LIBRAW_RELEASE/build
    # edit CMake file to avoid LibRaw to install some files to /usr/ (permissions error)
    sed -l 590 -i 's/${CMAKE_ROOT}/share/g' ../CMakeLists.txt
    cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} && make && make install/strip

    cd $TRAVIS_BUILD_DIR
    wget https://github.com/OpenImageIO/oiio/archive/Release-$OIIO_VERSION.tar.gz -O $OIIO_RELEASE.tar.gz
    tar -xzf $OIIO_RELEASE.tar.gz > /dev/null 2>&1
    mkdir $OIIO_RELEASE/build
    cd $OIIO_RELEASE/build
    cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL} -DCMAKE_PREFIX_PATH=${DEPENDENCIES_INSTALL} -DCMAKE_CXX_FLAGS="-D__STDC_CONSTANT_MACROS" && make && make install/strip

elif [[ ${TRAVIS_OS_NAME} == "osx" ]]; then

    echo "Brew update"
    brew update > /dev/null 2>&1
    brew tap homebrew/python
    brew tap homebrew/science

    echo "Brew install python & nose"
    if [[ ${PYTHON_VERSION} == 2* ]]; then
        brew install python
        pip install nose
        pip install clint argcomplete
    elif [[ ${PYTHON_VERSION} == 3* ]]; then
        brew install python3
        pip3 install nose
        pip3 install clint argcomplete
    fi

    echo "Brew install packages"
    brew install swig ilmbase openexr jasper little-cms2 glew freetype fontconfig ffmpeg imagemagick libcaca aces_container ctl jpeg-turbo libraw seexpr openjpeg opencolorio openimageio numpy

fi
