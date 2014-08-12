
# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

tuttle_ofx_plugin_target(Exr)

# Add external libraries
set(Exr_LIBRARIES OpenEXR sequenceParser)
tuttle_ofx_plugin_add_libraries(Exr "${Exr_LIBRARIES}")
