# - Find OpenJpeg library
# Find the native OpenJpeg includes and library
# This module defines:
#
#  OPENJPEG_INCLUDE_DIRS, where to find openimageio.h, Set when OPENJPEG_INCLUDE_DIR is found.
#  OPENJPEG_LIBRARIES, libraries to link against to use OpenJpeg.
#  OPENJPEG_ROOT_DIR, The base directory to search for OpenJpeg.
#                        This can also be an environment variable.
#  OPENJPEG_FOUND, If false, do not try to use OpenJpeg.
#
# also defined, but not for general use are
#  OPENJPEG_LIBRARY, where to find the OpenJpeg library.
# 
# If OPENJPEG_ROOT is defined in the environment this module will use it in priority


# If OPENJPEG_ROOT is defined in the environment, use it.
if(NOT OPENJPEG_ROOT_DIR AND NOT $ENV{OPENJPEG_ROOT} STREQUAL "")
  set(OPENJPEG_ROOT_DIR $ENV{OPENJPEG_ROOT})
endif()

# Add few search directories
set(_openjpeg_SEARCH_DIRS
  ${OPENJPEG_ROOT_DIR}
  /usr/local
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
)

# Looking for include path
find_path(OPENJPEG_INCLUDE_DIR
    NAMES         openjpeg.h
    HINTS         ${_openjpeg_SEARCH_DIRS}
  PATH_SUFFIXES  include include/openjpeg-1.0 include/openjpeg-1.5 include/openjpeg-2.0 include/openjpeg-2.1
)

# Looking for library path
find_library(OPENJPEG_LIBRARY
    NAMES         openjpeg openjp2
    HINTS         ${_openjpeg_SEARCH_DIRS}
  PATH_SUFFIXES lib64 lib
)

# handle the QUIETLY and REQUIRED arguments and set OPENJPEG_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenJpeg DEFAULT_MSG
    OPENJPEG_LIBRARY OPENJPEG_INCLUDE_DIR)

if(OPENJPEG_FOUND)
  set(OPENJPEG_LIBRARIES ${OPENJPEG_LIBRARY})
  set(OPENJPEG_INCLUDE_DIRS ${OPENJPEG_INCLUDE_DIR})
endif(OPENJPEG_FOUND)

# Set variable as advanced mode
mark_as_advanced(
  OPENJPEG_INCLUDE_DIR
  OPENJPEG_LIBRARY
)
