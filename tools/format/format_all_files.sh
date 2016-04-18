#!/bin/bash

# libraries
find ../../libraries -type f \( -iname \*.?pp \) -exec clang-format -i {} \;
# plugins
find ../../plugins -type f \( -iname \*.?pp \) -exec clang-format -i {} \;
# applications
find ../../applications -type f \( -iname \*.?pp \) -exec clang-format -i {} \;
