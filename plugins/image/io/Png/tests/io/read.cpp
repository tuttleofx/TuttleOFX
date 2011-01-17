#include <iostream>
#define BOOST_TEST_MODULE plugin_Png
#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>
#include <tuttle/host/InputBufferNode.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 * Simple functional test, to read and write an image.
 */

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( plugin_Png_reader )

BOOST_AUTO_TEST_CASE( process_reader )
{
	using namespace tuttle::host;
	TUTTLE_TCOUT( "__________________________________________________0" );
	Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
	Core::instance().preload();

//	TUTTLE_TCOUT( Core::instance().getImageEffectPluginCache() );

	Graph g;

	TUTTLE_TCOUT( "__________________________________________________1" );
	Graph::Node& read = g.createNode( "fr.tuttle.pngreader" );

	TUTTLE_TCOUT( "__________________________________________________3" );

	read.getParam( "filename" ).set( "data/input.png" );

	TUTTLE_TCOUT( "__________________________________________________4" );
//	g.connect( read, write );

	TUTTLE_TCOUT( "__________________________________________________5" );
	boost::posix_time::ptime t1a(boost::posix_time::microsec_clock::local_time());
	memory::MemoryCache res0 = g.compute( read, 0 );
	boost::posix_time::ptime t2a(boost::posix_time::microsec_clock::local_time());

	TUTTLE_COUT( "Process took: " << t2a - t1a );

	std::cout << res0 << std::endl;

	memory::CACHE_ELEMENT imgRes = res0.get( read.getName(), 0 );

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
	using namespace tuttle::host;
	TUTTLE_TCOUT( "__________________________________________________0" );
	Graph g;

	TUTTLE_TCOUT( "__________________________________________________1" );
	Graph::Node& read = g.createNode( "fr.tuttle.pngreader" );

	TUTTLE_TCOUT( "__________________________________________________3" );
	read.getParam( "filename" ).set( "data/no-such-file.png" );

	TUTTLE_TCOUT( "__________________________________________________4" );
//	g.connect( read, write );

	TUTTLE_TCOUT( "__________________________________________________5" );
	BOOST_REQUIRE_THROW( g.compute( read, 0 ), boost::exception );
}

BOOST_AUTO_TEST_SUITE_END()

