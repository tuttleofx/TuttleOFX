#define BOOST_TEST_MODULE tuttle_boostgraph
#include <tuttle/test/main.hpp>

// custom host
#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/GraphExporter.hpp>
#include <tuttle/host/graph/Visitors.hpp>

#include <boost/graph/graph_utility.hpp>
//#include <boost/bind.hpp>

#include <iostream>
#include <vector>

namespace tuttle {
namespace host {
namespace graph {

class TestVertex
{
public:
	typedef std::string Key;
public:
	TestVertex() {}
	TestVertex( const std::string& name ) : _name( name ) {}
	~TestVertex() {}
	std::string _name;
	const std::string& getName() const { return _name; }
	const std::string& getKey() const { return getName(); }
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
		, _name( out + "->" + in )
	{}

	~TestEdge() {}
	std::string _nameIn;
	std::string _nameOut;
	std::string _name;
	const std::string&           getName() const { return _name; }
	const std::string&           getInAttrName() const { return _nameIn; }
	friend std::ostream& operator<<( std::ostream& os, const TestEdge& v );
};
inline std::ostream& operator<<( std::ostream& os, const TestEdge& e )
{
	os << e._name;
	return os;
}

template<class G>
struct vertex_label_writer
{
	vertex_label_writer( const G& g ) : _g( g ) {}
	template<class Vertex>
	void operator()( std::ostream& out, const Vertex& v ) const
	{
		out << dotEntry( "type", "vertex" );
		out << dotEntry( "label", _g[v].getName() );
	}

	private:
		const G& _g;
};

template<class G>
struct edge_label_writer
{
	edge_label_writer( const G& g ) : _g( g ) {}
	template<class Edge>
	void operator()( std::ostream& out, const Edge& e ) const
	{
		out << dotEntry( "type", "vertex" );
		out << dotEntry( "label", _g[e].getName() );
	}

	private:
		const G& _g;
};

template<template<typename> class T, class G>
T<G> make( const G& g )
{
	return T<G>( g );
}

template<>
void exportAsDOT<TestVertex, TestEdge>( std::ostream& os, const InternalGraph<TestVertex, TestEdge >& g )
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
	                       make<vertex_label_writer>( g.getGraph() ),
	                       make<edge_label_writer>( g.getGraph() ),
	                       boost::make_graph_attributes_writer( graph_attr, vertex_attr, edge_attr ) );

}

}
}
}

BOOST_AUTO_TEST_SUITE( tuttle_boostgraph_suite )

using namespace boost::unit_test;

