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
	TestVertex(){}
	TestVertex(const std::string& name):_name(name){}
	~TestVertex(){}
	std::string _name;
	friend std::ostream& operator<<( std::ostream& os, const TestVertex& v );
};
inline std::ostream& operator<<( std::ostream& os, const TestVertex& v )
{
	os << " _name:" << v._name;
	return os;
}

class TestEdge
{
public:
	TestEdge(){}
	TestEdge(const std::string& in, const std::string& out):_nameIn(in),_nameOut(out){}
	~TestEdge(){}
	std::string _nameIn;
	std::string _nameOut;
	friend std::ostream& operator<<( std::ostream& os, const TestEdge& v );
};
inline std::ostream& operator<<( std::ostream& os, const TestEdge& e )
{
	os << " _in:" << e._nameIn << " out:" << e._nameOut;
	return os;
}

namespace tuttle {
namespace host {
namespace graph {
template <>
struct GraphExporter<TestVertex, TestEdge >
{
	static void exportAsDOT( const InternalGraph<TestVertex, TestEdge >& g, const char* filename )
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
		std::ofstream ofs( filename );
		boost::write_graphviz( ofs,
		                       g.getGraph(),
		                       boost::make_label_writer( get( vertex_properties, g.getGraph() ) ),
		                       boost::make_label_writer( get( edge_properties, g.getGraph() ) ),
		                       boost::make_graph_attributes_writer( graph_attr, vertex_attr, edge_attr ) );
	}

};
}}}






#define BOOST_TEST_MODULE graph_tests
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( internalgraph_tests_suite01 )

BOOST_AUTO_TEST_CASE( create_internalGraph )
{
	using namespace std;
	using namespace tuttle::host;

	typedef graph::InternalGraph<TestVertex, TestEdge> InternalGraph;
	typedef graph::InternalGraph<TestVertex, TestEdge>::VertexDescriptor Descriptor;
	typedef std::map<std::string, int> InstanceCountMap;

	std::string n1("coucou1");
	std::string n2("coucou2");
	std::string n3("coucou3");

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

	TCOUT("graph:");
	boost::print_graph( graph.getGraph() );


	InternalGraph graphT;
	graphT.copyTransposed(graph);
	
	TCOUT("graphT:");
	boost::print_graph( graphT.getGraph() );

	graph::GraphExporter<TestVertex, TestEdge>::exportAsDOT( graph, "boostgraphtest.dot" );
	graph::GraphExporter<TestVertex, TestEdge>::exportAsDOT( graphT, "boostgraphTtest.dot" );

	TCOUT("__________________________________________________");
	TCOUT("graph:");
	graph::test_dfs_visitor testVisitor;
	graph.dfs(testVisitor/*, nodesDescriptor[n1]*/);

	TCOUT("__________________________________________________");
	TCOUT("graphT:");
	graphT.dfs(testVisitor/*, nodesDescriptor[n1]*/);
}

BOOST_AUTO_TEST_SUITE_END()

