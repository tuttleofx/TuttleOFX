#include <tuttle/host/Graph.hpp>

#include <iostream>

#define BOOST_TEST_MODULE tuttle_graph
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE( tuttle_graph_suite01 )

BOOST_AUTO_TEST_CASE( common_preload_plugins )
{
	TUTTLE_COUT( "-------- LOADING OPENFX PLUGINS --------" );
	Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
	Core::instance().preload();
//	TUTTLE_TCOUT( Core::instance().getImageEffectPluginCache() );
	TUTTLE_COUT( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( create_node )
{
	TUTTLE_COUT( "--> PLUGINS CREATION" );
	Graph g;
	BOOST_CHECK_NO_THROW( g.createNode( "tuttle.invert" ) );
	TUTTLE_COUT( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( graph_copy )
{
	TUTTLE_COUT( "--> PLUGINS CREATION" );
	Graph g;
	Graph::Node& read1  = g.createNode( "tuttle.pngreader" );
	Graph::Node& invert1 = g.createNode( "tuttle.invert" );
	Graph::Node& invert2 = g.createNode( "tuttle.invert" );
	Graph::Node& write1 = g.createNode( "tuttle.pngwriter" );

	TUTTLE_COUT( "-------- GRAPH CONNECTION --------" );
	g.connect( read1, invert1 );
	g.connect( invert1, invert2 );
	g.connect( invert2, write1 );

	TUTTLE_COUT( "-------- GRAPH COPY --------" );
	Graph g2(g);
	
	BOOST_CHECK_NE( &g2.getNode( read1.getName() ), &read1 );
	BOOST_CHECK( strcmp( g2.getNode( read1.getName() ).getName().c_str(), read1.getName().c_str() ) == 0 );
	//BOOST_CHECK( g2.getNode( read1.getName() ) == read1 );
	TUTTLE_COUT( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( create_processGraph )
{
	TUTTLE_COUT( "--> PLUGINS CREATION" );
	Graph g;
	Graph::Node& read1 = g.createNode( "tuttle.jpegreader" );
	Graph::Node& read2 = g.createNode( "tuttle.jpegreader" );
	Graph::Node& invert1 = g.createNode( "tuttle.invert" );
	Graph::Node& invert2 = g.createNode( "tuttle.invert" );
	Graph::Node& invert3 = g.createNode( "tuttle.invert" );
	Graph::Node& invert4 = g.createNode( "tuttle.invert" );
	/*Graph::Node& crop1   = */ g.createNode( "tuttle.crop" ); // add unused node
	Graph::Node& merge1 = g.createNode( "tuttle.merge" );
	Graph::Node& write1 = g.createNode( "tuttle.pngwriter" );
	Graph::Node& write4 = g.createNode( "tuttle.pngwriter" );
	Graph::Node& write2 = g.createNode( "tuttle.jpegwriter" );
	Graph::Node& write3 = g.createNode( "tuttle.exrwriter" );

	TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );
	// Setup parameters
	read1.getParam( "filename" ).setValue( "/datas/TuttleOFX-data/images/GRN.JPG" );
	read2.getParam( "filename" ).setValue( "/datas/TuttleOFX-data/images/RED.JPG" );
	//bitdepth.getParam( "outputBitDepth" ).setValue( 3 );
	//	crop1.getParam( "Down" ).setValue( 400 );
	write1.getParam( "filename" ).setValue( ".tests/processGraph/output1.png" );
	write2.getParam( "filename" ).setValue( ".tests/processGraph/output2.jpg" );
	write3.getParam( "filename" ).setValue( ".tests/processGraph/output3.exr" );
	write4.getParam( "filename" ).setValue( ".tests/processGraph/output4.png" );

	TUTTLE_COUT( "-------- GRAPH CONNECTION --------" );
	TUTTLE_TCOUT( "connect" );
	g.connect( read1, invert1 );
	TUTTLE_TCOUT( "connect" );
	//		g.connect( invert1, bitdepth );
	TUTTLE_TCOUT( "connect" );
	g.connect( invert1, invert2 );
	TUTTLE_TCOUT( "connect" );
	g.connect( invert2, invert3 );
	TUTTLE_TCOUT( "connect" );
	g.connect( invert3, write1 );
	TUTTLE_TCOUT( "connect" );
	g.connect( invert1, invert4 );
	TUTTLE_TCOUT( "connect" );
	g.connect( invert4, write2 );
	TUTTLE_TCOUT( "connect" );
	g.connect( invert1, write3 );

	TUTTLE_COUT( "-------- GRAPH CONNECT CLIPS --------" );
	g.connect( invert1, merge1.getAttribute( "A" ) );
	g.connect( read2, merge1.getAttribute( "B" ) );
	g.connect( merge1, write4 );

	TUTTLE_COUT( "-------- SET GRAPH OUTPUTS --------" );
	std::list<std::string> outputs;
	outputs.push_back( write1.getName() );
	outputs.push_back( write2.getName() );
	outputs.push_back( write3.getName() );
	outputs.push_back( write4.getName() );
	
	TUTTLE_COUT( "-------- GRAPH PROCESSING --------" );
	BOOST_CHECK_NO_THROW( g.compute( outputs ) );

	TUTTLE_COUT( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( graph_compute )
{
	TUTTLE_COUT( "--> PLUGINS CREATION" );
	Graph g;
	Graph::Node& read1  = g.createNode( "tuttle.pngreader" );
	Graph::Node& read2  = g.createNode( "tuttle.pngreader" );
	Graph::Node& invert1 = g.createNode( "tuttle.invert" );
	Graph::Node& merge1 = g.createNode( "tuttle.merge" );
	Graph::Node& write1 = g.createNode( "tuttle.pngwriter" );

	TUTTLE_COUT( "--> PLUGINS CONFIGURATION" );
	read1.getParam( "filename" ).setValue( "/datas/TuttleOFX-data/images/RGB16Million.png" );
	read2.getParam( "filename" ).setValue( "/datas/TuttleOFX-data/images/RGB16Million.png" );
	write1.getParam( "filename" ).setValue( ".tests/computeGraph/output.png" );
	
	TUTTLE_COUT( "-------- GRAPH CONNECTION --------" );
	g.connect( read1, invert1 );
	g.connect( read2, merge1.getClip("A") );
	g.connect( invert1, merge1.getClip("B") );
	g.connect( merge1, write1 );

//	BOOST_CHECK_NO_THROW( g.compute( write1 ) ); /// @toto
	TUTTLE_COUT( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_SUITE_END()

