#ifndef _TUTTLE_HOST_PRELOADPLUGINS_HPP_
#define _TUTTLE_HOST_PRELOADPLUGINS_HPP_

#include "Core.hpp"

#include <iostream>

namespace tuttle {
namespace host {

struct PreloadPlugins
{
    PreloadPlugins()
	{
		using namespace tuttle::host;
		// std::cout << "PreloadOpenFXPlugins: global setup\n";
		TUTTLE_COUT( "-------- LOADING OPENFX PLUGINS --------" );
		Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
		Core::instance().preload();
		// TUTTLE_COUT( Core::instance().getImageEffectPluginCache() );
		TUTTLE_COUT( "------ LOADING OPENFX PLUGINS DONE -----" );
	}
    ~PreloadPlugins()
	{
		// std::cout << "PreloadOpenFXPlugins: global teardown\n";
	}
};

}
}

#endif
