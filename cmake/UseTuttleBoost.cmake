
# Boost for the whole Tuttle project

set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost 1.53.0 
    COMPONENTS regex date_time chrono thread serialization system filesystem atomic log program_options timer QUIET)

if (Boost_FOUND) 
  set(TuttleBoost_FOUND 1)
else(Boost_FOUND)
    message("please set BOOST_ROOT environment variable to a proper boost install")
endif(Boost_FOUND)
