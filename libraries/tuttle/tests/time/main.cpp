#include <tuttle/host/Graph.hpp>

#include <iostream>

#define BOOST_TEST_MODULE tuttle_time
#include <boost/test/unit_test.hpp>
//#include <boost/test/execution_monitor.hpp>
//#include <boost/test/exception_safety.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE( time_tests_suite01 )

BOOST_AUTO_TEST_CASE( preload )
{
	TUTTLE_COUT( "-------- LOADING OPENFX PLUGINS --------" );
	Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
	Core::instance().preload();
//	TUTTLE_TCOUT( Core::instance().getImageEffectPluginCache() );
	TUTTLE_COUT( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( time_shift_offset_null )
{
	TUTTLE_COUT( "******** PROCESS TIMESHIFT ********" );
	try
	{
		TUTTLE_COUT( "--> PLUGINS CREATION" );

		Graph g;
		Graph::Node& read1 = g.createNode( "tuttle.jpegreader" );
		//Graph::Node& invert1 = g.createNode( "tuttle.invert" );
		Graph::Node& timeshift1 = g.createNode( "tuttle.timeshift" );
		Graph::Node& write1 = g.createNode( "tuttle.jpegwriter" );

		TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );
		read1.getParam( "filename" ).setValue( "/datas/TuttleOFX-data/images/MARS@.JPG" );
		timeshift1.getParam("offset").setValue( 0 );
		write1.getParam( "filename" ).setValue( ".tests/output_####.jpg" );

		TUTTLE_COUT( "-------- GRAPH CONNECTION --------" );
		g.connect( read1, timeshift1 );
		g.connect( timeshift1, write1 );

		TUTTLE_COUT( "-------- GRAPH PROCESSING --------" );
		std::list<std::string> outputs;
		outputs.push_back( write1.getName() );
		// computing at time 3 with an offset of -2, it require the frame 1
		g.compute( outputs, ComputeOptions( 1 ) );
	}
	catch(... )
	{
		std::cerr << boost::current_exception_diagnostic_information() << std::endl;
		BOOST_FAIL( "Exception" );
	}
}


BOOST_AUTO_TEST_CASE( time_shift_positive_offset )
{
	TUTTLE_COUT( "******** PROCESS TIMESHIFT ********" );
	try
	{
		TUTTLE_COUT( "--> PLUGINS CREATION" );

		Graph g;
		Graph::Node& read1 = g.createNode( "tuttle.jpegreader" );
		//Graph::Node& invert1 = g.createNode( "tuttle.invert" );
		Graph::Node& timeshift1 = g.createNode( "tuttle.timeshift" );
		Graph::Node& write1 = g.createNode( "tuttle.jpegwriter" );

		TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );
		read1.getParam( "filename" ).setValue( "/datas/TuttleOFX-data/images/MARS@.JPG" );
		timeshift1.getParam("offset").setValue( 1 );
		write1.getParam( "filename" ).setValue( ".tests/output_####.jpg" );

		TUTTLE_COUT( "-------- GRAPH CONNECTION --------" );
		g.connect( read1, timeshift1 );
		g.connect( timeshift1, write1 );

		TUTTLE_COUT( "-------- GRAPH PROCESSING --------" );
		std::list<std::string> outputs;
		outputs.push_back( write1.getName() );
		// computing at time 3 with an offset of -2, it require the frame 1
		g.compute( outputs, ComputeOptions( 1 ) );
	}
	catch(... )
	{
		std::cerr << boost::current_exception_diagnostic_information() << std::endl;
		BOOST_FAIL( "Exception" );
	}
}

BOOST_AUTO_TEST_CASE( time_shift_negative_offset )
{
	TUTTLE_COUT( "******** PROCESS TIMESHIFT ********" );
	try
	{
		TUTTLE_COUT( "--> PLUGINS CREATION" );

		Graph g;
		Graph::Node& read1 = g.createNode( "tuttle.jpegreader" );
		//Graph::Node& invert1 = g.createNode( "tuttle.invert" );
		Graph::Node& timeshift1 = g.createNode( "tuttle.timeshift" );
		Graph::Node& write1 = g.createNode( "tuttle.jpegwriter" );

		TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );
		read1.getParam( "filename" ).setValue( "/datas/TuttleOFX-data/images/MARS@.JPG" );
		timeshift1.getParam("offset").setValue( -3 );
		write1.getParam( "filename" ).setValue( ".tests/output_####.jpg" );

		TUTTLE_COUT( "-------- GRAPH CONNECTION --------" );
		g.connect( read1, timeshift1 );
		g.connect( timeshift1, write1 );

		TUTTLE_COUT( "-------- GRAPH PROCESSING --------" );
		std::list<std::string> outputs;
		outputs.push_back( write1.getName() );
		// computing at time 3 with an offset of -2, it require the frame 1
		g.compute( outputs, ComputeOptions( 4 ) );
	}
	catch(... )
	{
		std::cerr << boost::current_exception_diagnostic_information() << std::endl;
		BOOST_FAIL( "Exception" );
	}
}


BOOST_AUTO_TEST_SUITE_END()

