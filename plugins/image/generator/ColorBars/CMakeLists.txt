# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Find files for this plugin
file(GLOB_RECURSE COLORBARS_SRC_FILES src/*.cpp src/*.tcc)

# Declare the plugin
tuttle_ofx_plugin_target(ColorBars "${COLORBARS_SRC_FILES}")


