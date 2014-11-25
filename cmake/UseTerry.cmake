
# Use terry image library
# include(UseTerry)
include_directories(${PROJECT_SOURCE_DIR}/libraries/terry/src)
# FIXME: there is a openexr subfolder with a file half.hpp including for
# external half.h from ilmbase/openexr.
# So terry depends on openexr 

