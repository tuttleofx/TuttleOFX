#ifndef _TUTTLE_HOST_CORE_GRAPH_HPP_
#define _TUTTLE_HOST_CORE_GRAPH_HPP_

#include "Core.hpp"
#include "Exception.hpp"
#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/Vertex.hpp>
#include <tuttle/host/graph/Edge.hpp>
#include <tuttle/host/ofx/attribute/OfxhAttribute.hpp>
#include <tuttle/common/utils/global.hpp>

#include <boost/ptr_container/ptr_map.hpp>

#include <stdexcept>
#include <string>
#include <sstream>
#include <map>
#include <list>

#ifdef SWIG
#define TUTTLE_AUTODOCUMENTATION %feature("autodoc", "1");
#define TUTTLE_AUTODOCUMENTATION_NOTYPES %feature("autodoc", "0");
#define TUTTLE_DOCUMENTATION(doc) %feature("autodoc", doc);
#else
#define TUTTLE_AUTODOCUMENTATION
#define TUTTLE_AUTODOCUMENTATION_NOTYPES
#define TUTTLE_DOCUMENTATION(doc)
#endif

namespace tuttle {
namespace host {
namespace core {

class Graph
{
public:
	typedef core::ImageEffectNode Node; /// @todo tuttle ProcessNode...
	typedef core::ProcessAttribute Attribute;
	typedef graph::InternalGraph<graph::Vertex, graph::Edge> InternalGraph;
	typedef graph::InternalGraph<graph::Vertex, graph::Edge>::VertexDescriptor Descriptor;
	typedef boost::ptr_map<std::string, Node> NodeMap;
	typedef std::map<std::string, int> InstanceCountMap;

public:
	Graph();
	//Graph( const Graph& other );
	~Graph();

	TUTTLE_AUTODOCUMENTATION
	Node& createNode( const std::string& id );
	TUTTLE_AUTODOCUMENTATION
	void  deleteNode( const Node& node );// throw( exception::LogicError );
	TUTTLE_AUTODOCUMENTATION
	void connect( const Node& out, const Node& in );// throw( exception::LogicError );
	TUTTLE_AUTODOCUMENTATION
	void connect( const Node& out, const Attribute& inAttr );// throw( exception::LogicError );
	TUTTLE_AUTODOCUMENTATION
	void unconnectNode( const Node& node );// throw( exception::LogicError );

	TUTTLE_AUTODOCUMENTATION
	void compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd );
	TUTTLE_AUTODOCUMENTATION
	void compute( const std::list<std::string>& nodes, const int time ) { compute( nodes, time, time ); }

	TUTTLE_AUTODOCUMENTATION
	void compute( const std::string& node, const int tBegin, const int tEnd )
	{
		std::list<std::string> outputs;
		outputs.push_back( node );
		compute( outputs, tBegin, tEnd );
	}
	TUTTLE_AUTODOCUMENTATION
	void compute( const std::string& node, const int time )
	{
		compute( node, time, time );
	}

	TUTTLE_AUTODOCUMENTATION
	const InternalGraph&    getGraph() const         { return _graph; }
	TUTTLE_AUTODOCUMENTATION
	const NodeMap&          getNodes() const         { return _nodes; }
	TUTTLE_AUTODOCUMENTATION
	NodeMap&                getNodes()               { return _nodes; }
	TUTTLE_AUTODOCUMENTATION
	std::list<Node*> getNodesByContext( const std::string& type );
	TUTTLE_AUTODOCUMENTATION
	std::list<Node*> getNodesByPlugin( const std::string& pluginId );
//	const Node&          getNode( const std::string& name ) const { return getNodes()[name]; }
	TUTTLE_AUTODOCUMENTATION
	const Node& getNode( const std::string& id ) const { return _nodes.at( id ); }
	TUTTLE_AUTODOCUMENTATION
	Node&                getNode( const std::string& name )       { return getNodes().at(name); }
	TUTTLE_AUTODOCUMENTATION
	const InstanceCountMap& getInstanceCount() const { return _instanceCount; }

public:
	friend std::ostream& operator<<( std::ostream& os, const Graph& g );

#ifdef SWIG
	%extend
	{
		Node& __getitem__( const std::string& name )
		{
			return self->getNode(name);
		}
		std::string __str__() const
		{
			std::stringstream s;
			s << *self;
			return s.str();
		}
	}
#endif


private:
	InternalGraph _graph;
	NodeMap _nodes;
	std::map<std::string, Descriptor> _nodesDescriptor;
	InstanceCountMap _instanceCount; ///< used to assign a unique name to each node

private:
	void addToGraph( Node& node );
	void removeFromGraph( Node& node ) throw( exception::LogicError );
};

}
}
}

#endif

