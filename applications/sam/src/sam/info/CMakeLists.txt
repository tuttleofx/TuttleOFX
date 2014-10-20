## sam info

# Load project cmake macros
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(TuttleMacros)

tuttle_add_executable(sam-info main.cpp)
tuttle_executable_add_library(sam-info ImageMagick COMPONENTS MagickCore)
tuttle_executable_add_library(sam-info sequenceParser)

