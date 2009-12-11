#ifndef _TUTTLE_HOST_GRAPH_HPP_
#define _TUTTLE_HOST_GRAPH_HPP_

#include "Core.hpp"
#include "Exception.hpp"
#include <tuttle/host/ofx/OfxhAttribute.hpp>
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
	typedef graph::InternalGraph<graph::Vertex, graph::Edge> InternalGraph;
	typedef boost::ptr_map<const std::string, Node> NodeMap;

public:
	Graph();
	Graph( const Graph& other );
	~Graph();

	Node& createNode( const std::string& id ) throw( exception::LogicError );
	void  deleteNode( const EffectInstance& node ) throw( exception::LogicError );

	void connect( const Node& out, const Node& in ) throw( exception::LogicError );
	void connect( const EffectInstance& out, const EffectInstance& in, const ofx::attribute::AttributeInstance& inAttr ) throw( exception::LogicError );
	void unconnectNode( const EffectInstance& node ) throw( exception::LogicError );

	void compute( const std::list<std::string>& nodes, const int first, const int last );

	const InternalGraph& getGraph() const { return _graph; }

public:
	const Node& getNode( const std::string& id ) const { return _nodes.at( id ); }
	void        dumpToStdOut();

private:
	InternalGraph _graph;
	NodeMap _nodes;
	std::map<std::string, InternalGraph::VertexDescriptor> _nodesDescriptor;
	std::map<std::string, int> _instanceCount; ///< used to assign a unique name to each node

private:
	void relink();
	void addToGraph( EffectInstance& node );
	void removeFromGraph( EffectInstance& node ) throw( exception::LogicError );
};

}
}
}

#endif

