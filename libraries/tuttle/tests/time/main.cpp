#include <tuttle/host/Graph.hpp>

#include <iostream>

#define BOOST_TEST_MODULE graph_tests
#include <boost/test/unit_test.hpp>
//#include <boost/test/execution_monitor.hpp>
//#include <boost/test/exception_safety.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( time_tests_suite01 )

BOOST_AUTO_TEST_CASE( time_shift )
{
	try
	{
		using namespace std;
		using namespace tuttle::host;

		Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
		Core::instance().preload();
//		TUTTLE_TCOUT( Core::instance().getImageEffectPluginCache() );

		TUTTLE_TCOUT( "__________________________________________________1" );

		Graph g;
		Graph::Node& read1 = g.createNode( "fr.tuttle.pngreader" );
		Graph::Node& invert1 = g.createNode( "fr.tuttle.invert" );
		Graph::Node& timeshift1 = g.createNode( "fr.tuttle.timeshift" );
		Graph::Node& write1 = g.createNode( "fr.tuttle.pngwriter" );

		TUTTLE_TCOUT( "__________________________________________________2" );
		// Setup parameters
		read1.getParam( "filename" ).set( "data/input-###.png" );
		timeshift1.getParam("offset").set( 0 );
		write1.getParam( "filename" ).set( "data/output_####.png" );

		TUTTLE_TCOUT( "__________________________________________________3" );
		g.connect( read1, invert1 );
		g.connect( invert1, timeshift1 );
		g.connect( timeshift1, write1 );
//		g.connect( invert1, write1 );

		TUTTLE_TCOUT( "__________________________________________________4" );
		std::list<std::string> outputs;
		outputs.push_back( write1.getName() );
		g.compute( outputs, 1, 1 );

		TUTTLE_TCOUT( "__________________________________________________5" );
	}
	catch(... )
	{
		std::cerr << boost::current_exception_diagnostic_information() << std::endl;
		BOOST_FAIL( "Exception" );

	}
}

//BOOST_AUTO_TEST_CASE( graph_compute )
//{
//	using namespace std;
//	using namespace tuttle::host;
//
//	//Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
//	//Core::instance().getPluginCache().scanPluginFiles();
//	Core::instance().preload();
//
//	Graph g;
//	Graph::Node& read1  = g.createNode( "fr.tuttle.pngreader" );
////	Graph::Node& read2  = g.createNode( "fr.tuttle.pngreader" );
//	Graph::Node& invert1 = g.createNode( "fr.tuttle.invert" );
//	Graph::Node& invert2 = g.createNode( "fr.tuttle.invert" );
////	Graph::Node& merge1 = g.createNode( "fr.tuttle.merge" );
//	Graph::Node& basic1 = g.createNode( "net.sf.openfx:basicplugin" );
//	Graph::Node& write1 = g.createNode( "fr.tuttle.pngwriter" );
//
//	g.connect( read1, invert1 );
//	g.connect( invert1, invert2 );
//	g.connect( invert2, basic1 );
//	g.connect( basic1, write1 );
////	g.connect( read2, merge1, merge1.getClip("A") );
////	g.connect( invert1, merge1, merge1.getClip("B") );
////	g.connect( merge1, write1 );
//
//	BOOST_CHECK_NO_THROW( g.compute() );
//}

BOOST_AUTO_TEST_SUITE_END()

