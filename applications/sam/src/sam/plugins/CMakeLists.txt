

# Load project cmake macros
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(TuttleExeMacros)

## sam plugins
add_tuttle_executable(sam-plugins main.cpp)
target_link_libraries(sam-plugins tuttleHost)
