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
		TUTTLE_LOG_TRACE( "-------- LOADING OPENFX PLUGINS --------" );
		core().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
		core().preload();
		// TUTTLE_LOG_TRACE( core().getImageEffectPluginCache() );
		TUTTLE_LOG_TRACE( "------ LOADING OPENFX PLUGINS DONE -----" );
	}
	~PreloadPlugins()
	{
		// TUTTLE_LOG_TRACE( "PreloadOpenFXPlugins: global teardown" );
	}
};

}
}

#endif
