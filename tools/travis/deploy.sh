#!/usr/bin/env bash

# Exit immediately if a command exits with a non-zero status
set -e
# Print commands and their arguments as they are executed.
set -x

# Create archive
cd ${TRAVIS_BUILD_DIR}
tar -czf ${TUTTLEOFX_DEPLOY}-${CC}-python-${PYTHON_VERSION}.tgz install-tuttle
