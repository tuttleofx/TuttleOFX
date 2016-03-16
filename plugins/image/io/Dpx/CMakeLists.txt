# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

tuttle_ofx_plugin_target(Dpx "" True)

# Add include of dpx-google-code only to DPX plugin
target_include_directories(Dpx PUBLIC "src/dpx-google-code")

# Add external libraries
tuttle_ofx_plugin_add_libraries(Dpx sequenceParser)
