# Defines add_tuttle_executable function
# and sets all include and library used to build
# an executable file linked to the host

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Boost libraries
include(UseTuttleBoost)
if(TuttleBoost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS}) 
  include_directories(${PROJECT_SOURCE_DIR}/libraries/tuttle/src) 

  # OpenFX and Terry rely on boost
  include(UseOfxpp)
  include(UseTerry)

  # to include common folder, we include current folder.
  # unfortunatelly common is prefixed in all include<> 
  include_directories(${PROJECT_SOURCE_DIR}/applications/sam/src)

endif(TuttleBoost_FOUND)

function(add_tuttle_executable TARGET SOURCES)

  if(TuttleBoost_FOUND)
      message("=== Executable ${TARGET} ===")
      add_executable(${TARGET} ${SOURCES} $<TARGET_OBJECTS:tuttleCommon>)

      # Link with libraries
      target_link_libraries(${TARGET} ${Boost_LIBRARIES} dl)

      # RPATH
      if (APPLE)
          set_target_properties(${TARGET} 
            PROPERTIES INSTALL_RPATH "@loader_path/../lib")
      else(APPLE)
          set_target_properties(${TARGET}
            PROPERTIES INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN")
      endif(APPLE)

      # Install
      install(TARGETS ${TARGET} 
                      DESTINATION bin)
    endif(TuttleBoost_FOUND)
endfunction(add_tuttle_executable)
