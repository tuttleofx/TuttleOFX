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
#include <boost/serialization/serialization.hpp>

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
}

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
	/*
	std::string expatCacheFile( "tuttlePluginCache.xml" );
	// try to read an old cache
	std::ifstream ifs( expatCacheFile.c_str() );
	_pluginCache.readCache( ifs );
	ifs.close();

	_pluginCache.scanPluginFiles();

	/// flush out the current cache
	if( _pluginCache.isDirty() )
	{
		std::ofstream ofs( expatCacheFile.c_str() );
		_pluginCache.writePluginCache( ofs );
		ofs.close();
	}
	*/

//	typedef boost::archive::binary_oarchive OArchive;
//	typedef boost::archive::binary_iarchive IArchive;
//	typedef boost::archive::text_oarchive OArchive;
//	typedef boost::archive::text_iarchive IArchive;
	typedef boost::archive::xml_oarchive OArchive;
	typedef boost::archive::xml_iarchive IArchive;

	std::string cacheFile( "tuttlePluginCacheSerialize.xml" );
	
	try
	{
		std::ifstream ifsb( cacheFile.c_str() );
		if( ifsb.is_open() )
		{
			COUT("Read plugins cache.");
			IArchive iArchive( ifsb );
			iArchive >> BOOST_SERIALIZATION_NVP(_pluginCache);
			ifsb.close();
		}
	}
	catch( std::exception& e )
	{
		COUT("Exception when reading cache file (" << e.what() << ")." );
	}
	

	_pluginCache.scanPluginFiles();

	if( _pluginCache.isDirty() )
	{
		/// flush out the current cache
		COUT("Write plugins cache.");
		std::ofstream ofsb( cacheFile.c_str() );
		OArchive oArchive( ofsb );
		oArchive << BOOST_SERIALIZATION_NVP(_pluginCache);
		ofsb.close();
	}
	
}

}
}
}

