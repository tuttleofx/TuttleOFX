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
		core().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
		core().preload();
		// TUTTLE_COUT( core().getImageEffectPluginCache() );
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
