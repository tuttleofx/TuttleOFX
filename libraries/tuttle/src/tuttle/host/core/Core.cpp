#include "Core.hpp"

#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>

#include <tuttle/host/core/memory/MemoryPool.hpp>
#include <tuttle/host/core/memory/MemoryCache.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // memset

namespace tuttle {
namespace host {
namespace core {

MemoryPool pool;
MemoryCache cache;

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
	ifs.close();

	_pluginCache.scanPluginFiles();

	/// flush out the current cache
	std::ofstream ofs( "tuttlePluginCache.xml" );
	_pluginCache.writePluginCache( ofs );
	ofs.close();

	
//	typedef boost::archive::binary_oarchive OArchive;
//	typedef boost::archive::binary_iarchive IArchive;
//	typedef boost::archive::text_oarchive OArchive;
//	typedef boost::archive::text_iarchive IArchive;
	typedef boost::archive::xml_oarchive OArchive;
	typedef boost::archive::xml_iarchive IArchive;

	std::ofstream ofsb( "tuttlePluginCacheSerialize.xml" );
	OArchive oArchive( ofsb );
	oArchive.register_type( static_cast<ofx::imageEffect::OfxhImageEffectPlugin*>(NULL) );
	boost::serialization::void_cast_register( static_cast<ofx::imageEffect::OfxhImageEffectPlugin*>(NULL),static_cast<ofx::OfxhPlugin*>(NULL) );
	oArchive << BOOST_SERIALIZATION_NVP(_pluginCache);
	ofsb.close();
}

}
}
}

