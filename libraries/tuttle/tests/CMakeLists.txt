# Setup CMake to run tests
enable_testing()

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories (${Boost_INCLUDE_DIRS})

file(GLOB_RECURSE TEST_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

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
    set_target_properties(${testName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin)

    # Add it to test execution
    add_test(NAME ${testName}
             WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/testBin 
             COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin/${testName} )

endforeach(testSrc)
