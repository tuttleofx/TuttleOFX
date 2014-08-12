# Project Tuttle OFX 
include(CPack)
cmake_minimum_required (VERSION 2.8)
project (TuttleOFX)

add_subdirectory(libraries/tuttle)
add_subdirectory(plugins)
add_subdirectory(applications)
