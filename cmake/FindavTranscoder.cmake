# Look for AvTranscoder.
# In this project, AvTranscoder is retrieved as a submodule and built with AudioVideo plugin.

# Find ffmpeg libraries which are dependencies of avTranscoder
find_package(FFmpeg COMPONENTS avformat avcodec avutil swscale swresample)
if(FFMPEG_FOUND)
    set(AVTRANSCODER_FOUND 1)
else(FFMPEG_FOUND)
    set(AVTRANSCODER_FOUND 0)
endif(FFMPEG_FOUND)

set(AVTRANSCODER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/plugins/image/io/AudioVideo/avTranscoder/src)
file(GLOB_RECURSE AVTRANSCODER_FILES ${PROJECT_SOURCE_DIR}/plugins/image/io/AudioVideo/avTranscoder/src/*.?pp)

# Use target defined in avTranscoder project
set(AVTRANSCODER_LIBRARIES avtranscoder-shared)

# Look for the submodule
if(NOT EXISTS ${AVTRANSCODER_INCLUDE_DIR})
    message(FATAL_ERROR
            "\n'avTranscoder' submodule is missing, please update your repository:\n"
            "  > git submodule update -i\n")
endif()
