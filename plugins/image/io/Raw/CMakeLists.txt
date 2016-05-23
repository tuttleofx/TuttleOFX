# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Declare the plugin
tuttle_ofx_plugin_target(Raw)
tuttle_ofx_plugin_add_library(Raw sequenceParser)
tuttle_ofx_plugin_add_library(Raw LibRaw)
