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


#define BOOST_TEST_MODULE "graph_tests"
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( graph_tests_suite01 )

BOOST_AUTO_TEST_CASE( create_node ) {

	using namespace std;
	using namespace tuttle::host;
	
	//core::Core::instance().getPluginCache().addDirectoryToPath( "/path/to/plugins" );
	//core::Core::instance().getPluginCache().scanPluginFiles();
	core::Core::instance().preload();

    tuttle::host::core::Core::instance().getImageEffectPluginCache().dumpToStdOut( );

	core::Graph g;
	BOOST_CHECK_NO_THROW(core::EffectInstance* invert = g.createNode( "fr.hd3d.tuttle.invert" ));

	/*
	reader = graph.createNode('fr.hd3d.tuttle.pngreader')
	invert = graph.createNode('fr.hd3d.tuttle.invert')
	writer1 = graph.createNode('fr.hd3d.tuttle.pngwriter')
	writer2 = graph.createNode('fr.hd3d.tuttle.pngwriter')

	# setParam(scriptname, value)
	reader.param('filename').setValue('test1.png')
	writer1.param('filename').setValue('test1_out1.png')
	writer2.param('filename').setValue('test1_out2.png')
	writer1.metaData('author').setValue('eloi')

	# connect(output, input)
	graph.connect(reader, invert)
	graph.connect(invert, writer1)
	graph.connect(reader, writer2)
	graph.compute()
	*/
}

/*
BOOST_AUTO_TEST_CASE( connect_two_nodes ) {

	tuttle::Graph<Vertex, Edge> graph;

	Vertex A( "nodeA" );
	Vertex B( "nodeB" );
	tuttle::Graph<Vertex, Edge>::VertexDescriptor ADesc = graph.addVertex(A);
	tuttle::Graph<Vertex, Edge>::VertexDescriptor BDesc = graph.addVertex(B);

	Edge AtoB( "edgeAtoB" );
	tuttle::Graph<Vertex, Edge>::EdgeDescriptor AtoBDesc = graph.addEdge(ADesc, BDesc, AtoB);

	BOOST_CHECK_EQUAL(graph.instance(ADesc).name(), std::string("nodeA"));
	BOOST_CHECK_EQUAL(graph.getVertexCount(), 2);
	BOOST_CHECK_EQUAL(graph.getEdgeCount(), 1);
	BOOST_CHECK_EQUAL(graph.instance(BDesc).name(), std::string("nodeB"));
	BOOST_CHECK_EQUAL(graph.instance(AtoBDesc).name(), std::string("edgeAtoB"));
}


BOOST_AUTO_TEST_CASE( delete_one_node ) {

	tuttle::Graph<Vertex, Edge> graph;

	Vertex A( "nodeA" );
	Vertex B( "nodeB" );
	Vertex C( "nodeC" );
	Vertex D( "nodeD" );

	Edge AtoB( "AtoB" );
	Edge BtoC( "BtoC" );
	Edge BtoD( "BtoD" );
	Edge CtoD( "CtoD" );

	tuttle::Graph<Vertex, Edge>::VertexDescriptor ADesc = graph.addVertex(A);
	tuttle::Graph<Vertex, Edge>::VertexDescriptor BDesc = graph.addVertex(B);
	tuttle::Graph<Vertex, Edge>::VertexDescriptor CDesc = graph.addVertex(C);
	tuttle::Graph<Vertex, Edge>::VertexDescriptor DDesc = graph.addVertex(D);

	graph.addEdge(ADesc, BDesc, AtoB); // A -> B
	graph.addEdge(BDesc, CDesc, BtoC); // B -> C
	graph.addEdge(CDesc, DDesc, CtoD); // C -> D

	BOOST_CHECK_EQUAL(graph.getVertexCount(), 4);
	BOOST_CHECK_EQUAL(graph.getEdgeCount(), 3);

	graph.removeVertex(CDesc);

	BOOST_CHECK_EQUAL(graph.getVertexCount(), 3);
	BOOST_CHECK_EQUAL(graph.getEdgeCount(), 1);

	graph.addEdge(BDesc, DDesc, BtoD); // B -> D

	BOOST_CHECK_EQUAL(graph.getVertexCount(), 3);
	BOOST_CHECK_EQUAL(graph.getEdgeCount(), 2);
}

BOOST_AUTO_TEST_CASE( detect_cycle ) {

	tuttle::Graph<Vertex, Edge> graph;

	Vertex A( "nodeA" );
	Vertex B( "nodeB" );
	Vertex C( "nodeC" );

	Edge AtoB( "AtoB" );
	Edge BtoC( "BtoC" );
	Edge CtoA( "CtoA" );

	tuttle::Graph<Vertex, Edge>::VertexDescriptor ADesc = graph.addVertex(A);
	tuttle::Graph<Vertex, Edge>::VertexDescriptor BDesc = graph.addVertex(B);
	tuttle::Graph<Vertex, Edge>::VertexDescriptor CDesc = graph.addVertex(C);

	// A -> B : ok
	BOOST_CHECK_NO_THROW(graph.addEdge(ADesc, BDesc, AtoB));
	// B -> C : ok
	BOOST_CHECK_NO_THROW(graph.addEdge(BDesc, CDesc, BtoC));
	// C -> A : cycle -> exception
	BOOST_CHECK_THROW(graph.addEdge(CDesc, ADesc, CtoA), std::invalid_argument);

}
*/
BOOST_AUTO_TEST_SUITE_END()



