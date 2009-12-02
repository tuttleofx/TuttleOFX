#ifndef _TUTTLE_HOST_GRAPH_HPP_
#define _TUTTLE_HOST_GRAPH_HPP_

#include "Core.hpp"
#include "Exception.hpp"
#include <tuttle/host/ofx/ofxhAttribute.h>
#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/Vertex.hpp>
#include <tuttle/host/graph/Edge.hpp>

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
	~Graph();

	Node& createNode( const std::string& id ) throw( std::logic_error );
	void  deleteNode( const EffectInstance& node );

	void connect( const Node& out, const Node& in ) throw( Exception );
	//		void connect( const EffectInstance& out, const ofx::attribute::AttributeInstance& outAttr, const EffectInstance& in, const ofx::attribute::AttributeInstance& inAttr );
	//		void connect( const EffectInstance& out, const EffectInstance& in, const AttributeInstance& inAttr ) { connectNodes( out, out.getOutput(), in, inAttr ); }
	//		void connect( const EffectInstance& out, const EffectInstance& in ) { connectNodes( out, out.getOutput(), in, in.getInput(0) ); }
	void dumpToStdOut();
	void compute();

private:
	typedef graph::InternalGraph<graph::Vertex, graph::Edge> internalGraph_t;
	internalGraph_t _graph;
	std::list<Node*> _nodesList;
	std::map<std::string, Node*> _nodes;
	std::map<std::string, internalGraph_t::VertexDescriptor> _nodesDescriptor;
	std::map<std::string, int> _instanceCount; ///< used to assign a unique name to each node

private:
	void addToGraph( EffectInstance& node );
	void removeFromGraph( EffectInstance& node );
	unsigned int _nodeCount;
};

}
}
}

#endif

