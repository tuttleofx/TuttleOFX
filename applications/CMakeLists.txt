# sam
add_subdirectory(sam)

# scripts
add_subdirectory(script)

# Edit python scripts if the host is built with python3
if(${TUTTLE_PYTHON_VERSION} VERSION_GREATER "3")
    set(BASH_PYTHON2 "#!/usr/bin/env python")
    set(BASH_PYTHON3 "#!/usr/bin/env python3")

    file(GLOB_RECURSE PYTHON_APPS sam/*.py script/*.py)
    foreach(PYTHON_APP ${PYTHON_APPS})
        file(READ "${PYTHON_APP}" APP_CONTENT_PYTHON2)
        string(REGEX REPLACE ${BASH_PYTHON2} ${BASH_PYTHON3} APP_CONTENT_PYTHON3  "${APP_CONTENT_PYTHON2}" )
        file(WRITE "${PYTHON_APP}" "${APP_CONTENT_PYTHON3}")
    endforeach()
endif()
