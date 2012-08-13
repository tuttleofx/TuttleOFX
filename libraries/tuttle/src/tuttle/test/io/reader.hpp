#include <iostream>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_CASE( process_reader )
{
	TUTTLE_COUT( "******** PROCESS READER " << pluginName << " ********" );
	Graph g;

	TUTTLE_COUT( "--> PLUGINS CREATION " );
	Graph::Node& read = g.createNode( pluginName );

	TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );

	std::string tuttleOFXData = "";
	if( const char* env_ls_options = std::getenv("TUTTLE_TEST_DATA") )
	{
		tuttleOFXData = env_ls_options;
	}
	
	std::string pluginFilename = tuttleOFXData + "image/" + filename;
	read.getParam( "filename" ).setValue( pluginFilename );
	
	TUTTLE_COUT( "--> GRAPH PROCESSING" );
	boost::posix_time::ptime t1a(boost::posix_time::microsec_clock::local_time());
	memory::MemoryCache outputCache;
	g.compute( outputCache, read );
	boost::posix_time::ptime t2a(boost::posix_time::microsec_clock::local_time());

	TUTTLE_COUT( "Process took: " << t2a - t1a );

	std::cout << outputCache << std::endl;

	memory::CACHE_ELEMENT imgRes = outputCache.get( read.getName(), 0 );

	TUTTLE_TCOUT_VAR( imgRes->getROD() );
	BOOST_CHECK_EQUAL( imgRes->getROD().x1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getROD().y1, 0 );
	BOOST_CHECK_NE( imgRes->getROD().x2, 0 );
	BOOST_CHECK_NE( imgRes->getROD().y2, 0 );

	TUTTLE_TCOUT_VAR( imgRes->getBounds() );
	BOOST_CHECK_EQUAL( imgRes->getBounds().x1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getBounds().y1, 0 );
	BOOST_CHECK_NE( imgRes->getBounds().x2, 0 );
	BOOST_CHECK_NE( imgRes->getBounds().y2, 0 );
}

BOOST_AUTO_TEST_CASE( process_nofile )
{
	TUTTLE_COUT( "******** PROCESS READER " << pluginName << " NO FILE ********" );
	Graph g;

	TUTTLE_COUT( "--> PLUGINS CREATION" );
	Graph::Node& read = g.createNode( pluginName );

	TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );
	std::string filename = "data/no-such-file";
	read.getParam( "filename" ).setValue( filename );

	TUTTLE_COUT( "---> GRAPH PROCESSING" );
	BOOST_REQUIRE_THROW( g.compute( read ), boost::exception );
}
