// custom host
#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/GraphExporter.hpp>

#include <boost/graph/graph_utility.hpp>

#include <iostream>

#define BOOST_TEST_MODULE graph_tests
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

BOOST_AUTO_TEST_SUITE( internalgraph_tests_suite01 )

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

	TCOUT("__________________________________________________");
}

BOOST_AUTO_TEST_SUITE_END()

