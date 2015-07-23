#!/usr/bin/env bash

if [[ ${COVERITY_SCAN_BRANCH} == 1 ]]; then
    if [[ ${TRAVIS_OS_NAME} == "linux" &&
          ${CC} == "gcc"
       ]]; then
        export COVERITY_BUILD_DISABLED=0
        echo "Coverity: build enabled"
    else
        export COVERITY_BUILD_DISABLED=1
        echo "Coverity: build disabled"
    fi
else
    echo "No Coverity"
fi
