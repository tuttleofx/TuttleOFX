# Project Tuttle OFX 
include(CPack)
cmake_minimum_required (VERSION 2.8)
project (TuttleOFX)

# Add a variable for production or development
set(TUTTLE_PRODUCTION TRUE CACHE BOOL "Fully optimized for production usage.")
if(TUTTLE_PRODUCTION)
  add_definitions(-DTUTTLE_PRODUCTION)
endif()
add_subdirectory(libraries/tuttle)
add_subdirectory(plugins)
add_subdirectory(applications)
