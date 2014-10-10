# Look for sequenceParser.
# In this project, sequenceParser is retrieved as a submodule
# with git submodule init
# TODO : add SEQUENCEPARSER_ROOT_DIR env to override current installation


# Look for the submodule
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/libraries/sequenceParser/src)
    message("unable to find sequenceParser library, please get it with git")
else(NOT EXISTS ${PROJECT_SOURCE_DIR}/libraries/sequenceParser/src)
    set(SEQUENCEPARSER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libraries/sequenceParser/src)
    file(GLOB_RECURSE SEQUENCEPARSER_FILES ${PROJECT_SOURCE_DIR}/libraries/sequenceParser/src/*.?pp)
    set(SEQUENCEPARSER_FOUND 1)
    set(SEQUENCEPARSER_LIBRARIES sequenceParser)

endif(NOT EXISTS ${PROJECT_SOURCE_DIR}/libraries/sequenceParser/src)
      

