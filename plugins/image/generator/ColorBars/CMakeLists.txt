
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
# TODO : find_package(OpenFX)
include_directories(${PROJECT_SOURCE_DIR}/libraries/openfxHack/include)
include_directories(${PROJECT_SOURCE_DIR}/libraries/openfxHack/Support/include)

# Terry
# TODO : find_package(terry)
include_directories(${PROJECT_SOURCE_DIR}/libraries/terry/src)

# TODO : a special install for OFX plugins

file(GLOB_RECURSE COLORBARS_SRC_FILES src/*.cpp src/*.tcc)
add_library(ColorBars SHARED ${COLORBARS_SRC_FILES})


