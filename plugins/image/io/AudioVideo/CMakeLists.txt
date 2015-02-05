# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Create target AudioVideo
tuttle_ofx_plugin_target(AudioVideo)

# Add external libraries
tuttle_ofx_plugin_add_library(AudioVideo avTranscoder)
tuttle_ofx_plugin_add_library(AudioVideo sequenceParser)

