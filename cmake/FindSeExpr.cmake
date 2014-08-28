# - Find SeExpr library
# Find the native SeExpr includes and library
# This module defines
#  SEEXPR_INCLUDE_DIRS, where to find openimageio.h, Set when
#                            SEEXPR_INCLUDE_DIR is found.
#  SEEXPR_LIBRARIES, libraries to link against to use SeExpr.
#  SEEXPR_ROOT_DIR, The base directory to search for SeExpr.
#                        This can also be an environment variable.
#  SEEXPR_FOUND, If false, do not try to use SeExpr.
#
# also defined, but not for general use are
#  SEEXPR_LIBRARY, where to find the SeExpr library.


# If SEEXPR_ROOT was defined in the environment, use it.
IF(NOT SEEXPR_ROOT_DIR AND NOT $ENV{SEEXPR_ROOT} STREQUAL "")
  SET(SEEXPR_ROOT_DIR $ENV{SEEXPR_ROOT})
ENDIF()

SET(_seexpr_SEARCH_DIRS
  ${SEEXPR_ROOT_DIR}
  /usr/local
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
)

FIND_PATH(SEEXPR_INCLUDE_DIR
    NAMES       SeExpression.h
  HINTS         ${_seexpr_SEARCH_DIRS} 
  PATH_SUFFIXES include
)

FIND_LIBRARY(SEEXPR_LIBRARY
    NAMES       SeExpr
  HINTS         ${_seexpr_SEARCH_DIRS}
  PATH_SUFFIXES lib64 lib
)

# handle the QUIETLY and REQUIRED arguments and set SEEXPR_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SeExpr DEFAULT_MSG
    SEEXPR_LIBRARY SEEXPR_INCLUDE_DIR)

IF(SEEXPR_FOUND)
  SET(SEEXPR_LIBRARIES ${SEEXPR_LIBRARY})
  SET(SEEXPR_INCLUDE_DIRS ${SEEXPR_INCLUDE_DIR} )
ENDIF(SEEXPR_FOUND)

MARK_AS_ADVANCED(
  SEEXPR_INCLUDE_DIR
  SEEXPR_LIBRARY
)
