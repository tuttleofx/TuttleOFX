// custom host
#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/math/rectOp.hpp>

#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/core/EffectInstance.hpp>
#include <tuttle/host/core/ClipInstance.hpp>
#include <tuttle/host/core/ParamInstance.hpp>
#include <tuttle/host/core/HostDescriptor.hpp>
#include <tuttle/host/core/Core.hpp>
#include <tuttle/host/core/EffectInstance.hpp>
#include <tuttle/host/core/Graph.hpp>

#include <iostream>

#define BOOST_TEST_MODULE graph_tests
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

class A
{
public:
	int _id;
	A( const int id ):_id(id) {}
	~A(){}
	bool operator==(const A& other) const { return _id == other._id; }
};

BOOST_AUTO_TEST_SUITE( graph_tests_suite01 )

/*
 * BOOST_AUTO_TEST_CASE( create_node ) {
 *
 *  using namespace std;
 *  using namespace tuttle::host;
 *
 *  //core::Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
 *  //core::Core::instance().getPluginCache().scanPluginFiles();
 *  core::Core::instance().preload();
 *
 *  tuttle::host::core::Core::instance().getImageEffectPluginCache().dumpToStdOut( );
 *
 *  core::Graph g;
 *  BOOST_CHECK_NO_THROW(core::EffectInstance* invert = g.createNode( "fr.hd3d.tuttle.invert" ));
 *
 *  //	reader = graph.createNode('fr.hd3d.tuttle.pngreader')
 *  //	invert = graph.createNode('fr.hd3d.tuttle.invert')
 *  //	writer1 = graph.createNode('fr.hd3d.tuttle.pngwriter')
 *  //	writer2 = graph.createNode('fr.hd3d.tuttle.pngwriter')
 *  //
 *  //	# setParam(scriptname, value)
 *  //	reader.param('filename').setValue('test1.png')
 *  //	writer1.param('filename').setValue('test1_out1.png')
 *  //	writer2.param('filename').setValue('test1_out2.png')
 *  //	writer1.metaData('author').setValue('eloi')
 *  //
 *  //	# connect(output, input)
 *  //	graph.connect(reader, invert)
 *  //	graph.connect(invert, writer1)
 *  //	graph.connect(reader, writer2)
 *  //	graph.compute()
 * }
 */

BOOST_AUTO_TEST_CASE( create_node )
{
	using namespace std;
	using namespace tuttle::host;

	//core::Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
	//core::Core::instance().getPluginCache().scanPluginFiles();
	core::Core::instance().preload();

	core::Graph g;
	core::Graph::Node& read1  = g.createNode( "fr.hd3d.tuttle.pngreader" );
	core::Graph::Node& read2  = g.createNode( "fr.hd3d.tuttle.pngreader" );
	core::Graph::Node& invert1 = g.createNode( "fr.hd3d.tuttle.invert" );
	core::Graph::Node& invert2 = g.createNode( "fr.hd3d.tuttle.invert" );
	core::Graph::Node& basic1 = g.createNode( "net.sf.openfx:basicplugin" );
	core::Graph::Node& write1 = g.createNode( "fr.hd3d.tuttle.pngwriter" );

	g.connect( read1, invert1 );
	g.connect( invert1, invert2 );
	g.connect( invert2, write1 );

	//read1.getParam('filename').setValue('test1.png')

	std::cout
	<< std::endl
	<< "COMPUTE:"
	<< std::endl
	<< std::endl;

	g.compute();


	TCOUT("avant g2");
	core::Graph g2(g);
	TCOUT("apres g2");

	BOOST_CHECK_NE( &g2.getNode( read1.getName() ), &read1 );

	BOOST_CHECK( g2.getNode( read1.getName() ) == read1 );

	//g2.dumpToStdOut();

	//	reader = graph.createNode('fr.hd3d.tuttle.pngreader')
	//	invert = graph.createNode('fr.hd3d.tuttle.invert')
	//	writer1 = graph.createNode('fr.hd3d.tuttle.pngwriter')
	//	writer2 = graph.createNode('fr.hd3d.tuttle.pngwriter')
	//
	//	# setParam(scriptname, value)
	//	reader.param('filename').setValue('test1.png')
	//	writer1.param('filename').setValue('test1_out1.png')
	//	writer2.param('filename').setValue('test1_out2.png')
	//	writer1.metaData('author').setValue('eloi')
	//
	//	# connect(output, input)
	//	graph.connect(reader, invert)
	//	graph.connect(invert, writer1)
	//	graph.connect(reader, writer2)
	//	graph.compute()
}

BOOST_AUTO_TEST_SUITE_END()

