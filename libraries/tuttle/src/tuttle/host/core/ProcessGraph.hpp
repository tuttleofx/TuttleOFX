#ifndef _TUTTLE_HOST_PROCESSGRAPH_HPP_
#define _TUTTLE_HOST_PROCESSGRAPH_HPP_

#include "Graph.hpp"

namespace tuttle {
namespace host {
namespace core {

class ProcessGraph
{
public:
	ProcessGraph(const Graph & graph);
	~ProcessGraph();

	void compute(const std::list<std::string>& nodes, const int t);

private:
	const Graph _graph;
};

}
}
}

#endif

