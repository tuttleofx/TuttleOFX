# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Create target AudioVideo
tuttle_ofx_plugin_target(AudioVideo)

# Find ffmpeg libraries which are dependencies of avTranscoder
find_package(FFmpeg COMPONENTS avformat avcodec avutil swscale swresample)
if(FFMPEG_FOUND)
    # Build avTranscoder library
    add_subdirectory(avTranscoder)
endif(FFMPEG_FOUND)

# Add external libraries
tuttle_ofx_plugin_add_library(AudioVideo avTranscoder)
tuttle_ofx_plugin_add_library(AudioVideo sequenceParser)
