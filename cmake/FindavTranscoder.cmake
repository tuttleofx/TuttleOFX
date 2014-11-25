# Look for AvTranscoder.
# In this project, AvTranscoder is retrieved as a submodule and built with TuttleOFX.

set(AVTRANSCODER_FOUND 1)

set(AVTRANSCODER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libraries/avTranscoder/src)
file(GLOB_RECURSE AVTRANSCODER_FILES ${PROJECT_SOURCE_DIR}/libraries/avTranscoder/src/*.?pp)

# Use target defined in avTranscoder project
set(AVTRANSCODER_LIBRARIES avtranscoder-shared)
