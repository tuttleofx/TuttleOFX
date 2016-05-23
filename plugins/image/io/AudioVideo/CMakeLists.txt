# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Create target AudioVideo
tuttle_ofx_plugin_target(AudioVideo)

# Find ffmpeg libraries which are dependencies of avTranscoder
find_package(FFmpeg COMPONENTS avformat avcodec avutil swscale swresample)
if(FFMPEG_FOUND)
    # Install ffmpeg libraries
    tuttle_find_package_paths(FFmpeg incvars libpaths libvars)
    foreach(var ${libvars})
        tuttle_install_shared_libs("${${var}}") 
    endforeach()
    # Build avTranscoder library
    set(AVTRANSCODER_DISABLE_APPS True)
    set(AVTRANSCODER_DISABLE_BINDINGS True)
    set(AVTRANSCODER_PYTHON_VERSION_OF_BINDING ${TUTTLE_PYTHON_VERSION})
    add_subdirectory(avTranscoder)
endif(FFMPEG_FOUND)

# Add external libraries
tuttle_ofx_plugin_add_library(AudioVideo avTranscoder)
tuttle_ofx_plugin_add_library(AudioVideo sequenceParser)
