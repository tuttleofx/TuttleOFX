# Uses custom ofx c++ wrapper


include_directories(${PROJECT_SOURCE_DIR}/libraries/openfxHack/include)
include_directories(${PROJECT_SOURCE_DIR}/libraries/openfxHack/Support/include)

file(GLOB_RECURSE OFXHACK ${PROJECT_SOURCE_DIR}/libraries/openfxHack/Support/*.cpp)
file(GLOB_RECURSE TUTTLE_PLUGIN_FILES 
        ${PROJECT_SOURCE_DIR}/libraries/tuttle/src/tuttle/plugin/*.?pp)
file(GLOB_RECURSE TUTTLE_COMMON_FILES 
        ${PROJECT_SOURCE_DIR}/libraries/tuttle/src/tuttle/common/*.?pp)
