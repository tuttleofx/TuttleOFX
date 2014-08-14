# - Find Ampas Color Transform Language
# Find CTL headers and libraries.
#
#  CTL_INCLUDE_DIRS - Where ctl includes are.
#  CTL_LIBRARIES    - List of libraries to compile with.
#  CTL_FOUND        - ctl is found.

# If CTL_ROOT_DIR was defined in the environment, use it.
if(NOT CTL_ROOT_DIR AND NOT $ENV{CTL_ROOT_DIR} STREQUAL "")
  set(CTL_ROOT_DIR $ENV{CTL_ROOT_DIR})
endif()

# Sets a list of path to look into
set(ctl_search_dirs
  ${CTL_ROOT_DIR}
  /usr/local
  /sw 
  /opt/local 
  /opt/csw 
)

# Look for the header file.
find_path(CTL_INCLUDE_DIR 
    NAMES CtlSimdInterpreter.h
    HINTS ${ctl_search_dirs}
    PATH_SUFFIXES include/CTL)

find_path(CTLEXR_INCLUDE_DIR 
    NAMES ImfCtlApplyTransforms.h 
    HINTS ${ctl_search_dirs}
    PATH_SUFFIXES include/OpenEXR)

# Look for the library.
find_library(CTL_LIBRARY NAMES IlmCtl HINTS ${ctl_search_dirs} PATH_SUFFIXES lib)
find_library(CTL_MATH_LIBRARY NAMES IlmCtlMath HINTS ${ctl_search_dirs} PATH_SUFFIXES lib)
find_library(CTL_SIMD_LIBRARY NAMES IlmCtlSimd HINTS ${ctl_search_dirs} PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CTL 
    DEFAULT_MSG CTL_LIBRARY CTL_MATH_LIBRARY CTL_SIMD_LIBRARY CTL_INCLUDE_DIR CTLEXR_INCLUDE_DIR)

# Copy the results to the output variables.
if(CTL_FOUND)
  set(CTL_LIBRARIES ${CTL_LIBRARY} ${CTL_MATH_LIBRARY} ${CTL_SIMD_LIBRARY})
  set(CTL_INCLUDE_DIRS ${CTL_INCLUDE_DIR} ${CTLEXR_INCLUDE_DIR})
else(CTL_FOUND)
  set(CTL_LIBRARIES)
  set(CTL_INCLUDE_DIRS)
endif(CTL_FOUND)

mark_as_advanced(CTL_INCLUDE_DIR CTL_LIBRARY CTL_MATH_LIBRARY CTL_SIMD_LIBRARY)

