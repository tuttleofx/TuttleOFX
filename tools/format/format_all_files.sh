#!/bin/bash

# libraries
find ../../libraries -type f -iname "*.?pp" -or -iname "*.tcc" -exec clang-format -i {} \;
# plugins
find ../../plugins -type f -iname "*.?pp" -or -iname "*.tcc" -exec clang-format -i {} \;
# applications
find ../../applications -type f -iname "*.?pp" -or -iname "*.tcc" -exec clang-format -i {} \;
