## sam

# common
find_package(PythonLibs)
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/common"
    FILE_PERMISSIONS WORLD_READ GROUP_WRITE GROUP_READ OWNER_WRITE OWNER_READ
    DESTINATION lib/python${PYTHONLIBS_VERSION_STRING}/site-packages/
)
install(CODE "file(WRITE ${CMAKE_INSTALL_PREFIX}/lib/python${PYTHONLIBS_VERSION_STRING}/site-packages/common/__init__.py)")

# sam
add_custom_target(sam SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam.py")
add_dependencies(sam sam-ls sam-mv sam-cp sam-rm)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam.py"
    PERMISSIONS WORLD_EXECUTE WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin/
)

# sam-ls
add_custom_target(sam-ls SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam-ls.py")
add_dependencies(sam-ls _sequenceparser-py)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam-ls.py"
    PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin/
)

# sam-mv
add_custom_target(sam-mv SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam-mv.py")
add_dependencies(sam-mv _sequenceparser-py)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam-mv.py"
    PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin/
)

# sam-cp
add_custom_target(sam-cp SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam-cp.py")
add_dependencies(sam-cp _sequenceparser-py)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam-cp.py"
    PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin/
)

# sam-rm
add_custom_target(sam-rm SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam-rm.py")
add_dependencies(sam-rm _sequenceparser-py)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam-rm.py"
    PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION bin/
)
