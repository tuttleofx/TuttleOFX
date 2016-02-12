# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

tuttle_ofx_plugin_target(Jpeg2000 "" TRUE)

# Add external libraries
tuttle_ofx_plugin_add_library(Jpeg2000 OpenJpeg)
tuttle_ofx_plugin_add_library(Jpeg2000 sequenceParser)
