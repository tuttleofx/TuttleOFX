#include <tuttle/host/core/Graph.hpp>

#include <iostream>

#define BOOST_TEST_MODULE graph_tests
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( graph_tests_suite01 )

//BOOST_AUTO_TEST_CASE( create_node ) {
//	using namespace std;
//	using namespace tuttle::host;
//
//	//core::Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
//	//core::Core::instance().getPluginCache().scanPluginFiles();
//	core::Core::instance().preload();
//
//	tuttle::host::core::Core::instance().getImageEffectPluginCache().dumpToStdOut( );
//
//	core::Graph g;
//	BOOST_CHECK_NO_THROW(core::EffectInstance* invert = g.createNode( "fr.tuttle.invert" ));
//}

//BOOST_AUTO_TEST_CASE( graph_copy )
//{
//	using namespace std;
//	using namespace tuttle::host;
//
//	//core::Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
//	//core::Core::instance().getPluginCache().scanPluginFiles();
//	core::Core::instance().preload();
//
//	core::Graph g;
//	core::Graph::Node& read1  = g.createNode( "fr.tuttle.pngreader" );
//	core::Graph::Node& invert1 = g.createNode( "fr.tuttle.invert" );
//	core::Graph::Node& invert2 = g.createNode( "fr.tuttle.invert" );
//	core::Graph::Node& basic1 = g.createNode( "net.sf.openfx:basicplugin" );
//	core::Graph::Node& write1 = g.createNode( "fr.tuttle.pngwriter" );
//
//	g.connect( read1, invert1 );
//	g.connect( invert1, invert2 );
//	g.connect( invert2, basic1 );
//	g.connect( basic1, write1 );
//
//	core::Graph g2(g);
//
//	BOOST_CHECK_NE( &g2.getNode( read1.getName() ), &read1 );
//	BOOST_CHECK( g2.getNode( read1.getName() ) == read1 );
//}

BOOST_AUTO_TEST_CASE( create_processGraph )
{
	using namespace std;
	using namespace tuttle::host;

	//core::Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
	//core::Core::instance().getPluginCache().scanPluginFiles();
	core::Core::instance().preload();
	core::Core::instance().getImageEffectPluginCache().dumpToStdOut();

	TCOUT( "__________________________________________________1" );

	core::Graph g;
	core::Graph::Node& read1   = g.createNode( "fr.tuttle.pngreader" );
	core::Graph::Node& read2   = g.createNode( "fr.tuttle.dpxreader" );
	core::Graph::Node& read3   = g.createNode( "fr.tuttle.exrreader" );
	core::Graph::Node& invert1 = g.createNode( "fr.tuttle.invert" );
	core::Graph::Node& invert2 = g.createNode( "fr.tuttle.invert" );
	core::Graph::Node& invert3 = g.createNode( "fr.tuttle.invert" );
	core::Graph::Node& invert4 = g.createNode( "fr.tuttle.invert" );
	/*core::Graph::Node& crop1   = */g.createNode( "fr.tuttle.crop" );
	core::Graph::Node& merge1  = g.createNode( "fr.tuttle.merge" );
	core::Graph::Node& write1  = g.createNode( "fr.tuttle.pngwriter" );
	core::Graph::Node& write4  = g.createNode( "fr.tuttle.pngwriter" );
	core::Graph::Node& write2  = g.createNode( "fr.tuttle.dpxwriter" );
	core::Graph::Node& write3  = g.createNode( "fr.tuttle.exrwriter" );

	TCOUT( "__________________________________________________2" );
	// Setup parameters
	read1.getParam( "filename" ).set( "input.png" );
	read2.getParam( "filename" ).set( "input.dpx" );
	read3.getParam( "filename" ).set( "input.exr" );
//	crop1.getParam( "Down" ).set( 400 );
	write1.getParam( "filename" ).set( "output1.png" );
	write2.getParam( "filename" ).set( "output2.dpx" );
	write3.getParam( "filename" ).set( "output3.exr" );
	write4.getParam( "filename" ).set( "output4.png" );

	TCOUT( "__________________________________________________3" );
	g.connect( read1, invert1 );
	g.connect( invert1, invert2 );
	g.connect( invert2, invert3 );
	g.connect( invert3, write1 );
	g.connect( invert1, invert4 );
	g.connect( invert4, write2 );
	g.connect( invert1, write3 );

	g.connect( invert1, merge1.getProcessAttribute("SourceA") );
	g.connect( read3, merge1.getProcessAttribute("SourceB") );
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

//BOOST_AUTO_TEST_CASE( graph_compute )
//{
//	using namespace std;
//	using namespace tuttle::host;
//
//	//core::Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
//	//core::Core::instance().getPluginCache().scanPluginFiles();
//	core::Core::instance().preload();
//
//	core::Graph g;
//	core::Graph::Node& read1  = g.createNode( "fr.tuttle.pngreader" );
////	core::Graph::Node& read2  = g.createNode( "fr.tuttle.pngreader" );
//	core::Graph::Node& invert1 = g.createNode( "fr.tuttle.invert" );
//	core::Graph::Node& invert2 = g.createNode( "fr.tuttle.invert" );
////	core::Graph::Node& merge1 = g.createNode( "fr.tuttle.merge" );
//	core::Graph::Node& basic1 = g.createNode( "net.sf.openfx:basicplugin" );
//	core::Graph::Node& write1 = g.createNode( "fr.tuttle.pngwriter" );
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

