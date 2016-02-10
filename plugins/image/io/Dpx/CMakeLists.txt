# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Include dpx-google-code to find libdpx
include_directories(src/dpx-google-code)

tuttle_ofx_plugin_target(Dpx)

# Add external libraries
tuttle_ofx_plugin_add_libraries(Dpx sequenceParser)
