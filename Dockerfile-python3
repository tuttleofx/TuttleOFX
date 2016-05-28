FROM tuttleofx/tuttleofx-env:python3-latest

MAINTAINER TuttleOFX <tuttleofx-dev@googlegroups.com>

RUN mkdir ${TUTTLEOFX_BUILD} && \
    cd ${TUTTLEOFX_DEV} && \
    git submodule update --init --recursive && \
    ${TUTTLEOFX_DEV}/tools/travis/build.sh && \
    rm -rf ${TUTTLEOFX_BUILD}

