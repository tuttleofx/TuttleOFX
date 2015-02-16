# - Find LibCaca library
# Find the native LibCaca includes and library
# This module defines
#  LIBCACA_INCLUDE_DIRS, where to find openimageio.h, Set when
#                            LIBCACA_INCLUDE_DIR is found.
#  LIBCACA_LIBRARIES, libraries to link against to use LibCaca.
#  LIBCACA_ROOT_DIR, The base directory to search for LibCaca.
#                        This can also be an environment variable.
#  LIBCACA_FOUND, If false, do not try to use LibCaca.
#
# also defined, but not for general use are
#  LIBCACA_LIBRARY, where to find the LibCaca library.


# If LIBCACA_ROOT was defined in the environment, use it.
IF(NOT LIBCACA_ROOT_DIR AND NOT $ENV{LIBCACA_ROOT} STREQUAL "")
  SET(LIBCACA_ROOT_DIR $ENV{LIBCACA_ROOT})
ENDIF()

SET(_libcaca_SEARCH_DIRS
  ${LIBCACA_ROOT_DIR}
  /usr/local
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
)

FIND_PATH(LIBCACA_INCLUDE_DIR
    NAMES       caca.h 
  HINTS         ${_libcaca_SEARCH_DIRS} 
  PATH_SUFFIXES include
)

FIND_LIBRARY(LIBCACA_LIBRARY
    NAMES       caca
  HINTS         ${_libcaca_SEARCH_DIRS}
  PATH_SUFFIXES lib64 lib
)

# handle the QUIETLY and REQUIRED arguments and set LIBCACA_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibCaca DEFAULT_MSG
    LIBCACA_LIBRARY LIBCACA_INCLUDE_DIR)

IF(LIBCACA_FOUND)
  SET(LIBCACA_LIBRARIES ${LIBCACA_LIBRARY})
  SET(LIBCACA_INCLUDE_DIRS ${LIBCACA_INCLUDE_DIR} )
ENDIF(LIBCACA_FOUND)

MARK_AS_ADVANCED(
  LIBCACA_INCLUDE_DIR
  LIBCACA_LIBRARY
)
