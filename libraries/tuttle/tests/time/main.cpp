#include <tuttle/host/Graph.hpp>

#include <iostream>

#define BOOST_TEST_MODULE tuttle_time
#include <boost/test/unit_test.hpp>
//#include <boost/test/execution_monitor.hpp>
//#include <boost/test/exception_safety.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( time_tests_suite01 )

BOOST_AUTO_TEST_CASE( preload )
{
	using namespace tuttle::host;

	Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
	Core::instance().preload();
}

BOOST_AUTO_TEST_CASE( time_shift )
{
	try
	{
		using namespace tuttle::host;
//		TUTTLE_TCOUT( Core::instance().getImageEffectPluginCache() );

		TUTTLE_TCOUT( "__________________________________________________1" );

		Graph g;
		Graph::Node& read1 = g.createNode( "tuttle.pngreader" );
		Graph::Node& invert1 = g.createNode( "tuttle.invert" );
		Graph::Node& timeshift1 = g.createNode( "tuttle.timeshift" );
		Graph::Node& write1 = g.createNode( "tuttle.pngwriter" );

		TUTTLE_TCOUT( "__________________________________________________2" );
		// Setup parameters
		read1.getParam( "filename" ).setValue( "data/input-###.png" );
		timeshift1.getParam("offset").setValue( 0 );
		write1.getParam( "filename" ).setValue( "data/output_####.png" );

		TUTTLE_TCOUT( "__________________________________________________3" );
		g.connect( read1, invert1 );
		g.connect( invert1, timeshift1 );
		g.connect( timeshift1, write1 );

		TUTTLE_TCOUT( "__________________________________________________4" );
		std::list<std::string> outputs;
		outputs.push_back( write1.getName() );
		g.compute( outputs, ComputeOptions(1) );

		TUTTLE_TCOUT( "__________________________________________________5" );
	}
	catch(... )
	{
		std::cerr << boost::current_exception_diagnostic_information() << std::endl;
		BOOST_FAIL( "Exception" );

	}
}

BOOST_AUTO_TEST_SUITE_END()

