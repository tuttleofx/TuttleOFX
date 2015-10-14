
# Boost for the whole Tuttle project

set(Boost_USE_STATIC_LIBS OFF)
add_definitions(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.53.0 
    COMPONENTS date_time chrono serialization system filesystem atomic log program_options timer QUIET)

if (Boost_FOUND) 
  set(TuttleBoost_FOUND 1)
else(Boost_FOUND)
    message("please set BOOST_ROOT environment variable to a proper boost install")
endif(Boost_FOUND)

