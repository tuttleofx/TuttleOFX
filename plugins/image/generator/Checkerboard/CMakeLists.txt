
#find_package(openfx)

#if (OPENFX_FOUND) {

include_directories(${PROJECT_SOURCE_DIR}/libraries/tuttle/src/)

# Use Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF) 
find_package(Boost 1.55.0)

# TODO if boost found
include_directories(${Boost_INCLUDE_DIRS}) 

# OpenFX
include(UseOFXPP)

# Terry image library
include(UseTerry)

# TODO : a special install for OFX plugins
file(GLOB_RECURSE CHECKERBOARD_SRC_FILES src/*.cpp src/*.tcc)
add_library(CheckerBoard SHARED ${CHECKERBOARD_SRC_FILES})


