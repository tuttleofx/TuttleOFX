#include "Core.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // memset

namespace tuttle {
	namespace host {
		namespace core {

			void Core::preload( )
			{
				tuttle::host::ofx::PluginCache::getPluginCache( )->setCacheVersion( "tuttleV1" );

				// register the image effect cache with the global plugin cache
				tuttle::host::ofx::PluginCache::getPluginCache( )->registerAPICache( _imageEffectPluginCache );

				// try to read an old cache
				std::ifstream ifs( "tuttlePluginCache.xml" );
				tuttle::host::ofx::PluginCache::getPluginCache( )->readCache( ifs );
				tuttle::host::ofx::PluginCache::getPluginCache( )->scanPluginFiles( );
				ifs.close( );

				/// flush out the current cache
				std::ofstream of( "tuttlePluginCache.xml" );
				tuttle::host::ofx::PluginCache::getPluginCache( )->writePluginCache( of );
				of.close( );
			}

		}
	}
}
