
# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

tuttle_ofx_plugin_target(ImageMagick)

# Add external libraries with specific components
tuttle_ofx_plugin_add_library(ImageMagick ImageMagick COMPONENTS MagickCore)
tuttle_ofx_plugin_add_library(ImageMagick sequenceParser)

