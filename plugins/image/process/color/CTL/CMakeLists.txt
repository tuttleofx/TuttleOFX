# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Declare the plugin
tuttle_ofx_plugin_target(CTL)
tuttle_ofx_plugin_add_library(CTL CTL)
