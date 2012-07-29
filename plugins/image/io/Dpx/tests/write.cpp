#include <boost/test/unit_test.hpp>

#include <tuttle/host/Graph.hpp>
#include <tuttle/host/InputBufferNode.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>

/**
 * Simple functional test, to read and write an image.
 */

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE( plugin_Dpx_writer )

BOOST_AUTO_TEST_CASE( process_writer )
{
	TUTTLE_COUT( "******** PROCESS WRITER DPX ********" );
	Graph g;

	TUTTLE_COUT( "-------- PLUGINS CREATION --------" );
	Graph::Node& constant = g.createNode( "tuttle.constant" );
	Graph::Node& writer   = g.createNode( "tuttle.dpxwriter" );

	TUTTLE_COUT( "-------- PLUGINS CONFIGURATION --------" );

	constant.getParam( "width" ).setValue( 500 );
	constant.getParam( "height" ).setValue( 500 );
	writer.getParam( "filename" ).setValue( ".tests/file.dpx" );

	TUTTLE_COUT( "-------- GRAPH CONNECTION --------" );
	g.connect( constant, writer );

	TUTTLE_COUT( "-------- GRAPH PROCESSING --------" );
	boost::posix_time::ptime t1a(boost::posix_time::microsec_clock::local_time());
	memory::MemoryCache outputCache;
	g.compute( outputCache, writer );
	boost::posix_time::ptime t2a(boost::posix_time::microsec_clock::local_time());

	TUTTLE_COUT( "Process took: " << t2a - t1a );

	std::cout << outputCache << std::endl;

	memory::CACHE_ELEMENT imgRes = outputCache.get( writer.getName(), 0 );

	TUTTLE_TCOUT_VAR( imgRes->getROD() );
	BOOST_CHECK_EQUAL( imgRes->getROD().x1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getROD().y1, 0 );
	BOOST_CHECK_NE( imgRes->getROD().x2, 500 );
	BOOST_CHECK_NE( imgRes->getROD().y2, 500 );

	TUTTLE_TCOUT_VAR( imgRes->getBounds() );
	BOOST_CHECK_EQUAL( imgRes->getBounds().x1, 0 );
	BOOST_CHECK_EQUAL( imgRes->getBounds().y1, 0 );
	BOOST_CHECK_NE( imgRes->getBounds().x2, 500 );
	BOOST_CHECK_NE( imgRes->getBounds().y2, 500 );
}

BOOST_AUTO_TEST_CASE( process_unconnected )
{
	TUTTLE_COUT( "******** PROCESS WRITER DPX UNCONNECTED ********" );
	Graph g;

	TUTTLE_COUT( "--> PLUGINS CREATION" );
	Graph::Node& read = g.createNode( "tuttle.dpxwriter" );

	TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );
	read.getParam( "filename" ).setValue( "data/no-such-file.dpx" );

	TUTTLE_COUT( "---> GRAPH PROCESSING" );
	BOOST_REQUIRE_THROW( g.compute( read ), boost::exception );
}

BOOST_AUTO_TEST_SUITE_END()

