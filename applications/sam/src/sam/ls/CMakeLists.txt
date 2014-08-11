## sam ls

# Load project cmake macros
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(TuttleExeMacros)

# needs sequenceParser package
find_package(sequenceParser)
if(SEQUENCEPARSER_FOUND)
  include_directories(${SEQUENCEPARSER_INCLUDE_DIR})
  set(SAMLS_FILES main.cpp ${SEQUENCEPARSER_FILES})
  add_tuttle_executable(sam-ls "${SAMLS_FILES}")
endif(SEQUENCEPARSER_FOUND)
