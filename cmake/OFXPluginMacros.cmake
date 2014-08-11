# Custom macros, UseOfxpp UseTerry
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Return OFX architecture
function(tuttle_ofx_architecture ARCH)
    # use ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}
    # message("ARCH=${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
    # TODO test all systems
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
      set(${ARCH} "MacOS" PARENT_SCOPE)
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
      # Assume x86_64
      set(${ARCH} "Linux-x86-64" PARENT_SCOPE)
    endif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
endfunction(tuttle_ofx_architecture) 

# Use this function to create a new plugin target
# The first argument is the plugin name
# the second argument is a list of files to compile
function(tuttle_ofx_plugin_target PLUGIN_NAME)
   
    message("======= ${PLUGIN_NAME} plugin  =======")

    # FIXME: Why ? what are we including here ? common ??
    include_directories(${PROJECT_SOURCE_DIR}/libraries/tuttle/src/)
    
    # Use Boost as openfxHack is dependent on it
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED OFF) 
    find_package(Boost 1.53.0
        COMPONENTS regex date_time chrono thread serialization system filesystem atomic log program_options timer)

    # TODO if boost found else error message
    include_directories(${Boost_INCLUDE_DIRS}) 

    # If no plugin sources is provided, recursively find file below src folder
    set(PLUGIN_SOURCES ${ARGV1}) 
    if (NOT PLUGIN_SOURCES)
        file(GLOB_RECURSE PLUGIN_SOURCES src/*.?pp src/*.tcc)
        include_directories(src)
    endif ()

    # OpenFX and Terry are used by default
    include(UseOfxpp)
    include(UseTerry)
    
    # Plugin target is a shared library
    add_library(${PLUGIN_NAME} MODULE ${PLUGIN_SOURCES} $<TARGET_OBJECTS:tuttlePluginLib>)
    target_link_libraries(${PLUGIN_NAME} ${Boost_LIBRARIES})
    set_target_properties(${PLUGIN_NAME} PROPERTIES SUFFIX .ofx)
    set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
    # tuttlePluginLib depends on OpenGL ......
    if(APPLE)
        find_package(OpenGL)
        target_link_libraries(${PLUGIN_NAME} ${OPENGL_LIBRARIES})
        set_target_properties(${PLUGIN_NAME} PROPERTIES LINK_FLAGS "-framework CoreFoundation -w")
        set_target_properties(${PLUGIN_NAME} 
            PROPERTIES INSTALL_RPATH "@loader_path/../../../../lib")
    else(APPLE)
        set_target_properties(${PLUGIN_NAME}
            PROPERTIES INSTALL_RPATH "$ORIGIN/../../../../lib:$ORIGIN")
    endif(APPLE)
 
    # Install OFX plugin as specified in
    # http://openfx.sourceforge.net/Documentation/1.3/Reference/ch02s02.html
    set(OFX_PLUGIN_ROOT ${CMAKE_INSTALL_PREFIX}/OFX/${PLUGIN_NAME}.ofx.bundle/Contents)
    tuttle_ofx_architecture(OFX_ARCH)
    # TODO : should install Resources only if target is installed
    install(DIRECTORY Resources DESTINATION ${OFX_PLUGIN_ROOT})
    install(TARGETS ${PLUGIN_NAME} 
        DESTINATION ${OFX_PLUGIN_ROOT}/${OFX_ARCH} OPTIONAL)

    # Set RPATH for boost 
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    #message(STATUS "sources: ${PLUGIN_SOURCES}")

endfunction(tuttle_ofx_plugin_target)


# Search in current variables the ones starting with PACKAGE_NAME
# and having INCLUDE/LIBRARY_PATH/LIBRARIES in their name
# Returns the results in INCLUDES LIBRARYPATH LIBRARIES
function(tuttle_find_package_paths PACKAGE_NAME INCLUDES LIBRARYPATH LIBRARIES)
    
    get_cmake_property(_vars VARIABLES)
    # Handle Package_XX and PACKAGE_XX variable prefixes
    string(TOUPPER ${PACKAGE_NAME} PACKAGE_UNAME)
    string (REGEX MATCHALL 
        "(^|;)(${PACKAGE_NAME}|${PACKAGE_UNAME})[A-Za-z0-9_]*INCLUDE[A-Za-z0-9_]*" incvar "${_vars}")
    set (${INCLUDES} ${incvar} PARENT_SCOPE)
    string (REGEX MATCHALL 
        "(^|;)(${PACKAGE_NAME}|${PACKAGE_UNAME})[A-Za-z0-9_]*LIBRARY_PATH[A-Za-z0-9_]*" libvar "${_vars}")
    set (${LIBRARYPATH} ${libvar} PARENT_SCOPE)
    string (REGEX MATCHALL 
        "(^|;)(${PACKAGE_NAME}|${PACKAGE_UNAME})[A-Za-z0-9_]*LIBRARIES[A-Za-z0-9_]*" libvar "${_vars}")
    set (${LIBRARIES} ${libvar} PARENT_SCOPE)

endfunction(tuttle_find_package_paths)

# Add a package/library dependency to a plugin
function(tuttle_ofx_plugin_add_library PLUGIN_TARGET PACKAGE_NAME)
    message("Looking for package ${PACKAGE_NAME}")

    find_package(${PACKAGE_NAME} ${ARGN})
    
    # Test both lower and upper case FOUND variable ie.
    # MyLib_FOUND and MYLIB_FOUND 
    # as it seems that there is no norm
    string(TOUPPER ${PACKAGE_NAME} PACKAGE_UNAME)
    if(${PACKAGE_UNAME}_FOUND OR ${PACKAGE_NAME}_FOUND)
        message("found ${PACKAGE_NAME}")
    
        tuttle_find_package_paths(${PACKAGE_NAME} incvars libpaths libvars)

        foreach(var ${incvars})
            include_directories(${${var}})
            message("include: ${${var}}")
        endforeach()
        foreach(var ${libpaths})
            link_directories(${${var}})
            message("lib path: ${${var}}")
        endforeach()
        foreach(var ${libvars})
            target_link_libraries(${PLUGIN_TARGET} ${${var}})
            message("library: ${${var}}")
        endforeach()
    else(${PACKAGE_UNAME}_FOUND OR ${PACKAGE_NAME}_FOUND)
        message("package ${PACKAGE_NAME} not found, plugin ${PLUGIN_TARGET} will not be built")
        # Removes the target from the build and install if a lib is missing
        set_target_properties(${PLUGIN_TARGET} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
    endif(${PACKAGE_UNAME}_FOUND OR ${PACKAGE_NAME}_FOUND)

endfunction(tuttle_ofx_plugin_add_library) 

# Add a list of packages to a plugin
function(tuttle_ofx_plugin_add_libraries PLUGIN_TARGET PLUGIN_LIBRARIES)
  foreach(lib ${PLUGIN_LIBRARIES})
    tuttle_ofx_plugin_add_library(${PLUGIN_TARGET} ${lib})
  endforeach()
endfunction(tuttle_ofx_plugin_add_libraries)
