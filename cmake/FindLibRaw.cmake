# - Find LibRaw library
# Find the native LibRaw includes and library
# This module defines
#  LIBRAW_INCLUDE_DIRS, where to find openimageio.h, Set when
#                            LIBRAW_INCLUDE_DIR is found.
#  LIBRAW_LIBRARIES, libraries to link against to use LibRaw.
#  LIBRAW_ROOT_DIR, The base directory to search for LibRaw.
#                        This can also be an environment variable.
#  LIBRAW_FOUND, If false, do not try to use LibRaw.
#
# also defined, but not for general use are
#  LIBRAW_LIBRARY, where to find the LibRaw library.


# If LIBRAW_ROOT was defined in the environment, use it.
if(NOT LIBRAW_ROOT_DIR AND NOT $ENV{LIBRAW_ROOT} STREQUAL "")
  set(LIBRAW_ROOT_DIR $ENV{LIBRAW_ROOT})
else()
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(LIBRAWPKG QUIET libraw)
    endif()
endif()

set(_libraw_SEARCH_DIRS
  ${LIBRAW_ROOT_DIR}
  /usr/local
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt/lib/libraw
)

find_path(LIBRAW_INCLUDE_DIR
    NAMES         libraw/libraw.h
    HINTS         ${_libraw_SEARCH_DIRS} ${LIBRAW_INCLUDE_DIRS}
  PATH_SUFFIXES include
)

find_library(LIBRAW_LIBRARY
  NAMES       raw
  HINTS         ${_libraw_SEARCH_DIRS} ${LIBRAW_LIBRARY_DIRS}
  PATH_SUFFIXES lib64 lib
)

# handle the QUIETLY and REQUIRED arguments and set LIBRAW_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibRaw DEFAULT_MSG
    LIBRAW_LIBRARY LIBRAW_INCLUDE_DIR)

if(LIBRAW_FOUND)
  set(LIBRAW_LIBRARIES ${LIBRAW_LIBRARY})
  set(LIBRAW_INCLUDE_DIRS ${LIBRAW_INCLUDE_DIR} ${LIBRAW_INCLUDE_DIR}/LibRaw)
endif(LIBRAW_FOUND)

mark_as_advanced(
  LIBRAW_INCLUDE_DIR
  LIBRAW_LIBRARY
)
