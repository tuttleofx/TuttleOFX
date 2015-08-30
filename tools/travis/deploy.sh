#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

if [[ ${TRAVIS_OS_NAME} == "linux" ]]; then
    # Fix additionnal dependencies of boost-regex
    cp /usr/lib/libicuuc.so.48 ${TUTTLE_INSTALL}/lib
    cp /usr/lib/libicui18n.so.48 ${TUTTLE_INSTALL}/lib
    cp /usr/lib/libicudata.so.48 ${TUTTLE_INSTALL}/lib
fi

# Create archive
cd ${TRAVIS_BUILD_DIR}
tar -czf ${TUTTLE_DEPLOY}-${CC}-python-${PYTHON_VERSION}.tgz install-tuttle
