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

class Graph
{
	public:
		Graph();
		~Graph(){}

		EffectInstance* createNode( const std::string& id ) throw(std::logic_error);
		void deleteNode( const EffectInstance& node );

		void connectNodes( const EffectInstance& out, const ofx::attribute::AttributeInstance& outAttr, const EffectInstance& in, const ofx::attribute::AttributeInstance& inAttr );
//		void connectNodes( const EffectInstance& out, const EffectInstance& in, const AttributeInstance& inAttr ) { connectNodes( out, out.getOutput(), in, inAttr ); }
//		void connectNodes( const EffectInstance& out, const EffectInstance& in ) { connectNodes( out, out.getOutput(), in, in.getInput(0) ); }

private:
		graph::InternalGraph<graph::Vertex, graph::Edge> _graph;
		std::map<std::string, int> _instanceCount;

private:
		void addToGraph( EffectInstance& node );
};

}
}
}

#endif

