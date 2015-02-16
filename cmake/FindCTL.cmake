# - Find CTL library 
# CTL stands for Color transform language 
# http://ampasctl.sourceforge.net/
# Find the native CTL includes and library
# This module defines
#  CTL_INCLUDE_DIRS, where to find openimageio.h, Set when
#                            CTL_INCLUDE_DIR is found.
#  CTL_LIBRARIES, libraries to link against to use CTL.
#  CTL_ROOT_DIR, The base directory to search for CTL.
#                        This can also be an environment variable.
#  CTL_FOUND, If false, do not try to use CTL.
#


# If CTL_ROOT was defined in the environment, use it, otherwise 
# try to use pkgconfig
if(NOT CTL_ROOT_DIR AND NOT $ENV{CTL_ROOT} STREQUAL "")
  set(CTL_ROOT_DIR $ENV{CTL_ROOT})
else()
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(CTLPKG QUIET CTL)
    endif()
endif()

set(_ctl_SEARCH_DIRS
  ${CTL_ROOT_DIR}
  /usr/local
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt/lib/ocio
)

find_path(CTL_INCLUDE_DIR
    NAMES         CTL/CtlVersion.h
    HINTS         ${_ctl_SEARCH_DIRS} ${CTLPKG_INCLUDE_DIRS}
  PATH_SUFFIXES include
)

find_library(CTL_ILMIMF_LIB
  NAMES       IlmImfCtl 
  HINTS         ${_ctl_SEARCH_DIRS} ${CTLPKG_LIBRARY_DIRS}
  PATH_SUFFIXES lib64 lib
)

find_library(CTL_ILMCTL_LIB
  NAMES       IlmCtl 
  HINTS         ${_ctl_SEARCH_DIRS} ${CTLPKG_LIBRARY_DIRS}
  PATH_SUFFIXES lib64 lib
)

find_library(CTL_ILMCTLMATH_LIB
  NAMES       IlmCtlMath 
  HINTS         ${_ctl_SEARCH_DIRS} ${CTLPKG_LIBRARY_DIRS}
  PATH_SUFFIXES lib64 lib
)

find_library(CTL_ILMCTLSIMD_LIB
  NAMES       IlmCtlSimd
  HINTS         ${_ctl_SEARCH_DIRS} ${CTLPKG_LIBRARY_DIRS}
  PATH_SUFFIXES lib64 lib
)

set(CTL_LIBS ${CTL_ILMIMF_LIB} ${CTL_ILMCTL_LIB} ${CTL_ILMCTLMATH_LIB} ${CTL_ILMCTLSIMD_LIB})

# handle the QUIETLY and REQUIRED arguments and set CTL_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CTL DEFAULT_MSG
    CTL_LIBS CTL_INCLUDE_DIR)

if(CTL_FOUND)
  set(CTL_LIBRARIES ${CTL_LIBS})
  set(CTL_INCLUDE_DIRS ${CTL_INCLUDE_DIR} ${CTL_INCLUDE_DIR}/CTL)
endif(CTL_FOUND)

mark_as_advanced(
  CTL_INCLUDE_DIR
  CTL_LIBRARIES
)
