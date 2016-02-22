# Boost for the whole Tuttle project

set(Boost_USE_STATIC_LIBS OFF)
add_definitions(-DBOOST_LOG_DYN_LINK)

# Get boost libraries for tuttleCommon
find_package(Boost 1.53.0
    COMPONENTS log filesystem
    QUIET
)
set(TuttleCommonBoost_LIBRARIES ${Boost_LIBRARIES})

# Get boost libraries for tuttleHost
find_package(Boost 1.53.0
    COMPONENTS date_time chrono serialization system filesystem atomic log timer
    QUIET
)
set(TuttleHostBoost_LIBRARIES ${Boost_LIBRARIES})

if (Boost_FOUND) 
  set(TuttleBoost_FOUND 1)
  # Override some boost gil headers
  include_directories(${PROJECT_SOURCE_DIR}/libraries/boostHack)
else(Boost_FOUND)
    message("please set BOOST_ROOT environment variable to a proper boost install")
endif(Boost_FOUND)

