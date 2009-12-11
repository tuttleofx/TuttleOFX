#include "ProcessGraph.hpp"
#include <tuttle/host/graph/GraphExporter.hpp>

namespace tuttle {
namespace host {
namespace core {

ProcessGraph::ProcessGraph( const Graph& graph )
	: _graph( graph )
{}

ProcessGraph::~ProcessGraph()
{}

void ProcessGraph::compute( const std::list<std::string>& nodes, const int t )
{
	Graph::InternalGraph optimizedGraph = _graph.getGraph();

	optimizedGraph.toDominatorTree();
	optimizedGraph.test_dfs();

	graph::GraphExporter<graph::Vertex, graph::Edge>::exportAsDOT( optimizedGraph, "subcopy.dot" );
}

}
}
}

