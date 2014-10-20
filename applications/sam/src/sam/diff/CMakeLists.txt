## sam diff

# Load project cmake macros
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(TuttleMacros)

tuttle_add_executable(sam-diff main.cpp)
tuttle_executable_add_library(sam-diff sequenceParser)
tuttle_executable_add_library(sam-diff tuttleHost)


