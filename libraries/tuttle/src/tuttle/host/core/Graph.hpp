#ifndef _TUTTLE_HOST_GRAPH_HPP_
#define _TUTTLE_HOST_GRAPH_HPP_

#include "Core.hpp"
#include <tuttle/host/ofx/ofxhAttribute.h>
#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/Vertex.hpp>
#include <tuttle/host/graph/Edge.hpp>

#include <stdexcept>
#include <string>
#include <map>

namespace tuttle {
namespace host {
namespace core {

typedef unsigned int NodeID;

class Graph
{
public:
	Graph();
	~Graph() {}

	NodeID createNode( const std::string& id ) throw( std::logic_error );
	void   deleteNode( const EffectInstance& node );

	void connect( const NodeID& out, const NodeID& in );
	//		void connect( const EffectInstance& out, const ofx::attribute::AttributeInstance& outAttr, const EffectInstance& in, const ofx::attribute::AttributeInstance& inAttr );
	//		void connect( const EffectInstance& out, const EffectInstance& in, const AttributeInstance& inAttr ) { connectNodes( out, out.getOutput(), in, inAttr ); }
	//		void connect( const EffectInstance& out, const EffectInstance& in ) { connectNodes( out, out.getOutput(), in, in.getInput(0) ); }
	void dumpToStdOut();
	void compute();

private:
	typedef graph::InternalGraph<graph::Vertex, graph::Edge> internalGraph_t;
	internalGraph_t _graph;
	std::map<NodeID, internalGraph_t::VertexDescriptor> _nodes;
	std::map<std::string, int> _instanceCount; ///< used to assign a unique name to each node

private:
	void addToGraph( EffectInstance& node );
	NodeID _nodeCount;
};

}
}
}

#endif

