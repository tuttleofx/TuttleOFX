# Project Tuttle OFX 
include(CPack)
cmake_minimum_required (VERSION 2.8)
project (TuttleOFX)

# The install rule does not depend on all, i.e. everything will not be built before installing
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

# Add a variable for experimental host/plugins features
if(NOT TUTTLE_EXPERIMENTAL)
  set(TUTTLE_EXPERIMENTAL 0)
endif()
add_definitions(-DTUTTLE_EXPERIMENTAL=${TUTTLE_EXPERIMENTAL})

# Add a variable to set a default path to OFX plugins
add_definitions(-DTUTTLE_OFX_PLUGIN_PATH="${CMAKE_INSTALL_PREFIX}/OFX")

# Add a variable to install dependencies with tuttle when deploy
if(NOT TUTTLE_DEPLOY_DEPENDENCIES)
  set(TUTTLE_DEPLOY_DEPENDENCIES 0)
endif()

# Create an alias for all OFX plugins.
add_custom_target(ofxplugins)

# Include subdirs
set(SEQUENCEPARSER_PYTHON_VERSION ${TUTTLE_PYTHON_VERSION})
add_subdirectory(libraries/sequenceParser)
add_subdirectory(libraries/tuttle)
add_subdirectory(applications)
add_subdirectory(plugins)

# Add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(doxyfile_in ${PROJECT_SOURCE_DIR}/doc/code/Doxyfile)
  set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${doxyfile_in} ${doxyfile} @ONLY)

  FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/doc)
  add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating TuttleHOST documentation with Doxygen" VERBATIM
    )
  install(DIRECTORY "${CMAKE_BINARY_DIR}/share/doc" DESTINATION "${CMAKE_INSTALL_PREFIX}/share" OPTIONAL)

endif(DOXYGEN_FOUND)
