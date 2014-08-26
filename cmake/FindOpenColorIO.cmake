# - Find OpenColorIO library
# Find the native OpenColorIO includes and library
# This module defines:
#
#  OPENCOLORIO_INCLUDE_DIRS, where to find openimageio.h, Set when OPENCOLORIO_INCLUDE_DIR is found.
#  OPENCOLORIO_LIBRARIES, libraries to link against to use OpenColorIO.
#  OPENCOLORIO_ROOT_DIR, The base directory to search for OpenColorIO.
#                        This can also be an environment variable.
#  OPENCOLORIO_FOUND, If false, do not try to use OpenColorIO.
#
# also defined, but not for general use are
#  OPENCOLORIO_LIBRARY, where to find the OpenColorIO library.
# 
# If OPENCOLORIO_ROOT is defined in the environment this module will use it in priority


# If OPENCOLORIO_ROOT is defined in the environment, use it.
if(NOT OPENCOLORIO_ROOT_DIR AND NOT $ENV{OPENCOLORIO_ROOT} STREQUAL "")
  set(OPENCOLORIO_ROOT_DIR $ENV{OPENCOLORIO_ROOT})
else()
    # If not environment variable is set, fall back on pkg-config
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(OCIOPKG QUIET OpenColorIO)
    endif()
endif()

# Add few search directories
set(_opencolorio_SEARCH_DIRS
  ${OPENCOLORIO_ROOT_DIR}
  /usr/local
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt/lib/ocio
)

# Looking for include path
find_path(OPENCOLORIO_INCLUDE_DIR
    NAMES         OpenColorIO/OpenColorIO.h
    HINTS         ${_opencolorio_SEARCH_DIRS} ${OCIOPKG_INCLUDE_DIRS}
  PATH_SUFFIXES  include
)

# Looking for library path
find_library(OPENCOLORIO_LIBRARY
    NAMES         OpenColorIO
    HINTS         ${_opencolorio_SEARCH_DIRS} ${OCIOPKG_LIBRARY_DIRS}
  PATH_SUFFIXES lib64 lib
)

# handle the QUIETLY and REQUIRED arguments and set OPENCOLORIO_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenColorIO DEFAULT_MSG
    OPENCOLORIO_LIBRARY OPENCOLORIO_INCLUDE_DIR)

if(OPENCOLORIO_FOUND)
  set(OPENCOLORIO_LIBRARIES ${OPENCOLORIO_LIBRARY})
  set(OPENCOLORIO_INCLUDE_DIRS ${OPENCOLORIO_INCLUDE_DIR} ${OPENCOLORIO_INCLUDE_DIR}/OpenColorIO)
endif(OPENCOLORIO_FOUND)

# Set variable as advanced mode
mark_as_advanced(
  OPENCOLORIO_INCLUDE_DIR
  OPENCOLORIO_LIBRARY
)
