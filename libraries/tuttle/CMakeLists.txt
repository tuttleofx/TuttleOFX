# CMakeLists.txt for Tuttle host library 

# Add custom cmake macros
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# All libraries will be put in INSTALL_PREFIX/lib 
# RPATH of host points INSTALL_PREFIX/lib
# see: http://www.cmake.org/Wiki/CMake_RPATH_handling
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Tuttle source code
include_directories(src)
file(GLOB_RECURSE TUTTLE_HOST_SRC_FILES src/tuttle/host/*.?pp src/tuttle/common/*.?pp)

# Boost dependencies
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF) 
find_package(Boost 1.55.0 
    COMPONENTS regex date_time chrono thread serialization system filesystem atomic log program_options timer)

if (Boost_FOUND) 
  include_directories(${Boost_INCLUDE_DIRS}) 
  
  # OpenFX and Terry rely on boost
  include(UseOfxpp)
  include(UseTerry)
  
  # SequenceParser, as a separate project
  find_package(sequenceParser)
  include_directories(${SEQUENCEPARSER_INCLUDE_DIR})
  
  # Atomic -> FIXME in the code, there is a 
  # redefinition of boost atomic to change
  # libraries/tuttle/src/tuttle/common/atomic.hpp
  add_definitions(-DWITH_BOOST_ATOMIC)
  
  # Target is a shared library
  add_library(tuttleHost SHARED ${TUTTLE_HOST_SRC_FILES}) 
  target_link_libraries(tuttleHost ${Boost_LIBRARIES})
  if(APPLE)
      set_target_properties(tuttleHost 
            PROPERTIES LINK_FLAGS "-framework CoreFoundation -w")
      set_target_properties(tuttleHost 
            PROPERTIES INSTALL_RPATH "@loader_path")
  endif(APPLE)
  if (UNIX AND NOT APPLE)
      target_link_libraries(tuttleHost dl)
      set_target_properties(tuttleHost
            PROPERTIES INSTALL_RPATH "$ORIGIN")
  endif(UNIX AND NOT APPLE)

  # Adds a support library for plugins
  # only compile object files, and use them at link time
  add_library(tuttlePluginLib OBJECT ${OFXPP_FILES} ${SEQUENCEPARSER_FILES}) 
  set_target_properties(tuttlePluginLib
                        PROPERTIES COMPILE_FLAGS "-fPIC")

  # Install tuttle host library in INSTALL_PREFIX/lib/
  install(TARGETS tuttleHost DESTINATION lib/)

  # Install boost library along with tuttle library
  # Need to copy the files pointed by symlinks instead of the symlinks
  foreach(lib ${Boost_LIBRARIES})
     get_filename_component(realpath ${lib} REALPATH) 
     list(APPEND Boost_REALPATH_LIBRARIES ${realpath})
  endforeach()
  install(FILES ${Boost_REALPATH_LIBRARIES}
          DESTINATION lib/
          NAMELINK_SKIP)
  # Is CPack needed ?
  #include(CPack)

  # Python bindings with Swig 
  find_package(SWIG)
  if (SWIG_FOUND)
    
    # Load Modules/UseSWIG.cmake
    include(${SWIG_USE_FILE})
  
    # Find python c libs
    find_package(PythonLibs)
    include_directories(${PYTHON_INCLUDE_DIRS})
  
    # Find numpy headers
    # TODO is numpy is not found, remove source files using it
    find_package(Numpy)
    include_directories(${NUMPY_INCLUDE_DIR})
  
    # Swig to compile c++ -> python
    set(CMAKE_SWIG_FLAGS -c++ -threads)
    set(TUTTLE_PYTHON_BINDING_FILE src/tuttle/host/tuttle.i)
    # FIXME : do we need a python package as before ?
    #list(APPEND options --package pyTuttle)
    #set_source_files_properties(${TUTTLE_PYTHON_BINDING_FILE} PROPERTIES SWIG_FLAGS "${options}") 
    set_source_files_properties(${TUTTLE_PYTHON_BINDING_FILE} PROPERTIES CPLUSPLUS ON) 

    # Create the output dir as it's not done automatically by cmake
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/tuttle/host)
    swig_add_module(tuttle python ${TUTTLE_PYTHON_BINDING_FILE})
    swig_link_libraries(tuttle tuttleHost)
    swig_link_libraries(tuttle ${Boost_LIBRARIES})
    swig_link_libraries(tuttle ${PYTHON_LIBRARIES})
    # Force the library to look in the project lib folder relatively
    if (APPLE)
      set_target_properties(${SWIG_MODULE_tuttle_REAL_NAME} 
            PROPERTIES INSTALL_RPATH "@loader_path/../../lib")
    else(APPLE)
        set_target_properties(${SWIG_MODULE_tuttle_REAL_NAME}
            PROPERTIES INSTALL_RPATH "$ORIGIN/../../lib:$ORIGIN")
    endif(APPLE)

    # Install python libs and wrapper in INSTALL_PREFIX/share/python
    install(TARGETS ${SWIG_MODULE_tuttle_REAL_NAME} 
                  DESTINATION share/python)
    install(FILES ${CMAKE_BINARY_DIR}/libraries/tuttle/tuttle.py 
                  DESTINATION share/python)

  else(SWIG_FOUND)
    message("SWIG not found, will not build python bindings")
  endif(SWIG_FOUND)

endif(Boost_FOUND)
