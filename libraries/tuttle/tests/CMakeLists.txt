# Setup CMake to run tests
enable_testing()

find_package(Boost COMPONENTS unit_test_framework)
if(NOT Boost_FOUND)
    message(WARNING "Boost Unit Tests will not be built")
    return()
endif()

include_directories (${Boost_INCLUDE_DIRS})

# Create custom target 'run_tests'
add_custom_target(run_tests ${CMAKE_CTEST_COMMAND} -V)

# Get tests of tuttle host and plugins
file(GLOB_RECURSE TEST_HOST_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(GLOB_RECURSE TEST_PLUGIN_SRC ${PROJECT_SOURCE_DIR}/plugins/*plugin_*.cpp)
set(TEST_SRC ${TEST_HOST_SRC} ${TEST_PLUGIN_SRC})

# Run through each source
foreach(testSrc ${TEST_SRC})

    # Extract the filename without an extension (NAME_WE)
    get_filename_component(testName ${testSrc} NAME_WE)

    # Add define for all tests
    add_definitions(-DBOOST_TEST_DYN_LINK) 
    add_definitions(-DBOOST_TEST_MAIN)

    # Build test
    add_executable(${testName} ${testSrc})
    target_link_libraries(${testName} pthread)
    target_link_libraries(${testName} ${Boost_LIBRARIES})
    target_link_libraries(${testName} tuttleHost)

    # Move testing binary into a testBin directory
    set_target_properties(${testName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/testBin)

    # Add it to test execution
    add_test(NAME ${testName}
             WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/testBin 
             COMMAND ${PROJECT_SOURCE_DIR}/testBin/${testName} )

endforeach(testSrc)
