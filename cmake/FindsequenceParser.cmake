
# TODO : add SEQUENCEPARSER_ROOT_DIR env 
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/libraries/sequenceParser/src)
  message("unable to find sequenceParser library, please get it with git recurse -i")
else(NOT EXISTS ${PROJECT_SOURCE_DIR}/libraries/sequenceParser/src)
  set(SEQUENCEPARSER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/libraries/sequenceParser/src)
  file(GLOB_RECURSE SEQUENCEPARSER_FILES ${PROJECT_SOURCE_DIR}/libraries/sequenceParser/src/*.?pp)
  set(SEQUENCEPARSER_FOUND 1)
endif(NOT EXISTS ${PROJECT_SOURCE_DIR}/libraries/sequenceParser/src)
