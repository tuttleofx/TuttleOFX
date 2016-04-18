#!/usr/bin/env bash

samOK=(
# sam ls
    "sam ls"
    "sam ls -a"
    "sam ls -d"
    "sam ls -s"
    "sam ls -f"
    "sam ls -e *.cmake"
    "sam ls -R"
    "sam ls -L 2"
    "sam ls --absolute-path"
    "sam ls --relative-path"
    "sam ls --no-color"
    "sam ls --detect-negative"
    "sam ls --detect-without-holes"
    "sam ls --explode-sequences"
    "sam ls --script"
    "sam ls -v trace"
    "sam ls -v 10"
# sam do
    "sam do"
    "sam do --help"
    "sam do --nodes"
    "sam do --file-formats"
    "sam do blur -h"
    "sam do jpegreader plop.jpg // viewer"
)

samKO=(
# sam
    "sam"
    "sam toto"
# sam ls
    "sam ls toto"
    "sam ls -e"
    "sam ls -L"
    "sam ls -L -1"
    "sam ls --absolute-path --relative-path"
# sam do
    "sam do toto"
    "sam do //"
    "sam do jpegreader plop.jpg // viewer --stop-on-missing-files"
)

# Check all sam commands which are expected to be OK.
for sam in "${samOK[@]}"
do
    $sam
    if [ $? -eq 0 ]; then
        echo ">>> '$sam' command succeeded: expected."
    else
        echo ">>> '$sam' command failed: unexpected."
        exit 2
    fi
done

# Check all sam commands which are expected to be KO.
for sam in "${samKO[@]}"
do
    $sam
    if [ $? -eq 0 ]; then
        echo ">>> '$sam' command succeeded: unexpected."
        exit 2
    else
        echo ">>> '$sam' command failed: expected."
    fi
done
