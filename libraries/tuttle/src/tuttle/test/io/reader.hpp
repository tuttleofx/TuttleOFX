#include <iostream>

#include <boost/filesystem/path.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;
namespace bfs = boost::filesystem;

BOOST_AUTO_TEST_CASE( process_reader )
{
	TUTTLE_LOG_INFO( "******** PROCESS READER " << pluginName << " ********" );
	Graph g;

	TUTTLE_LOG_INFO( "--> PLUGINS CREATION " << pluginName );
	Graph::Node& read = g.createNode( pluginName );

	TUTTLE_LOG_INFO( "--> PLUGINS CONFIGURATION" );

	std::string tuttleOFXData = "TuttleOFX-data";
	if( const char* env_test_data = std::getenv("TUTTLE_TEST_DATA") )
	{
		tuttleOFXData = env_test_data;
	}
	
	const std::string pluginFilename = ( bfs::path(tuttleOFXData) / "image" / filename ).string();
	read.getParam( "filename" ).setValue( pluginFilename );
	
	TUTTLE_LOG_INFO( "--> GRAPH PROCESSING" );
	boost::posix_time::ptime t1a(boost::posix_time::microsec_clock::local_time());
	memory::MemoryCache outputCache;
	g.compute( outputCache, read );
	boost::posix_time::ptime t2a(boost::posix_time::microsec_clock::local_time());

	TUTTLE_LOG_INFO( "Process took: " << t2a - t1a );
	TUTTLE_LOG_INFO( outputCache );

	memory::CACHE_ELEMENT imgRes = outputCache.get( read.getName(), 0 );

	TUTTLE_LOG_VAR( TUTTLE_INFO, imgRes->getROD() );
	BOOST_CHECK_EQUAL( imgRes->getROD().x1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getROD().y1, 0 );
	BOOST_CHECK_NE( imgRes->getROD().x2, 0 );
	BOOST_CHECK_NE( imgRes->getROD().y2, 0 );

	TUTTLE_LOG_VAR( TUTTLE_INFO, imgRes->getBounds() );
	BOOST_CHECK_EQUAL( imgRes->getBounds().x1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getBounds().y1, 0 );
	BOOST_CHECK_NE( imgRes->getBounds().x2, 0 );
	BOOST_CHECK_NE( imgRes->getBounds().y2, 0 );
}

BOOST_AUTO_TEST_CASE( process_nofile )
{
	TUTTLE_LOG_INFO( "******** PROCESS READER " << pluginName << " NO FILE ********" );
	Graph g;

	TUTTLE_LOG_INFO( "--> PLUGINS CREATION" );
	Graph::Node& read = g.createNode( pluginName );

	TUTTLE_LOG_INFO( "--> PLUGINS CONFIGURATION" );
	std::string filename = "data/no-such-file";
	read.getParam( "filename" ).setValue( filename );

	TUTTLE_LOG_INFO( "---> GRAPH PROCESSING" );
	BOOST_REQUIRE_THROW( g.compute( read ), boost::exception );
}
