## sam do

# Load project cmake macros
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(TuttleMacros)

set(SAMDO_FILES main.cpp nodeDummy.cpp nodeDummy.hpp global.hpp commandLine.hpp)
tuttle_add_executable(sam-do "${SAMDO_FILES}")
tuttle_executable_add_library(sam-do sequenceParser)
tuttle_executable_add_library(sam-do tuttleHost)
tuttle_executable_add_library(sam-do Boost)

