# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Declare the plugin
file(GLOB_RECURSE PLUGIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.?pp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)

# Create the plugin
tuttle_ofx_plugin_target(YourPlugin "${PLUGIN_SOURCES}")

