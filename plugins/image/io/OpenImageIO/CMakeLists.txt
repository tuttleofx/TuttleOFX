# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

tuttle_ofx_plugin_target(OpenImageIO "" TRUE)

# Add external libraries
tuttle_ofx_plugin_add_library(OpenImageIO OpenImageIO)
tuttle_ofx_plugin_add_library(OpenImageIO sequenceParser)

# FIXME : as Terry needs to load half.h
tuttle_ofx_plugin_add_library(OpenImageIO IlmBase) 
tuttle_ofx_plugin_add_library(OpenImageIO OpenEXR) 

