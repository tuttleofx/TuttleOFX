#include <tuttle/host/Graph.hpp>

#include <iostream>

#define BOOST_TEST_MODULE graph_tests
#include <boost/test/unit_test.hpp>
//#include <boost/test/execution_monitor.hpp>
//#include <boost/test/exception_safety.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( graph_tests_suite01 )

//BOOST_AUTO_TEST_CASE( create_node ) {
//	using namespace std;
//	using namespace tuttle::host;
//
//	//Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
//	//Core::instance().getPluginCache().scanPluginFiles();
//	Core::instance().preload();
//
//	TCOUT( tuttle::host::Core::instance().getImageEffectPluginCache() );
//
//	Graph g;
//	BOOST_CHECK_NO_THROW(EffectInstance* invert = g.createNode( "fr.tuttle.invert" ));
//}

//BOOST_AUTO_TEST_CASE( graph_copy )
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
//	Graph::Node& invert1 = g.createNode( "fr.tuttle.invert" );
//	Graph::Node& invert2 = g.createNode( "fr.tuttle.invert" );
//	Graph::Node& basic1 = g.createNode( "net.sf.openfx:basicplugin" );
//	Graph::Node& write1 = g.createNode( "fr.tuttle.pngwriter" );
//
//	g.connect( read1, invert1 );
//	g.connect( invert1, invert2 );
//	g.connect( invert2, basic1 );
//	g.connect( basic1, write1 );
//
//	Graph g2(g);
//
//	BOOST_CHECK_NE( &g2.getNode( read1.getName() ), &read1 );
//	BOOST_CHECK( g2.getNode( read1.getName() ) == read1 );
//}

BOOST_AUTO_TEST_CASE( create_processGraph )
{
	try
	{
		//		::boost::execution_monitor ex_mon;

		using namespace std;
		using namespace tuttle::host;

		//Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
		//Core::instance().getPluginCache().scanPluginFiles();
		Core::instance().preload();
		TCOUT( Core::instance().getImageEffectPluginCache() );

		TCOUT( "__________________________________________________1" );

		Graph g;
		Graph::Node& read1 = g.createNode( "fr.tuttle.pngreader" );
		/*Graph::Node& read2   = */ g.createNode( "fr.tuttle.dpxreader" );
		//		/*Graph::Node& read2   = */g.createNode( "fr.tuttle.dpxreader" );
		Graph::Node& read3 = g.createNode( "fr.tuttle.exrreader" );
		//		Graph::Node& bitdepth = g.createNode( "fr.tuttle.bitdepth" );
		Graph::Node& invert1 = g.createNode( "fr.tuttle.invert" );
		Graph::Node& invert2 = g.createNode( "fr.tuttle.invert" );
		Graph::Node& invert3 = g.createNode( "fr.tuttle.invert" );
		Graph::Node& invert4 = g.createNode( "fr.tuttle.invert" );
		/*Graph::Node& crop1   = */ g.createNode( "fr.tuttle.crop" );
		Graph::Node& merge1 = g.createNode( "fr.tuttle.merge" );
		Graph::Node& write1 = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write4 = g.createNode( "fr.tuttle.pngwriter" );
		Graph::Node& write2 = g.createNode( "fr.tuttle.dpxwriter" );
		Graph::Node& write3 = g.createNode( "fr.tuttle.exrwriter" );

		TCOUT( "__________________________________________________2" );
		// Setup parameters
		read1.getParam( "filename" ).set( "data/input.png" );
		//		read2.getParam( "filename" ).set( "data/input.dpx" );
		read3.getParam( "filename" ).set( "data/input.exr" );
		//bitdepth.getParam( "outputBitDepth" ).set( 3 );
		//	crop1.getParam( "Down" ).set( 400 );
		write1.getParam( "filename" ).set( "data/output1.png" );
		write2.getParam( "filename" ).set( "data/output2.dpx" );
		write3.getParam( "filename" ).set( "data/output3.exr" );
		write4.getParam( "filename" ).set( "data/output4.png" );

		TCOUT( "__________________________________________________3" );
		TCOUT( "connect" );
		g.connect( read1, invert1 );
		TCOUT( "connect" );
		//		g.connect( invert1, bitdepth );
		TCOUT( "connect" );
		g.connect( invert1, invert2 );
		TCOUT( "connect" );
		g.connect( invert2, invert3 );
		TCOUT( "connect" );
		g.connect( invert3, write1 );
		TCOUT( "connect" );
		g.connect( invert1, invert4 );
		TCOUT( "connect" );
		g.connect( invert4, write2 );
		TCOUT( "connect" );
		g.connect( invert1, write3 );

		TCOUT( "__________3.5" );
		g.connect( invert1, merge1.getAttribute( "A" ) );
		//	g.connect( bitdepth, merge1.getAttribute("A") );
		TCOUT( "__________3.5" );
		g.connect( read3, merge1.getAttribute( "B" ) );
		TCOUT( "__________3.5" );
		//	g.connect( merge1, crop1 );
		g.connect( merge1, write4 );

		TCOUT( "__________________________________________________4" );
		std::list<std::string> outputs;
		outputs.push_back( write1.getName() );
		outputs.push_back( write2.getName() );
		outputs.push_back( write3.getName() );
		outputs.push_back( write4.getName() );
		g.compute( outputs, 0, 1 );

		TCOUT( "__________________________________________________5" );
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

