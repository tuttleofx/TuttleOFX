# Macros used to create an openfx plugin with tuttle
include(TuttleMacros)

# Declare the plugin
tuttle_ofx_plugin_target(Text)
tuttle_ofx_plugin_add_library(Text Freetype)

#tuttle_ofx_plugin_add_library(Text PythonLibs)
# FIXME : add_library(PythonLibs) does not work because
# FindPythonLibs defines variables prefixed with PYTHON_ instead of
# PYTHONLIBS_ . The function tuttle_ofx_plugin_add_library assumes 
# the prefix is the name of the plugin 

# So we use the classic way of including and linking 
#find_package(PythonLibs)
#include_directories(${PYTHON_INCLUDE_DIRS})
#target_link_libraries(Text ${PYTHON_LIBRARIES})

# Moreover, this plugin needs boost python
#find_package(Boost 1.55.0 COMPONENTS python QUIET)
#target_link_libraries(Text ${Boost_LIBRARIES})
#tuttle_install_shared_libs(${Boost_LIBRARIES})

# On linux
target_link_libraries(Text fontconfig)

# FIXME should get rid of this include
include_directories(${PROJECT_SOURCE_DIR}/libraries/boostHack)
