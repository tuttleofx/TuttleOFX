#include "Core.hpp"

#include <tuttle/host/core/memory/MemoryPool.hpp>
#include <tuttle/host/core/memory/MemoryCache.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // memset

namespace tuttle {
namespace host {
namespace core {

namespace {

MemoryPool pool;
MemoryCache cache;

}  // namespace

Core::Core()
	: _imageEffectPluginCache( _host ),
	_memoryPool( pool ),
	_memoryCache( cache )
{
	_pluginCache.setCacheVersion( "tuttleV1" );

	// register the image effect cache with the global plugin cache
	_pluginCache.registerAPICache( _imageEffectPluginCache );

//	preload();
}

Core::~Core()
{}

void Core::preload()
{
	// try to read an old cache
	std::ifstream ifs( "tuttlePluginCache.xml" );

	_pluginCache.readCache( ifs );
	_pluginCache.scanPluginFiles();
	ifs.close();

	/// flush out the current cache
	std::ofstream of( "tuttlePluginCache.xml" );
	_pluginCache.writePluginCache( of );
	of.close();
}

}
}
}

