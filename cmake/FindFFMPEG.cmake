# - Find FFMPEG library
# Find the native FFMPEG includes and library
# This module defines:
#
#  FFMPEG_INCLUDE_DIRS, where to find openimageio.h, Set when FFMPEG_INCLUDE_DIR is found.
#  FFMPEG_LIBRARIES, libraries to link against to use FFMPEG.
#  FFMPEG_ROOT_DIR, The base directory to search for FFMPEG.
#                        This can also be an environment variable.
#  FFMPEG_FOUND, If false, do not try to use FFMPEG.
#
# also defined, but not for general use are
#  FFMPEG_LIBRARY, where to find the FFMPEG library.
# 
# If FFMPEG_ROOT is defined in the environment this module will use it in priority


# If FFMPEG_ROOT is defined in the environment, use it.
if(NOT FFMPEG_ROOT_DIR AND NOT $ENV{FFMPEG_ROOT} STREQUAL "")
  set(FFMPEG_ROOT_DIR $ENV{FFMPEG_ROOT})
else()
    # If not environment variable is set, fall back on pkg-config
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(FFMPEGPKG QUIET FFMPEG)
    endif()
endif()

# Add few search directories
set(_ffmpeg_SEARCH_DIRS
  ${FFMPEG_ROOT_DIR}
  /usr/local
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
)

# Looking for include path
find_path(FFMPEG_INCLUDE_DIR
    NAMES         libavcodec/avcodec.h
    HINTS         ${_ffmpeg_SEARCH_DIRS} ${FFMPEGPKG_INCLUDE_DIRS}
  PATH_SUFFIXES  include 

)

# Looking for library path
find_library(FFMPEG_LIBRARY
    NAMES         avcodec avutil avformat
    HINTS         ${_ffmpeg_SEARCH_DIRS} ${FFMPEGPKG_LIBRARY_DIRS}
  PATH_SUFFIXES lib64 lib
)

# handle the QUIETLY and REQUIRED arguments and set FFMPEG_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFMPEG DEFAULT_MSG
    FFMPEG_LIBRARY FFMPEG_INCLUDE_DIR)

if(FFMPEG_FOUND)
  set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARY})
  set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIR})
endif(FFMPEG_FOUND)

# Set variable as advanced mode
mark_as_advanced(
  FFMPEG_INCLUDE_DIR
  FFMPEG_LIBRARY
)
