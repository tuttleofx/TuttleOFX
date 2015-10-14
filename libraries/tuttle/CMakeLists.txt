# CMakeLists.txt for Tuttle host library 
# This file needs to be split in several simple one
# The folder tree must be reorganized

# Add custom cmake macros
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(${TUTTLE_DEPLOY_DEPENDENCIES})
  # All libraries will be put in INSTALL_PREFIX/lib 
  # RPATH of host points INSTALL_PREFIX/lib
  # see: http://www.cmake.org/Wiki/CMake_RPATH_handling
  set(CMAKE_MACOSX_RPATH 1)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# Tuttle source code
include_directories(src)
file(GLOB_RECURSE TUTTLE_HOST_SRC_FILES src/tuttle/host/*.?pp src/tuttle/common/*.?pp)
file(GLOB_RECURSE TUTTLE_HOST_HEADER_FILES src/tuttle/host/*.hpp src/tuttle/common/*.hpp)

# Boost dependencies
include(UseTuttleBoost)
if (TuttleBoost_FOUND) 
  
  # OpenFX and Terry rely on boost
  include(UseOfxpp)
  include(UseTerry)

  # Extract tuttleHost_VERSION_MAJOR/MINOR/MICRO from version.hpp
  file(STRINGS "src/tuttle/host/version.hpp" _tuttleHost_VERSION_HPP_CONTENTS REGEX "#define TUTTLE_HOST_VERSION_")
  foreach(v MAJOR MINOR MICRO)
    if("${_tuttleHost_VERSION_HPP_CONTENTS}" MATCHES "#define TUTTLE_HOST_VERSION_${v} ([0-9]+)")
      set(tuttleHost_VERSION_${v} "${CMAKE_MATCH_1}")
    else()
      set(tuttleHost_RETRIEVE_VERSION_FAILED 1)
    endif()
  endforeach()
  unset(_tuttleHost_VERSION_HPP_CONTENTS)

  if(tuttleHost_RETRIEVE_VERSION_FAILED)
    message(WARNING "Failed to retrieve tuttleHost version (from version.hpp): "
		   "${tuttleHost_VERSION_MAJOR}.${tuttleHost_VERSION_MINOR}.${tuttleHost_VERSION_MICRO}")
  else()
    message(STATUS "tuttleHost version is "
                 "${tuttleHost_VERSION_MAJOR}.${tuttleHost_VERSION_MINOR}.${tuttleHost_VERSION_MICRO} (retrieved from version.hpp)")
  endif()

  # SequenceParser, as a separate project
  find_package(sequenceParser)

  # Target is a shared library
  add_library(tuttleHost SHARED ${TUTTLE_HOST_SRC_FILES})
  set_target_properties(tuttleHost PROPERTIES VERSION "${tuttleHost_VERSION_MAJOR}.${tuttleHost_VERSION_MINOR}.${tuttleHost_VERSION_MICRO}")
  target_include_directories(tuttleHost PUBLIC ${SEQUENCEPARSER_INCLUDE_DIR})
  target_include_directories(tuttleHost PUBLIC ${Boost_INCLUDE_DIRS})
  target_link_libraries(tuttleHost ${TuttleHostBoost_LIBRARIES})
  if(APPLE)
      set_target_properties(tuttleHost PROPERTIES LINK_FLAGS "-framework CoreFoundation -w")
      set_target_properties(tuttleHost PROPERTIES INSTALL_RPATH "@loader_path")
  endif(APPLE)
  if (UNIX AND NOT APPLE)
      target_link_libraries(tuttleHost dl)
      set_target_properties(tuttleHost
            PROPERTIES INSTALL_RPATH "$ORIGIN")
  endif(UNIX AND NOT APPLE)
  
  # TuttleHost depends on libpng when it is built in debug.
  # To generate png images when for each output of each nodes computed in the graph.
  if(CMAKE_BUILD_TYPE MATCHES RELEASE)
    find_package(PNG)
    if(PNG_FOUND) 
      target_link_libraries(tuttleHost ${PNG_LIBRARIES})
    endif(PNG_FOUND)
  endif(CMAKE_BUILD_TYPE MATCHES RELEASE)

  # TODO: if cmake >= 2.8.11
  # TODO: declare BOOST Atomic optional.
  # There is a basic fallback implementation for boost_atomic:
  # libraries/tuttle/src/tuttle/common/atomic.hpp
  # target_compile_definitions(tuttleHost PUBLIC WITHOUT_BOOST_ATOMIC)
  
  # Adds a support library for common and plugins
  # only compile object files, and use them at link time
  add_library(tuttleCommon STATIC ${TUTTLE_COMMON_FILES})
  target_include_directories(tuttleCommon PUBLIC ${Boost_INCLUDE_DIRS})
  set_target_properties(tuttleCommon PROPERTIES COMPILE_FLAGS "-fPIC")
  find_package(Threads)
  target_link_libraries(tuttleCommon ${CMAKE_THREAD_LIBS_INIT})
  target_link_libraries(tuttleCommon ${TuttleCommonBoost_LIBRARIES})

  # Utilities for OFX plugins
  add_library(tuttlePluginLib STATIC ${OFXPP_FILES})
  target_include_directories(tuttlePluginLib PUBLIC ${SEQUENCEPARSER_INCLUDE_DIR})
  set_target_properties(tuttlePluginLib PROPERTIES COMPILE_FLAGS "-fPIC")
  target_link_libraries(tuttlePluginLib ${SEQUENCEPARSER_LIBRARIES}) 
  target_link_libraries(tuttlePluginLib tuttleCommon)

  # Install tuttle host library in INSTALL_PREFIX/lib/
  install(TARGETS tuttleHost DESTINATION lib/ OPTIONAL)
  install(TARGETS tuttleCommon DESTINATION lib/ OPTIONAL)
  install(TARGETS tuttlePluginLib DESTINATION lib/ OPTIONAL)

  # Install tuttle host headers
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/tuttle"
    DESTINATION "include"
    FILES_MATCHING
    PATTERN "*.hpp"
    PATTERN "*.h"
    PATTERN "*.tcc"
    PATTERN "*.i"
  )

  # Install boost library along with tuttle library
  # Need to copy the files pointed by symlinks instead of the symlinks
  include(TuttleMacros)
  tuttle_install_shared_libs("${TuttleCommonBoost_LIBRARIES}")
  tuttle_install_shared_libs("${TuttleHostBoost_LIBRARIES}")

  # Python bindings with Swig 
  find_package(SWIG)
  if (SWIG_FOUND)
    
    # Load Modules/UseSWIG.cmake
    include(${SWIG_USE_FILE})
  
    # Find python c libs
    find_package(PythonLibs ${TUTTLE_PYTHON_VERSION})
    include_directories(${PYTHON_INCLUDE_DIRS})

    # Numpy
    if(WITHOUT_NUMPY)
        # Add swig flag to create binding without Numpy
        message("Build TuttleOFX python binding without numpy module.")
        set(SWIG_NUMPY_FLAG "-DWITHOUT_NUMPY")
    else()
        # Find numpy headers
        find_package(Numpy)
        if(NUMPY_FOUND)
            include_directories(${NUMPY_INCLUDE_DIR})
        else()
            set(SWIG_NUMPY_FLAG "-DWITHOUT_NUMPY")
        endif()
    endif()

    # Swig to compile c++ -> python
    set(CMAKE_SWIG_FLAGS -c++ -threads -Wall -small -fcompact -O -modern ${SWIG_NUMPY_FLAG}) #-doxygen
    if(${PYTHONLIBS_VERSION_STRING} VERSION_GREATER "3")
         set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -py3)
    endif()

    # pyTuttle
    set(TUTTLE_PYTHON_BINDING_FILE src/tuttle/host/tuttle.i)
    file(GLOB_RECURSE TUTTLE_PYTHON_ALL_BINDING_FILES src/tuttle/*.i)
    # FIXME : do we need a python package as before ?
    set_source_files_properties(${TUTTLE_PYTHON_BINDING_FILE} PROPERTIES CPLUSPLUS ON) 
    set(SWIG_MODULE_tuttle_EXTRA_DEPS "${TUTTLE_PYTHON_ALL_BINDING_FILES};${TUTTLE_HOST_HEADER_FILES}")

    # Create the output dir as it's not done automatically by cmake
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/tuttle/host)
    swig_add_module(tuttle python ${TUTTLE_PYTHON_BINDING_FILE})
    swig_link_libraries(tuttle tuttleHost)
    swig_link_libraries(tuttle ${TuttleHostBoost_LIBRARIES})
    swig_link_libraries(tuttle ${PYTHON_LIBRARIES})
    # Force the library to look in the project lib folder relatively
    if (APPLE)
      set_target_properties(${SWIG_MODULE_tuttle_REAL_NAME} PROPERTIES INSTALL_RPATH "@loader_path/../../..")
    else(APPLE)
        set_target_properties(${SWIG_MODULE_tuttle_REAL_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN/../../..:$ORIGIN")
    endif(APPLE)

    # get path to install python bindings
    tuttle_get_path_to_install_python(TUTTLE_INSTALL_PYTHON)

    # Install python libs and wrapper in INSTALL_PREFIX/lib/python
    install(TARGETS ${SWIG_MODULE_tuttle_REAL_NAME} 
        DESTINATION ${TUTTLE_INSTALL_PYTHON}/pyTuttle OPTIONAL)
    install(FILES ${CMAKE_BINARY_DIR}/libraries/tuttle/tuttle.py
        DESTINATION ${TUTTLE_INSTALL_PYTHON}/pyTuttle OPTIONAL)
    install(CODE "file(WRITE ${TUTTLE_INSTALL_PYTHON}/pyTuttle/__init__.py)")

  else(SWIG_FOUND)
    message("SWIG not found, will not build python bindings")
  endif(SWIG_FOUND)

  # Build boot unit tests of host and plugins
  add_subdirectory(tests)

endif(TuttleBoost_FOUND)
