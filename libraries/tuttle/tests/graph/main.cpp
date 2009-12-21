// custom host
#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/math/rectOp.hpp>

#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>
#include <tuttle/host/core/ClipImage.hpp>
#include <tuttle/host/core/Param.hpp>
#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/core/Core.hpp>
#include <tuttle/host/core/ImageEffectNode.hpp>
#include <tuttle/host/core/Graph.hpp>
#include <tuttle/host/core/ProcessGraph.hpp>

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
//	BOOST_CHECK_NO_THROW(core::EffectInstance* invert = g.createNode( "fr.hd3d.tuttle.invert" ));
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
//	core::Graph::Node& read1  = g.createNode( "fr.hd3d.tuttle.pngreader" );
//	core::Graph::Node& invert1 = g.createNode( "fr.hd3d.tuttle.invert" );
//	core::Graph::Node& invert2 = g.createNode( "fr.hd3d.tuttle.invert" );
//	core::Graph::Node& basic1 = g.createNode( "net.sf.openfx:basicplugin" );
//	core::Graph::Node& write1 = g.createNode( "fr.hd3d.tuttle.pngwriter" );
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
	std::cout<< " hm. " << std::endl;
	core::Core::instance().preload();
	core::Core::instance().getImageEffectPluginCache().dumpToStdOut();

	TCOUT("__________________________________________________1");

	core::Graph g;
	core::Graph::Node& read1   = g.createNode( "fr.hd3d.tuttle.pngreader" );
	core::Graph::Node& invert1 = g.createNode( "fr.hd3d.tuttle.invert" );
	core::Graph::Node& write1  = g.createNode( "fr.hd3d.tuttle.pngwriter" );

	TCOUT("__________________________________________________2");
	// Setup parameters
	read1.getParam("Input filename").set("input.png");
	write1.getParam("Output filename").set("output.png");
	OfxPointD renderScale = { 1.0, 1.0 };
	read1.paramInstanceChangedAction("Input filename", kOfxChangeUserEdited, OfxTime( 0 ), renderScale );
	write1.paramInstanceChangedAction("Output filename", kOfxChangeUserEdited, OfxTime( 0 ), renderScale );

	TCOUT("__________________________________________________3");
	g.connect( read1, invert1 );
	g.connect( invert1, write1 );

	//	core::ProcessGraph processGraph(g);

	TCOUT("__________________________________________________4");
	std::list<std::string> outputs;
	outputs.push_back( write1.getName() );
	g.compute( outputs, 0, 1 );

	TCOUT("__________________________________________________5");
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
//	core::Graph::Node& read1  = g.createNode( "fr.hd3d.tuttle.pngreader" );
////	core::Graph::Node& read2  = g.createNode( "fr.hd3d.tuttle.pngreader" );
//	core::Graph::Node& invert1 = g.createNode( "fr.hd3d.tuttle.invert" );
//	core::Graph::Node& invert2 = g.createNode( "fr.hd3d.tuttle.invert" );
////	core::Graph::Node& merge1 = g.createNode( "fr.hd3d.tuttle.merge" );
//	core::Graph::Node& basic1 = g.createNode( "net.sf.openfx:basicplugin" );
//	core::Graph::Node& write1 = g.createNode( "fr.hd3d.tuttle.pngwriter" );
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

