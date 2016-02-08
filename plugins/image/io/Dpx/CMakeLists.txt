# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Declare the plugin
# TODO : the reader is not taken into account.
# if it is permanent, remove it from git
file(GLOB_RECURSE WRITER_PLUGIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/writer/*.?pp)
file(GLOB_RECURSE DPX_PLUGIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/dpx-google-code/*.cpp)
include_directories(src/dpx-google-code)

set(DPX_PLUGIN_SOURCES ${WRITER_PLUGIN_SOURCES} ${DPX_PLUGIN_SOURCES} src/mainEntry.cpp)
tuttle_ofx_plugin_target(Dpx "${DPX_PLUGIN_SOURCES}" TRUE)

# Add external libraries
tuttle_ofx_plugin_add_libraries(Dpx sequenceParser)