BOOST_AUTO_TEST_CASE( test_export )
{
	using namespace tuttle::host;

	typedef graph::InternalGraph<graph::TestVertex, graph::TestEdge> InternalGraph;
	typedef InternalGraph::vertex_descriptor Descriptor;
	typedef std::map<std::string, int> InstanceCountMap;

	std::string n1( "v1" );
	std::string n2( "v2" );
	std::string n3( "v3" );

	InternalGraph graph;
	std::map<std::string, Descriptor> nodesDescriptor;
	nodesDescriptor[n1] = graph.addVertex( graph::TestVertex( n1 ) );
	nodesDescriptor[n2] = graph.addVertex( graph::TestVertex( n2 ) );
	nodesDescriptor[n3] = graph.addVertex( graph::TestVertex( n3 ) );

	graph.addEdge( nodesDescriptor[n1], nodesDescriptor[n2], graph::TestEdge( n1, n2 ) );
	graph.addEdge( nodesDescriptor[n2], nodesDescriptor[n3], graph::TestEdge( n2, n3 ) );

	TUTTLE_LOG_TRACE( "graph:" );
	boost::print_graph( graph.getGraph() );

	InternalGraph graphT;
	graphT.copyTransposed( graph );

	TUTTLE_LOG_TRACE( "graphT:" );
	boost::print_graph( graphT.getGraph() );

	graph::exportAsDOT<graph::TestVertex, graph::TestEdge>( "boostgraphtest.dot", graph );
	graph::exportAsDOT<graph::TestVertex, graph::TestEdge>( "boostgraphTtest.dot", graphT );

	TUTTLE_LOG_TRACE( "__________________________________________________" );
	TUTTLE_LOG_TRACE( "graph:" );
	//	std::vector<boost::default_color_type > colormap(boost::num_vertices(graph.getGraph()));
	graph::visitor::Test_dfs<InternalGraph> testVisitorA( graph );
	//	boost::depth_first_search( graph.getGraph(), boost::root_vertex(nodesDescriptor[n1]), boost::visitor(testVisitorA) );//, colormap );
	graph.depthFirstVisit( testVisitorA, nodesDescriptor[n1] );

	TUTTLE_LOG_TRACE( "__________________________________________________" );
	TUTTLE_LOG_TRACE( "graphT:" );

	std::map<std::string, InternalGraph::vertex_descriptor> mmap;
	for( InternalGraph::vertex_iterator i = vertices( graphT.getGraph() ).first, iEnd = vertices( graphT.getGraph() ).second;
	     i != iEnd;
	     ++i )
	{
		TUTTLE_LOG_TRACE( "pp: " << graphT.getGraph()[*i]._name );
		mmap[graphT.getGraph()[*i]._name] = *i;
		//		TUTTLE_LOG_TRACE( "pp: "<< graphT.getGraph()[*i]._name );
	}

	graph::visitor::Test_dfs<InternalGraph> testVisitorB( graph );
	//	boost::depth_first_search( graphT.getGraph(), boost::visitor(testVisitorB) );

	graphT.depthFirstVisit( testVisitorB, mmap["v3"] );

//	   graph_traits<graph_type>::out_edge_iterator ei, edge_end;
//	   for( boost::tie(ei, edge_end) = out_edges(*i, g); ei != edge_end; ++ei )
//	    std::cout << " --" << name[*ei] << "--> " << id[target(*ei, g)] << "  ";
}

BOOST_AUTO_TEST_CASE( check_cycle )
{
	using namespace tuttle::host;

	typedef graph::InternalGraph<graph::TestVertex, graph::TestEdge> InternalGraph;
	typedef InternalGraph::vertex_descriptor Descriptor;
	typedef std::map<std::string, int> InstanceCountMap;

	std::string n1( "v1" );
	std::string n2( "v2" );
	std::string n3( "v3" );

	InternalGraph graph;
	std::map<std::string, Descriptor> nodesDescriptor;
	nodesDescriptor[n1] = graph.addVertex( graph::TestVertex( n1 ) );
	nodesDescriptor[n2] = graph.addVertex( graph::TestVertex( n2 ) );
	nodesDescriptor[n3] = graph.addVertex( graph::TestVertex( n3 ) );

	graph.addEdge( nodesDescriptor[n1], nodesDescriptor[n2], graph::TestEdge( n1, n2 ) );
	graph.addEdge( nodesDescriptor[n2], nodesDescriptor[n3], graph::TestEdge( n2, n3 ) );
	
	BOOST_CHECK_THROW(
		graph.addEdge( nodesDescriptor[n3], nodesDescriptor[n2], graph::TestEdge( n3, n2 ) ),
		exception::Logic );
	BOOST_CHECK_THROW(
		graph.addEdge( nodesDescriptor[n3], nodesDescriptor[n1], graph::TestEdge( n3, n1 ) ),
		 exception::Logic );
	
	TUTTLE_LOG_TRACE( "graph:" );
	boost::print_graph( graph.getGraph() );
}

