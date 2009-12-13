#ifndef _TUTTLE_HOST_PROCESSGRAPH_HPP_
#define _TUTTLE_HOST_PROCESSGRAPH_HPP_

#include "Graph.hpp"

namespace tuttle {
namespace host {
namespace core {

class ProcessGraph
{
public:
	ProcessGraph( const Graph& graph );
	~ProcessGraph();

	void compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd );

private:
	void relink();

	Graph::InternalGraph _graph;
	Graph::NodeMap _nodes;
	std::map<std::string, int> _instanceCount;

};

}
}
}

#endif

