#include "Core.hpp"

#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>

#include <tuttle/host/memory/MemoryPool.hpp>
#include <tuttle/host/memory/MemoryCache.hpp>

#include <boost/filesystem/operations.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

#ifndef TUTTLE_HOST_WITHOUT_PYTHON
	#include <boost/python.hpp>
#endif

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // memset

namespace tuttle {
namespace host {

namespace {
memory::MemoryPool pool;
memory::MemoryCache cache;
}

Core::Core()
	: _imageEffectPluginCache( _host )
	, _memoryPool( pool )
	, _memoryCache( cache )
{
#ifndef TUTTLE_HOST_WITHOUT_PYTHON
	Py_Initialize( );
#endif
	_pluginCache.setCacheVersion( "tuttleV1" );

	// register the image effect cache with the global plugin cache
	_pluginCache.registerAPICache( _imageEffectPluginCache );

	_memoryPool.updateMemoryAuthorizedWithRAM();
	//	preload();
}

Core::~Core()
{}

void Core::preload()
{
#ifndef WINDOWS
	//	typedef boost::archive::binary_oarchive OArchive;
	//	typedef boost::archive::binary_iarchive IArchive;
	//	typedef boost::archive::text_oarchive OArchive;
	//	typedef boost::archive::text_iarchive IArchive;
	typedef boost::archive::xml_oarchive OArchive;
	typedef boost::archive::xml_iarchive IArchive;

	const std::string cacheFile( "tuttlePluginCacheSerialize.xml" );

	try
	{
		std::ifstream ifsb( cacheFile.c_str() );
		if( ifsb.is_open() )
		{
			TUTTLE_COUT_DEBUG( "Read plugins cache." );
			IArchive iArchive( ifsb );
			iArchive >> BOOST_SERIALIZATION_NVP( _pluginCache );
			ifsb.close();
		}
	}
	catch( std::exception& e )
	{
		TUTTLE_COUT_ERROR( "Exception when reading cache file (" << e.what() << ")." );
	}
#endif
	_pluginCache.scanPluginFiles();
#ifndef WINDOWS
	if( _pluginCache.isDirty() )
	{
		/// @todo generate unique name
		const std::string tmpCacheFile( cacheFile + ".writing" );
		/// flush out the current cache
		TUTTLE_TCOUT( "Writing plugins cache." );
		std::ofstream ofsb( tmpCacheFile.c_str() );
		OArchive oArchive( ofsb );
		oArchive << BOOST_SERIALIZATION_NVP( _pluginCache );
		ofsb.close();
		TUTTLE_TCOUT( "End writing." );
		TUTTLE_TCOUT( "Rename file." );
		boost::filesystem::rename( tmpCacheFile, cacheFile );
		TUTTLE_TCOUT( "End rename." );
	}
#endif
}

std::ostream& operator<<( std::ostream& os, const Core& v )
{
	os << "Core {" << std::endl;
	os << v.getImageEffectPluginCache();
	os << "}" << std::endl;
	return os;
}

}
}