BOOST_AUTO_TEST_CASE( test_create_add_remove_simple )
{
	typedef boost::adjacency_list< boost::setS, boost::vecS, boost::bidirectionalS > TestGraph;
	TestGraph graph;
	
	// add vertices
	TestGraph::vertex_descriptor vd1 = boost::add_vertex( graph );
	TestGraph::vertex_descriptor vd2 = boost::add_vertex( graph );
	TestGraph::vertex_descriptor vd3 = boost::add_vertex( graph );
	TestGraph::vertex_descriptor vd4 = boost::add_vertex( graph );
	
	BOOST_CHECK_EQUAL( boost::num_vertices( graph ), 4 );
	BOOST_CHECK_EQUAL( boost::num_edges( graph ), 0 );
	
	boost::add_edge( vd1, vd2, graph );
	boost::add_edge( vd2, vd3, graph );
	boost::add_edge( vd3, vd4, graph );
	
	BOOST_CHECK_EQUAL( boost::num_vertices( graph ), 4 );
	BOOST_CHECK_EQUAL( boost::num_edges( graph ), 3 );
	
	boost::clear_vertex( vd3, graph );
	boost::remove_vertex( vd3, graph );
	
	BOOST_CHECK_EQUAL( boost::num_vertices( graph ), 3 );
	BOOST_CHECK_EQUAL( boost::num_edges( graph ), 1 );
	
	boost::add_edge(
		graph.vertex_set()[1],
		graph.vertex_set()[2],
		graph );
	
	BOOST_CHECK_EQUAL( boost::num_vertices( graph ), 3 );
	BOOST_CHECK_EQUAL( boost::num_edges( graph ), 2 );
}

/*
 * Stupid test to check insertion, remove of vertices and edges.
 */
BOOST_AUTO_TEST_CASE( test_create_add_remove )
{
	using namespace tuttle::host;
	typedef boost::adjacency_list< boost::listS, boost::vecS, boost::undirectedS, graph::TestVertex, graph::TestEdge > TestGraph;
	TestGraph graph;

	// add vertices
	for( std::size_t i = 0; i < 20; ++i )
	{
		TestGraph::vertex_descriptor vd = boost::add_vertex( graph );
		graph[vd]._name = "a" + boost::lexical_cast<std::string > ( i );
	}
	
	BOOST_CHECK_EQUAL( boost::num_vertices( graph ), 20 );
	BOOST_CHECK_EQUAL( boost::num_edges( graph ), 0 );
	
	// add edges
	for( std::size_t i = 5; i < 8; ++i )
	{
		TestGraph::edge_descriptor ed = boost::add_edge(
			graph.vertex_set()[i],
			graph.vertex_set()[i+1],
			graph ).first;
	}
	
	BOOST_CHECK_EQUAL( boost::num_vertices( graph ), 20 );
	BOOST_CHECK_EQUAL( boost::num_edges( graph ), 3 );
	
	// remove some vertices
	for( std::size_t i = 0; i < 5; ++i )
	{
		TestGraph::vertex_descriptor vd = graph.vertex_set()[0];
		boost::clear_vertex( vd, graph ); // remove in and out edges
		boost::remove_vertex( vd, graph ); // finally remove the vertex from the boost graph
	}
	
	BOOST_CHECK_EQUAL( boost::num_vertices( graph ), 20-5 );
	BOOST_CHECK_EQUAL( boost::num_edges( graph ), 3 );
	
	// edges
	for( std::size_t i = 9; i < 14; ++i )
	{
		TestGraph::edge_descriptor ed = boost::add_edge(
			graph.vertex_set()[i],
			graph.vertex_set()[i+1],
			graph ).first;
	}
	
	BOOST_CHECK_EQUAL( boost::num_vertices( graph ), 20-5 );
	BOOST_CHECK_EQUAL( boost::num_edges( graph ), 8 );
	
	// add some vertices
	for( std::size_t i = 0; i < 7; ++i )
	{
		TestGraph::vertex_descriptor vd = boost::add_vertex( graph );
		graph[vd]._name = "b" + boost::lexical_cast<std::string > ( i );
	}
	
	BOOST_CHECK_EQUAL( boost::num_vertices( graph ), 20 - 5 + 7 );
	BOOST_CHECK_EQUAL( boost::num_edges( graph ), 8 );
		
//	//zero some_property for all vertices
//	for( Graph::vertex_iterator i = vertices( graph ).first, iEnd = vertices( graph ).second;
//		 i != iEnd;
//		 ++i )
//	{
//		TUTTLE_LOG_TRACE( ": " << graph[*i]._name );
//	}
}


BOOST_AUTO_TEST_SUITE_END()

