#define BOOST_TEST_MODULE tuttle_graph
#include <tuttle/test/main.hpp>

#include <tuttle/host/Graph.hpp>
#include <tuttle/host/Node.hpp>

#include <iostream>

using namespace boost::unit_test;
using namespace tuttle::host;

BOOST_AUTO_TEST_SUITE( tuttle_graph )

BOOST_AUTO_TEST_CASE( create_node )
{
	TUTTLE_LOG_INFO( "--> PLUGINS CREATION" );
	Graph g;
	BOOST_CHECK_NO_THROW( g.createNode( "tuttle.invert" ) );
	TUTTLE_LOG_INFO( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( graph_copy )
{
	TUTTLE_LOG_INFO( "--> PLUGINS CREATION" );
	Graph g;
	Graph::Node& read1  = g.createNode( "tuttle.pngreader" );
	Graph::Node& invert1 = g.createNode( "tuttle.invert" );
	Graph::Node& invert2 = g.createNode( "tuttle.invert" );
	Graph::Node& write1 = g.createNode( "tuttle.pngwriter" );

	TUTTLE_LOG_INFO( "-------- GRAPH CONNECTION --------" );
	g.connect( read1, invert1 );
	g.connect( invert1, invert2 );
	g.connect( invert2, write1 );

	TUTTLE_LOG_INFO( "-------- GRAPH COPY --------" );
	Graph g2(g);
	
	BOOST_CHECK_NE( &g2.getNode( read1.getName() ), &read1 );
	BOOST_CHECK_EQUAL( g2.getNode( read1.getName() ).getName(), read1.getName() );
	TUTTLE_LOG_INFO( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( graph_NodeInit )
{
	TUTTLE_LOG_INFO( "--> PLUGINS NodeInit" );
	
	BOOST_CHECK(
		compute(
			list_of
			( NodeInit("tuttle.pngreader")
				.setParam("filename", "TuttleOFX-data/image/png/color-chart.png")
				.setParamExp("bitDepth", "32f") )
			( NodeInit("tuttle.pngwriter")
				.setParam("filename", ".tests/graph/output.png") )
			) );
	
	TUTTLE_LOG_INFO( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( graph_unconnect )
{
	TUTTLE_LOG_INFO( "--> PLUGINS unconnect" );
	Graph g;
	g.addConnectedNodes(
		list_of
		( NodeInit("tuttle.pngreader")
			.setParam("filename", "TuttleOFX-data/image/png/color-chart.png")
			.setParamExp("bitDepth", "32f") )
		( NodeInit("tuttle.invert") )
		( NodeInit("tuttle.invert") )
		( NodeInit("tuttle.invert") )
		( NodeInit("tuttle.pngwriter")
			.setParam("filename", ".tests/graph/output.png") )
		);
	
	std::vector<Graph::Node*> invNodes = g.getNodesByPlugin( "tuttle.invert" );
	
	BOOST_FOREACH( Graph::Node* n, invNodes )
	{
		BOOST_CHECK_EQUAL( g.getNbInputConnections(*n), 1 );
		BOOST_CHECK_EQUAL( g.getNbOutputConnections(*n), 1 );
	}
	g.unconnect( *invNodes[1] );
	
	BOOST_CHECK_EQUAL( g.getNbInputConnections(*invNodes[0]), 1 );
	BOOST_CHECK_EQUAL( g.getNbOutputConnections(*invNodes[0]), 0 );
	
	BOOST_CHECK_EQUAL( g.getNbInputConnections(*invNodes[1]), 0 );
	BOOST_CHECK_EQUAL( g.getNbOutputConnections(*invNodes[1]), 0 );
	
	BOOST_CHECK_EQUAL( g.getNbInputConnections(*invNodes[2]), 0 );
	BOOST_CHECK_EQUAL( g.getNbOutputConnections(*invNodes[2]), 1 );
	
	TUTTLE_LOG_INFO( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( graph_replaceNodeConnections )
{
	TUTTLE_LOG_INFO( "--> PLUGINS replace node connections" );
	Graph g;
	g.addConnectedNodes(
		list_of
		( NodeInit("tuttle.pngreader")
			.setParam("filename", "TuttleOFX-data/image/png/color-chart.png")
			.setParamExp("bitDepth", "32f") )
		( NodeInit("tuttle.invert") )
		( NodeInit("tuttle.pngwriter")
			.setParam("filename", ".tests/graph/output.png") )
		);
	
	Graph::Node& inv = *g.getNodesByPlugin( "tuttle.invert" ).front();
	
	Graph::Node& blur = g.addNode( NodeInit("tuttle.blur").setParam( "size", 0.01, 0.02 ) );
	
	BOOST_CHECK_EQUAL( blur.getParam( "size" ).getDoubleValueAtIndex(0), 0.01 );
	BOOST_CHECK_EQUAL( blur.getParam( "size" ).getDoubleValueAtIndex(1), 0.02 );
	
	BOOST_CHECK_EQUAL( g.getNbInputConnections(inv), 1 );
	BOOST_CHECK_EQUAL( g.getNbOutputConnections(inv), 1 );
	
	BOOST_CHECK_EQUAL( g.getNbInputConnections(blur), 0 );
	BOOST_CHECK_EQUAL( g.getNbOutputConnections(blur), 0 );
	
	g.replaceNodeConnections( inv, blur );
	
	BOOST_CHECK_EQUAL( g.getNbInputConnections(inv), 0 );
	BOOST_CHECK_EQUAL( g.getNbOutputConnections(inv), 0 );
	
	BOOST_CHECK_EQUAL( g.getNbInputConnections(blur), 1 );
	BOOST_CHECK_EQUAL( g.getNbOutputConnections(blur), 1 );
	
	BOOST_CHECK( g.compute( *g.getNodesByPlugin( "tuttle.pngwriter" ).front() ) );
	TUTTLE_LOG_INFO( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( create_processGraph )
{
	TUTTLE_LOG_INFO( "--> PLUGINS CREATION" );
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

	TUTTLE_LOG_INFO( "--> PLUGINS CONFIGURATION" );
	// Setup parameters
	read1.getParam( "filename" ).setValue( "TuttleOFX-data/image/jpeg/GRN.JPG" );
	read1.getParam( "bitDepth" ).setValue( 3 );
	read2.getParam( "filename" ).setValue( "TuttleOFX-data/image/jpeg/RED.JPG" );
	read2.getParam( "bitDepth" ).setValue( 3 );
	//bitdepth.getParam( "outputBitDepth" ).setValue( 3 );
	//	crop1.getParam( "Down" ).setValue( 400 );
	write1.getParam( "filename" ).setValue( ".tests/processGraph/output1.png" );
	write2.getParam( "filename" ).setValue( ".tests/processGraph/output2.jpg" );
	write3.getParam( "filename" ).setValue( ".tests/processGraph/output3.exr" );
	write4.getParam( "filename" ).setValue( ".tests/processGraph/output4.png" );

	TUTTLE_LOG_INFO( "-------- GRAPH CONNECTION --------" );
	TUTTLE_LOG_TRACE( "connect" );
	g.connect( read1, invert1 );
	TUTTLE_LOG_TRACE( "connect" );
	//		g.connect( invert1, bitdepth );
	TUTTLE_LOG_TRACE( "connect" );
	g.connect( invert1, invert2 );
	TUTTLE_LOG_TRACE( "connect" );
	g.connect( invert2, invert3 );
	TUTTLE_LOG_TRACE( "connect" );
	g.connect( invert3, write1 );
	TUTTLE_LOG_TRACE( "connect" );
	g.connect( invert1, invert4 );
	TUTTLE_LOG_TRACE( "connect" );
	g.connect( invert4, write2 );
	TUTTLE_LOG_TRACE( "connect" );
	g.connect( invert1, write3 );

	TUTTLE_LOG_INFO( "-------- GRAPH CONNECT CLIPS --------" );
	g.connect( invert1, merge1.getAttribute( "A" ) );
	g.connect( read2, merge1.getAttribute( "B" ) );
	g.connect( merge1, write4 );

	TUTTLE_LOG_INFO( "-------- SET GRAPH OUTPUTS --------" );
	std::list<std::string> outputs;
	outputs.push_back( write1.getName() );
	outputs.push_back( write2.getName() );
	outputs.push_back( write3.getName() );
	outputs.push_back( write4.getName() );
	
	TUTTLE_LOG_INFO( "-------- GRAPH PROCESSING --------" );
//	BOOST_CHECK_NO_THROW( g.compute( outputs ) ); /// @todo Bug: error with merge

	TUTTLE_LOG_INFO( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_CASE( graph_compute )
{
	TUTTLE_LOG_INFO( "--> PLUGINS CREATION" );
	Graph g;
	Graph::Node& read1  = g.createNode( "tuttle.pngreader" );
	Graph::Node& read2  = g.createNode( "tuttle.pngreader" );
	Graph::Node& invert1 = g.createNode( "tuttle.invert" );
	Graph::Node& merge1 = g.createNode( "tuttle.merge" );
	Graph::Node& write1 = g.createNode( "tuttle.pngwriter" );

	TUTTLE_LOG_INFO( "--> PLUGINS CONFIGURATION" );
	read1.getParam( "filename" ).setValue( "TuttleOFX-data/image/png/RGB16Million.png" );
	read1.getParam( "bitDepth" ).setValue( 3 );
	
	read2.getParam( "filename" ).setValue( "TuttleOFX-data/image/png/RGB16Million.png" );
	read2.getParam( "bitDepth" ).setValue( 3 );
	
	write1.getParam( "filename" ).setValue( ".tests/computeGraph/output.png" );
	
	TUTTLE_LOG_INFO( "-------- GRAPH CONNECTION --------" );
	g.connect( read1, invert1 );
	g.connect( read2, merge1.getClip("A") );
	g.connect( invert1, merge1.getClip("B") );
	g.connect( merge1, write1 );

//	BOOST_CHECK_NO_THROW( g.compute( write1 ) ); /// @toto
	TUTTLE_LOG_INFO( "----------------- DONE -----------------" );
}

BOOST_AUTO_TEST_SUITE_END()

