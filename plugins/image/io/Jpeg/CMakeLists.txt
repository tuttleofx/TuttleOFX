# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

tuttle_ofx_plugin_target(Jpeg)

# Add external libraries
tuttle_ofx_plugin_add_library(Jpeg JPEG)
tuttle_ofx_plugin_add_library(Jpeg sequenceParser)
