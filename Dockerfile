FROM ubuntu:latest

MAINTAINER TuttleOFX <tuttleofx-dev@googlegroups.com>

RUN apt-get update && apt-get install -y \
  wget \
  build-essential \
  cmake \
  graphviz \
  git \
  swig \
  libboost1.55-all-dev \
  python-dev \
  python-numpy \
  python3-dev \
  python3-nose \
  libfreetype6-dev \
  libbz2-dev \
  libltdl-dev \
  libpng-dev \
  libjpeg-dev \
  libraw-dev \
  libglew-dev \
  libtiff4-dev \
  libilmbase-dev \
  libopenexr-dev \
  libopenimageio-dev \
  libglew-dev \
  libgraphviz-dev \
  graphviz-dev \
  python-nose \
  python-imaging \
  libopenjpeg-dev \
  libturbojpeg \
  libxmu-dev \
  libmp3lame-dev \
  liblzma-dev \
  libxt-dev \
  libmagickcore-dev \
  liblcms2-dev \
  libyaml-dev \
  libopenctl0.8 \
  libcaca-dev \
  libtinyxml-dev \
  yasm \
  libx264-dev \
  libxvidcore-dev

ENV PYTHON_VERSION=2.7
ENV TRAVIS_OS_NAME=linux
ENV TRAVIS_BUILD_DIR=/tmp/TuttleOFX_build
ENV TUTTLEOFX_DEV=/opt/TuttleOFX_git
ENV TUTTLEOFX_BUILD=/tmp/TuttleOFX_build_tmp
ENV TUTTLEOFX_INSTALL=/usr/local
ENV DEPENDENCIES_INSTALL=/usr/local
ENV PYTHONPATH=${PYTHONPATH}:/usr/local/lib/python${PYTHON_VERSION}/site-packages

RUN mkdir ${TUTTLEOFX_BUILD} ${TRAVIS_BUILD_DIR}

COPY . ${TUTTLEOFX_DEV}
RUN cd ${TUTTLEOFX_DEV} && git submodule update --init --recursive
RUN cd ${TUTTLEOFX_DEV} && ${TUTTLEOFX_DEV}/tools/travis/install_dependencies.sh
RUN cd ${TUTTLEOFX_DEV} && ${TUTTLEOFX_DEV}/tools/travis/build.sh


