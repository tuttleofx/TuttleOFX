# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# NOTE : boostHack must be included before boost for this plugin to compile
# FIXME should get rid of this include
include_directories(${PROJECT_SOURCE_DIR}/libraries/boostHack)
# Declare the plugin
tuttle_ofx_plugin_target(Merge)
