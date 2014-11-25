## sam rm

# Load project cmake macros
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(TuttleMacros)

tuttle_add_executable(sam-rm main.cpp)
tuttle_executable_add_library(sam-rm sequenceParser)


