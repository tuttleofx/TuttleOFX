#include <tuttle/host/Graph.hpp>

#include <iostream>

#define BOOST_TEST_MODULE graph_tests
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( graph_tests_suite01 )

BOOST_AUTO_TEST_CASE( common_preload_plugins )
{
	using namespace tuttle::host;

	Core::instance().getPluginCache().addDirectoryToPath( BOOST_PP_STRINGIZE(TUTTLE_PLUGIN_PATH) );
	Core::instance().preload();
	TUTTLE_TCOUT( Core::instance().getImageEffectPluginCache() );
}

BOOST_AUTO_TEST_CASE( create_node )
{
	using namespace tuttle::host;

	TUTTLE_TCOUT( tuttle::host::Core::instance().getImageEffectPluginCache() );

	Graph g;
	BOOST_CHECK_NO_THROW( g.createNode( "tuttle.invert" ) );
}

BOOST_AUTO_TEST_CASE( graph_copy )
{
	using namespace tuttle::host;

	Graph g;
	Graph::Node& read1  = g.createNode( "tuttle.pngreader" );
	Graph::Node& invert1 = g.createNode( "tuttle.invert" );
	Graph::Node& invert2 = g.createNode( "tuttle.invert" );
	Graph::Node& write1 = g.createNode( "tuttle.pngwriter" );

	g.connect( read1, invert1 );
	g.connect( invert1, invert2 );
	g.connect( invert2, write1 );

	Graph g2(g);

	BOOST_CHECK_NE( &g2.getNode( read1.getName() ), &read1 );
	BOOST_CHECK( g2.getNode( read1.getName() ) == read1 );
}

BOOST_AUTO_TEST_CASE( create_processGraph )
{
	using namespace tuttle::host;

	TUTTLE_TCOUT( "__________________________________________________1" );

	Graph g;
	Graph::Node& read1 = g.createNode( "tuttle.pngreader" );
//	/*Graph::Node& read2   = */ g.createNode( "tuttle.dpxreader" ); // add unused node
//	/*Graph::Node& read2   = */g.createNode( "tuttle.dpxreader" );  // add unused node
	Graph::Node& read3 = g.createNode( "tuttle.exrreader" );
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

	TUTTLE_TCOUT( "__________________________________________________2" );
	// Setup parameters
	read1.getParam( "filename" ).setValue( "data/input.png" );
	//		read2.getParam( "filename" ).setValue( "data/input.dpx" );
	read3.getParam( "filename" ).setValue( "data/input.exr" );
	//bitdepth.getParam( "outputBitDepth" ).setValue( 3 );
	//	crop1.getParam( "Down" ).setValue( 400 );
	write1.getParam( "filename" ).setValue( "data/output1.png" );
	write2.getParam( "filename" ).setValue( "data/output2.jpg" );
	write3.getParam( "filename" ).setValue( "data/output3.exr" );
	write4.getParam( "filename" ).setValue( "data/output4.png" );

	TUTTLE_TCOUT( "__________________________________________________3" );
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

	TUTTLE_TCOUT( "__________3.5" );
	g.connect( invert1, merge1.getAttribute( "A" ) );
	//	g.connect( bitdepth, merge1.getAttribute("A") );
	TUTTLE_TCOUT( "__________3.5" );
	g.connect( read3, merge1.getAttribute( "B" ) );
	TUTTLE_TCOUT( "__________3.5" );
	g.connect( merge1, write4 );

	TUTTLE_TCOUT( "__________________________________________________4" );
	std::list<std::string> outputs;
	outputs.push_back( write1.getName() );
	outputs.push_back( write2.getName() );
	outputs.push_back( write3.getName() );
	outputs.push_back( write4.getName() );
	
	BOOST_CHECK_NO_THROW( g.compute( outputs ) );

	TUTTLE_TCOUT( "__________________________________________________5" );
}

BOOST_AUTO_TEST_CASE( graph_compute )
{
	using namespace tuttle::host;

	Graph g;
	Graph::Node& read1  = g.createNode( "tuttle.pngreader" );
	Graph::Node& read2  = g.createNode( "tuttle.pngreader" );
	Graph::Node& invert1 = g.createNode( "tuttle.invert" );
	Graph::Node& merge1 = g.createNode( "tuttle.merge" );
	Graph::Node& write1 = g.createNode( "tuttle.pngwriter" );

	read1.getParam( "filename" ).setValue( "data/input.png" );
	read2.getParam( "filename" ).setValue( "data/input.png" );
	write1.getParam( "filename" ).setValue( "data/output.png" );
	
	g.connect( read1, invert1 );
	g.connect( read2, merge1.getClip("A") );
	g.connect( invert1, merge1.getClip("B") );
	g.connect( merge1, write1 );

//	BOOST_CHECK_NO_THROW( g.compute( write1 ) ); /// @toto
}

BOOST_AUTO_TEST_SUITE_END()

