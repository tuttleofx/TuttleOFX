#ifndef _TUTTLE_HOST_PROCESSGRAPH_HPP_
#define _TUTTLE_HOST_PROCESSGRAPH_HPP_

#include "Graph.hpp"

namespace tuttle {
namespace host {
namespace core {

class ProcessGraph
{
public:
	typedef Graph::Node Node; /// @todo tuttle ProcessNode...
	typedef Graph::Vertex Vertex;
	typedef Graph::Edge Edge;
	typedef Graph::Attribute Attribute;
	typedef Graph::InternalGraphImpl InternalGraphImpl;
	typedef Graph::Descriptor Descriptor;
	typedef Graph::NodeMap NodeMap;
	typedef Graph::InstanceCountMap InstanceCountMap;
public:
	ProcessGraph( Graph& graph, const std::list<std::string>& nodes );
	~ProcessGraph();

	void process( const int tBegin, const int tEnd );

private:
	void relink();

	InternalGraphImpl _graph;
	NodeMap _nodes;
	InstanceCountMap _instanceCount;

	Descriptor _output;
};

}
}
}

#endif

