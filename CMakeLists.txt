# Project Tuttle OFX 
include(CPack)
cmake_minimum_required (VERSION 2.8)
project (TuttleOFX)


# Add a variable for production
set(TUTTLE_PRODUCTION TRUE CACHE BOOL "Fully optimized for production usage.")
if(TUTTLE_PRODUCTION)
  add_definitions(-DTUTTLE_PRODUCTION)
endif()

# The install rule does not depend on all, i.e. everything will not be built before installing
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

# Create an alias for all OFX plugins.
add_custom_target(ofxplugins)

# Include subdirs
add_subdirectory(libraries/sequenceParser)
add_subdirectory(libraries/tuttle)
add_subdirectory(plugins)

# Add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/code/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doc/code/Doxyfile @ONLY)
  FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/share/tuttle/doc)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doc/code/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating TuttleHOST documentation with Doxygen" VERBATIM
    )
  # install(DIRECTORY "${CMAKE_BINARY_DIR}/share/tuttle/doc" DESTINATION "${CMAKE_INSTALL_PREFIX}/share/tuttle/doc")

endif(DOXYGEN_FOUND)
