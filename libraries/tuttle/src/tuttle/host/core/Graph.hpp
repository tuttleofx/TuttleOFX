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
#include <boost/exception/all.hpp>

#include <stdexcept>
#include <string>
#include <sstream>
#include <map>
#include <list>


namespace tuttle {
namespace host {
namespace core {

class Graph
{
public:
	typedef graph::Vertex Vertex;
	typedef graph::Edge Edge;
	typedef core::ImageEffectNode Node; /// @todo tuttle ProcessNode...
	typedef core::ProcessAttribute Attribute;
	typedef graph::InternalGraph<Vertex, Edge> InternalGraphImpl;
	typedef graph::InternalGraph<Vertex, Edge>::VertexDescriptor Descriptor;
	typedef boost::ptr_map<std::string, Node> NodeMap;
	typedef std::map<std::string, int> InstanceCountMap;

public:
	Graph();
	//Graph( const Graph& other );
	~Graph();

	/**
	 * @brief Create a new node in the current graph.
	 * @param id is the plugin unique string identification (e.g. "fr.tuttle.blur").
	 */
	Node& createNode( const std::string& id );
	/**
	 * @brief Delete a node from the current graph.
	 * This will remove all the connections.
	 */
	void  deleteNode( const Node& node );// throw( exception::LogicError );
	/**
	 * @brief Connect nodes (using there unique name in this graph).
	 */
	void connect( const std::string& outNode, const std::string& inNode, const std::string& inAttr = kOfxSimpleSourceAttributeName ); //throw( exception::LogicError )
	/**
	 * @brief Connect nodes the list of nodes linearly.
	 */
	void connect( const std::list<std::string>& nodes );
	void connect( const Node& outNode, const Node& inNode );// throw( exception::LogicError );
	void connect( const std::list<Node*>& nodes );// throw( exception::LogicError );
	void connect( const std::vector<Node*>& nodes );// throw( exception::LogicError );
	void connect( const Node& outNode, const Attribute& inAttr );// throw( exception::LogicError );
//	void unconnectNode( const Node& node );// throw( exception::LogicError );

	void compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd );
	void compute( const std::list<std::string>& nodes, const int time ) { compute( nodes, time, time ); }

	void compute( const std::list<Node*>& nodes, const int tBegin, const int tEnd )
	{
		std::list<std::string> nodesStr;
		//nodesStr.reserve(nodes.size());
		BOOST_FOREACH( Node* n, nodes )
		{
			nodesStr.push_back( n->getName() );
		}
		compute( nodesStr, tBegin, tEnd );
	}
	void compute( const std::list<Node*>& nodes, const int time ) { compute( nodes, time, time ); }

	void compute( const std::string& node, const int tBegin, const int tEnd )
	{
		std::list<std::string> outputs;
		outputs.push_back( node );
		compute( outputs, tBegin, tEnd );
	}
	void compute( const Node& node, const int tBegin, const int tEnd )
	{
		compute( node.getName(), tBegin, tEnd );
	}
	void compute( const std::string& node, const int time )
	{
		compute( node, time, time );
	}
	void compute( const Node& node, const int time )
	{
		compute( node.getName(), time );
	}

	const InternalGraphImpl&    getGraph() const         { return _graph; }
	const NodeMap&          getNodes() const         { return _nodes; }
	NodeMap&                getNodes()               { return _nodes; }
	std::list<Node*> getNodesByContext( const std::string& type );
	std::list<Node*> getNodesByPlugin( const std::string& pluginId );
//	const Node&          getNode( const std::string& name ) const { return getNodes()[name]; }
	const Node& getNode( const std::string& id ) const { return _nodes.at( id ); }
	Node&                getNode( const std::string& name )       { return getNodes().at(name); }
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
	InternalGraphImpl _graph;
	NodeMap _nodes;
	InstanceCountMap _instanceCount; ///< used to assign a unique name to each node

private:
	void addToGraph( Node& node );
	void removeFromGraph( Node& node ) throw( exception::LogicError );
};

}
}
}

#endif

