## sam ls

# Load project cmake macros
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(TuttleMacros)

add_tuttle_executable(sam main.cpp)
tuttle_executable_add_library(sam sequenceParser)

# install extra shell wrappers
install(FILES sam-ll sam-lls sam-lss sam-viewer 
    PERMISSIONS WORLD_EXECUTE WORLD_READ OWNER_EXECUTE OWNER_WRITE OWNER_READ
        DESTINATION bin)
