#ifndef _TUTTLE_HOST_PRELOADPLUGINS_HPP_
#define _TUTTLE_HOST_PRELOADPLUGINS_HPP_

#include "Core.hpp"

#include <iostream>

namespace tuttle
{
namespace host
{

struct PreloadPlugins
{
    PreloadPlugins(bool usingCache = false)
    {
        using namespace tuttle::host;
        TUTTLE_LOG_TRACE("[OpenFX] Loading plugins");
        TUTTLE_LOG_TRACE(BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH));
        core().getPluginCache().addDirectoryToPath(BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH));
        core().preload(usingCache);
        TUTTLE_LOG_TRACE(core().getImageEffectPluginCache());
        TUTTLE_LOG_TRACE("[OpenFX] Load of plugins done");
    }

    ~PreloadPlugins()
    {
        // TUTTLE_LOG_TRACE( "PreloadOpenFXPlugins: global teardown" );
    }
};
}
}

#endif
