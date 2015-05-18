## sam

# sam-ls
add_custom_target(sam-ls SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam-ls.py")
add_dependencies(sam-ls _sequenceparser-py)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam-ls.py"
    DESTINATION bin/
)

# sam-mv
add_custom_target(sam-mv SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam-mv.py")
add_dependencies(sam-mv _sequenceparser-py)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam-mv.py"
    DESTINATION bin/
)

