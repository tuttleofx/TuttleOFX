# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Declare the plugin
tuttle_ofx_plugin_target(ColorCubeViewer)
tuttle_ofx_plugin_add_library(ColorCubeViewer GLEW)

# FIXME the following include is here to fix the include "hsv" from gil
# and should definitely be removed
include_directories(${PROJECT_SOURCE_DIR}/libraries/boostHack)
