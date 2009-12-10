#ifndef _TUTTLE_HOST_GRAPH_HPP_
#define _TUTTLE_HOST_GRAPH_HPP_

#include "Core.hpp"
#include "Exception.hpp"
#include <tuttle/host/ofx/ofxhAttribute.h>
#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/Vertex.hpp>
#include <tuttle/host/graph/Edge.hpp>

#include <boost/ptr_container/ptr_map.hpp>

#include <stdexcept>
#include <string>
#include <map>
#include <list>

namespace tuttle {
namespace host {
namespace core {

class Graph
{
public:
	typedef core::EffectInstance Node;

public:
	Graph();
	Graph( const Graph& other);
	~Graph();

	Node& createNode( const std::string& id ) throw( std::logic_error );
	void  deleteNode( const EffectInstance& node );

	void connect( const Node& out, const Node& in ) throw( Exception );
	void connect( const EffectInstance& out, const EffectInstance& in, const ofx::attribute::AttributeInstance& inAttr ) throw( Exception );
	void unconnectNode( const EffectInstance& node );

	void compute();

public:
	const Node& getNode( const std::string& id ) const { return _nodes.at(id); }
	void dumpToStdOut();

private:
	typedef graph::InternalGraph<graph::Vertex, graph::Edge> InternalGraph;
	typedef boost::ptr_map<const std::string, Node> NodeMap;
	InternalGraph _graph;
	NodeMap _nodes;
	std::map<std::string, InternalGraph::VertexDescriptor> _nodesDescriptor;
	std::map<std::string, int> _instanceCount; ///< used to assign a unique name to each node

private:
	void addToGraph( EffectInstance& node );
	void removeFromGraph( EffectInstance& node );
};

}
}
}

#endif

