# libtuttle host

# Add custom cmake scripts
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Tuttle source code
include_directories(src)
file(GLOB_RECURSE TUTTLE_HOST_SRC_FILES src/tuttle/host/*.?pp src/tuttle/common/*.?pp)

# Boost dependencies
# we want the static multithreaded libs
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF) 
find_package(Boost 1.55.0 COMPONENTS thread serialization filesystem atomic log program_options timer)

# TODO if boost found
if (Boost_FOUND) 
  include_directories(${Boost_INCLUDE_DIRS}) 
  
  # OpenFX
  include(UseOfxpp)
  
  # Terry
  include(UseTerry)
  
  # SequenceParser
  # TODO : find_package(sequenceParser)
  include_directories(../sequenceParser/src/)
  
  # Atomic -> FIXME in the code, there is a 
  # redefinition of boost atomic to change
  # libraries/tuttle/src/tuttle/common/atomic.hpp
  add_definitions(-DWITH_BOOST_ATOMIC)
  
  # Static library
  add_library(tuttleHost SHARED ${TUTTLE_HOST_SRC_FILES} ) 
  target_link_libraries(tuttleHost ${Boost_LIBRARIES})
  
  # Python bindings with Swig 
  find_package(SWIG)
  if (SWIG_FOUND)
    
    # Load Modules/UseSWIG.cmake
    include(${SWIG_USE_FILE})
  
    # Find python c libs
    find_package(PythonLibs)
    include_directories(${PYTHON_INCLUDE_DIRS})
  
    # Find numpy headers
    find_package(Numpy)
  
    include_directories(${NUMPY_INCLUDE_DIR})
  
    # Swig to compile c++ -> python
    set(CMAKE_SWIG_FLAGS -c++ -threads)
    set(TUTTLE_PYTHON_BINDING_FILE src/tuttle/host/tuttle.i)
    set_source_files_properties(${TUTTLE_PYTHON_BINDING_FILE} PROPERTIES CPLUSPLUS ON) 
  
    # Fix as the module does not create the output dir
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/tuttle/host)
    swig_add_module(tuttle python ${TUTTLE_PYTHON_BINDING_FILE})
    swig_link_libraries(tuttle tuttleHost)
    swig_link_libraries(tuttle ${Boost_LIBRARIES})
  
    # TODO check 
    # http://www.cmake.org/Wiki/CMake_RPATH_handling
  
  else(SWIG_FOUND)
    message("SWIG not found, will not build python bindings")
  endif(SWIG_FOUND)


endif(Boost_FOUND)
