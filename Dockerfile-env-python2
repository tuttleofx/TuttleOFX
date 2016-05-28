FROM ubuntu:16.04

MAINTAINER TuttleOFX <tuttleofx-dev@googlegroups.com>

RUN apt-get update && apt-get install -y \
  wget \
  build-essential \
  cmake \
  graphviz \
  git \
  swig \
  libboost-all-dev \
  python-dev \
  python-numpy \
  python-clint \
  python-argcomplete \
  python-nose \
  python-imaging \
  libfreetype6-dev \
  libbz2-dev \
  libltdl-dev \
  libpng-dev \
  libjpeg-dev \
  libglew-dev \
  libtiff5-dev \
  libilmbase-dev \
  libopenexr-dev \
  libglew-dev \
  libgraphviz-dev \
  graphviz-dev \
  libopenjpeg-dev \
  libturbojpeg \
  libxmu-dev \
  libmp3lame-dev \
  liblzma-dev \
  libxt-dev \
  libmagickcore-dev \
  liblcms2-dev \
  libyaml-dev \
  libcaca-dev \
  libtinyxml-dev \
  yasm \
  libx264-dev \
  libxvidcore-dev \
  bison \
  flex \
  && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

ENV PYTHON_VERSION=2.7 \
    TRAVIS_OS_NAME=linux \
    TRAVIS_BUILD_DIR=/tmp/TuttleOFX_build \
    TUTTLEOFX_DEV=/opt/TuttleOFX_git \
    TUTTLEOFX_BUILD=/tmp/TuttleOFX_build_tmp \
    TUTTLEOFX_INSTALL=/usr/local \
    OFX_PLUGIN_PATH=/usr/local/OFX \
    DEPENDENCIES_INSTALL=/usr/local \
    PYTHONPATH=${PYTHONPATH}:/usr/local/lib/python2.7/site-packages

COPY . ${TUTTLEOFX_DEV}

RUN mkdir ${TRAVIS_BUILD_DIR} && \
    cd ${TUTTLEOFX_DEV} && \
    ${TUTTLEOFX_DEV}/tools/travis/install_dependencies.sh && \
    rm -rf ${TRAVIS_BUILD_DIR}

