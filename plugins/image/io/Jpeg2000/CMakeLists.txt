# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

tuttle_ofx_plugin_target(Jpeg2000)

# Add external libraries
tuttle_ofx_plugin_add_library(Jpeg2000 JPEG2K)
tuttle_ofx_plugin_add_library(Jpeg2000 sequenceParser)
