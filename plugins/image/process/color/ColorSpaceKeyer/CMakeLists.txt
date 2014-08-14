# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Declare the plugin
tuttle_ofx_plugin_target(ColorSpaceKeyer)
tuttle_ofx_plugin_add_library(ColorSpaceKeyer GLEW)

# FIXME should get rid of this include
include_directories(${PROJECT_SOURCE_DIR}/libraries/boostHack)
