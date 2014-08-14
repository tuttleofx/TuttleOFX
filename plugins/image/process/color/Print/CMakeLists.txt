# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Declare the plugin
tuttle_ofx_plugin_target(Print)
tuttle_ofx_plugin_add_library(Print Caca)
