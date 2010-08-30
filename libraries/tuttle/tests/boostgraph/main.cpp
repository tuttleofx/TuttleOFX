// custom host
#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/GraphExporter.hpp>
#include <tuttle/host/graph/Visitors.hpp>

#include <boost/graph/graph_utility.hpp>

#include <iostream>

class TestVertex
{
public:
	TestVertex() {}
	TestVertex( const std::string& name ) : _name( name ) {}
	~TestVertex() {}
	std::string _name;
	const std::string&           getName() const { return _name; }
	std::string&                 getName()       { return _name; }
	friend std::ostream& operator<<( std::ostream& os, const TestVertex& v );
};
inline std::ostream& operator<<( std::ostream& os, const TestVertex& v )
{
	os /*<< " _name:"*/ << v._name;
	return os;
}

class TestEdge
{
public:
	TestEdge() {}
	TestEdge( const std::string& in, const std::string& out )
		: _nameIn( in )
		, _nameOut( out )
	{}
	~TestEdge() {}
	std::string _nameIn;
	std::string _nameOut;
	friend std::ostream& operator<<( std::ostream& os, const TestEdge& v );
};
inline std::ostream& operator<<( std::ostream& os, const TestEdge& e )
{
	os << e._nameOut << "->" << e._nameIn;
	return os;
}

namespace tuttle {
namespace host {
namespace graph {

template<>
void exportAsDOT<TestVertex, TestEdge>( const InternalGraph<TestVertex, TestEdge >& g, std::ostream& os )
{
	std::map<std::string, std::string> graph_attr, vertex_attr, edge_attr;
	graph_attr["size"]       = "6,6";
	graph_attr["rankdir"]    = "LR";
	graph_attr["ratio"]      = "fill";
	graph_attr["label"]      = "TuttleOFX";
	vertex_attr["shape"]     = "circle";
	vertex_attr["color"]     = "dodgerblue4";
	vertex_attr["fontcolor"] = "dodgerblue4";
	edge_attr["style"]       = "dashed";
	edge_attr["minlen"]      = "1";
	edge_attr["color"]       = "darkslategray";
	edge_attr["fontcolor"]   = "darkslategray";

	using namespace boost;
	boost::write_graphviz( os,
						   g.getGraph(),
						   boost::make_label_writer( get( vertex_properties, g.getGraph() ) ),
						   boost::make_label_writer( get( edge_properties, g.getGraph() ) ),
						   boost::make_graph_attributes_writer( graph_attr, vertex_attr, edge_attr ) );
}


}
}
}

#define BOOST_TEST_MODULE graph_tests
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( internalgraph_tests_suite01 )

BOOST_AUTO_TEST_CASE( create_internalGraph )
{
	using namespace std;
	using namespace tuttle::host;

	typedef graph::InternalGraph<TestVertex, TestEdge> InternalGraph;
	typedef graph::InternalGraph<TestVertex, TestEdge>::vertex_descriptor Descriptor;
	typedef std::map<std::string, int> InstanceCountMap;

	std::string n1( "v1" );
	std::string n2( "v2" );
	std::string n3( "v3" );

	typedef graph::InternalGraph<TestVertex, TestEdge> InternalGraph;
	InternalGraph graph;
	std::map<std::string, Descriptor> nodesDescriptor;
	nodesDescriptor[n1] = graph.addVertex( TestVertex( n1 ) );
	nodesDescriptor[n2] = graph.addVertex( TestVertex( n2 ) );
	nodesDescriptor[n3] = graph.addVertex( TestVertex( n3 ) );

	TestEdge e1( n1, n2 );
	graph.addEdge( nodesDescriptor[n1], nodesDescriptor[n2], e1 );
	TestEdge e2( n2, n3 );
	graph.addEdge( nodesDescriptor[n2], nodesDescriptor[n3], e2 );

	TCOUT( "graph:" );
	boost::print_graph( graph.getGraph() );

	InternalGraph graphT;
	graphT.copyTransposed( graph );

	TCOUT( "graphT:" );
	boost::print_graph( graphT.getGraph() );

	graph::exportAsDOT<TestVertex, TestEdge>( graph, "boostgraphtest.dot" );
	graph::exportAsDOT<TestVertex, TestEdge>( graphT, "boostgraphTtest.dot" );

	TCOUT( "__________________________________________________" );
	TCOUT( "graph:" );
	//	std::vector<boost::default_color_type > colormap(boost::num_vertices(graph.getGraph()));
	graph::visitor::Test_dfs<InternalGraph> testVisitorA(graph);
	//	boost::depth_first_search( graph.getGraph(), boost::root_vertex(nodesDescriptor[n1]), boost::visitor(testVisitorA) );//, colormap );
	graph.dfs( testVisitorA, nodesDescriptor[n1] );

	TCOUT( "__________________________________________________" );
	TCOUT( "graphT:" );

	std::map<std::string, InternalGraph::vertex_descriptor> mmap;
	for( InternalGraph::vertex_iterator i = vertices( graphT.getGraph() ).first, iEnd = vertices( graphT.getGraph() ).second;
	     i != iEnd;
	     ++i )
	{
		TCOUT( "pp: " << get( vertex_properties, graphT.getGraph() )[*i]._name );
		mmap[get( vertex_properties, graphT.getGraph() )[*i]._name] = *i;
		//		TCOUT( "pp: "<< graphT.getGraph()[*i]._name );
	}

	graph::visitor::Test_dfs<InternalGraph> testVisitorB(graph);
	//	boost::depth_first_search( graphT.getGraph(), boost::visitor(testVisitorB) );

	graphT.dfs( testVisitorB, mmap["v3"] );
	/*
	graph_traits<graph_type>::out_edge_iterator ei, edge_end;
	for( boost::tie(ei, edge_end) = out_edges(*i, g); ei != edge_end; ++ei )
		std::cout << " --" << name[*ei] << "--> " << id[target(*ei, g)] << "  ";
	 */

}

/*
BOOST_AUTO_TEST_CASE( create_boostGraph )
{
	typedef boost::adjacency_list< boost::listS, boost::vecS, boost::undirectedS, TestVertex, TestEdge > Graph;
	Graph g;

	//add 100 vertices
	for( int i=0; i<15; ++i )
	{
		Graph::vertex_descriptor v = add_vertex(g);
		g[v]._name = "albert" + boost::lexical_cast<std::string>(i);
	}

	//zero some_property for all vertices
	for( Graph::vertex_iterator i = vertices(g).first, iEnd = vertices(g).second;
								i != iEnd;
								++i )
	{
		TCOUT(": " << g[*i]._name);
	}

}
 */

BOOST_AUTO_TEST_SUITE_END()

