
# Boost for the whole Tuttle project

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF) 
find_package(Boost 1.55.0 
    COMPONENTS regex date_time chrono thread serialization system filesystem atomic log program_options timer)

if (Boost_FOUND) 
  set(TuttleBoost_FOUND 1)
endif(Boost_FOUND)
