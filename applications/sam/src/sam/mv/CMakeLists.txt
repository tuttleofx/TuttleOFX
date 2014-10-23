## sam mv

# Load project cmake macros
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(TuttleMacros)

tuttle_add_executable(sam-mv main.cpp)
tuttle_executable_add_library(sam-mv sequenceParser)


