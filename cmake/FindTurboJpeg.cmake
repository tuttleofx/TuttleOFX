# - Find TurboJpeg library
# Find the native TurboJpeg includes and library
# This module defines:
#
#  TURBOJPEG_INCLUDE_DIRS, where to find openimageio.h, Set when TURBOJPEG_INCLUDE_DIR is found.
#  TURBOJPEG_LIBRARIES, libraries to link against to use TurboJpeg.
#  TURBOJPEG_ROOT_DIR, The base directory to search for TurboJpeg.
#                        This can also be an environment variable.
#  TURBOJPEG_FOUND, If false, do not try to use TurboJpeg.
#
# also defined, but not for general use are
#  TURBOJPEG_LIBRARY, where to find the TurboJpeg library.
# 
# If TURBOJPEG_ROOT is defined in the environment this module will use it in priority


# If TURBOJPEG_ROOT is defined in the environment, use it.
if(NOT TURBOJPEG_ROOT_DIR AND NOT $ENV{TURBOJPEG_ROOT} STREQUAL "")
  set(TURBOJPEG_ROOT_DIR $ENV{TURBOJPEG_ROOT})
else()
    # If not environment variable is set, fall back on pkg-config
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(TURBOJPEGPKG QUIET TurboJpeg)
    endif()
endif()

# Add few search directories
set(_turbojpeg_SEARCH_DIRS
  ${TURBOJPEG_ROOT_DIR}
  /usr/local
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt/lib/ocio
)

# Looking for include path
find_path(TURBOJPEG_INCLUDE_DIR
    NAMES         turbojpeg.h
    HINTS         ${_turbojpeg_SEARCH_DIRS} ${TURBOJPEGPKG_INCLUDE_DIRS}
  PATH_SUFFIXES  include
)

# Looking for library path
# On debian based system there is no "libturbojpeg.so" without specific version.
# So we look first for "libturbojpeg.so.0".
find_library(TURBOJPEG_LIBRARY
    NAMES       libturbojpeg.so.0 turbojpeg
    HINTS         ${_turbojpeg_SEARCH_DIRS} ${TURBOJPEGPKG_LIBRARY_DIRS}
  PATH_SUFFIXES lib64 lib
)

# handle the QUIETLY and REQUIRED arguments and set TURBOJPEG_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TurboJpeg DEFAULT_MSG
    TURBOJPEG_LIBRARY TURBOJPEG_INCLUDE_DIR)

if(TURBOJPEG_FOUND)
  set(TURBOJPEG_LIBRARIES ${TURBOJPEG_LIBRARY})
  set(TURBOJPEG_INCLUDE_DIRS ${TURBOJPEG_INCLUDE_DIR})
endif(TURBOJPEG_FOUND)

# Set variable as advanced mode
mark_as_advanced(
  TURBOJPEG_INCLUDE_DIR
  TURBOJPEG_LIBRARY
)
