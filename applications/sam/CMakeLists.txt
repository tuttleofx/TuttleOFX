## sam
find_package(SWIG)
if(NOT SWIG_FOUND)
    return()
endif()

set(TUTTLE_INSTALL_SAM_PYTHON "bin/python/sam")

# common
find_package(PythonLibs)
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/common"
    FILE_PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION ${TUTTLE_INSTALL_SAM_PYTHON}
)
install(CODE "file(WRITE ${CMAKE_INSTALL_PREFIX}/${TUTTLE_INSTALL_SAM_PYTHON}/common/__init__.py)")

# sam_ls
add_custom_target(sam_ls SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam_ls.py")
add_dependencies(sam_ls _sequenceparser-py)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam_ls.py"
    PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION ${TUTTLE_INSTALL_SAM_PYTHON}
)

# sam_mv
add_custom_target(sam_mv SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam_mv.py")
add_dependencies(sam_mv _sequenceparser-py)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam_mv.py"
    PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION ${TUTTLE_INSTALL_SAM_PYTHON}
)

# sam_cp
add_custom_target(sam_cp SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam_cp.py")
add_dependencies(sam_cp _sequenceparser-py)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam_cp.py"
    PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION ${TUTTLE_INSTALL_SAM_PYTHON}
)

# sam_rm
add_custom_target(sam_rm SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam_rm.py")
add_dependencies(sam_rm _sequenceparser-py)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam_rm.py"
    PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION ${TUTTLE_INSTALL_SAM_PYTHON}
)

# sam_do
add_custom_target(sam_do SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam_do.py")
add_dependencies(sam_do _sequenceparser-py _tuttle)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam_do.py"
    PERMISSIONS WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION ${TUTTLE_INSTALL_SAM_PYTHON}
)

# sam
add_custom_target(sam SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sam.py")
add_dependencies(sam sam_ls sam_mv sam_cp sam_rm sam_do)

install(
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/sam.py"
    PERMISSIONS WORLD_EXECUTE WORLD_EXECUTE WORLD_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION ${TUTTLE_INSTALL_SAM_PYTHON}
)

if(UNIX)
    # deploy sam symbolic links
    install( CODE "EXECUTE_PROCESS(COMMAND ln -sf python/sam/sam.py ${CMAKE_INSTALL_PREFIX}/bin/sam)" )
    install( CODE "EXECUTE_PROCESS(COMMAND ln -sf python/sam/sam_ls.py ${CMAKE_INSTALL_PREFIX}/bin/sam-ls)" )
    install( CODE "EXECUTE_PROCESS(COMMAND ln -sf python/sam/sam_mv.py ${CMAKE_INSTALL_PREFIX}/bin/sam-mv)" )
    install( CODE "EXECUTE_PROCESS(COMMAND ln -sf python/sam/sam_cp.py ${CMAKE_INSTALL_PREFIX}/bin/sam-cp)" )
    install( CODE "EXECUTE_PROCESS(COMMAND ln -sf python/sam/sam_rm.py ${CMAKE_INSTALL_PREFIX}/bin/sam-rm)" )
    install( CODE "EXECUTE_PROCESS(COMMAND ln -sf python/sam/sam_do.py ${CMAKE_INSTALL_PREFIX}/bin/sam-do)" )
endif(UNIX)
