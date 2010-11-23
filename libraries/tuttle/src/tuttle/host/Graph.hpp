#ifndef _TUTTLE_HOST_CORE_GRAPH_HPP_
#define _TUTTLE_HOST_CORE_GRAPH_HPP_

#include "Core.hpp"
#include "INode.hpp"
#include "exceptions.hpp"
#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/UVertex.hpp>
#include <tuttle/host/graph/UEdge.hpp>
#include <tuttle/host/attribute/Attribute.hpp>
#include <tuttle/host/memory/MemoryCache.hpp>
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

class Graph
{
public:
	typedef graph::UVertex Vertex;
	typedef graph::UEdge Edge;
	typedef INode Node; /// @todo tuttle INode...
	typedef attribute::Attribute Attribute;
	typedef graph::InternalGraph<Vertex, Edge> InternalGraphImpl;
	typedef InternalGraphImpl::vertex_descriptor vertex_descriptor;
	typedef InternalGraphImpl::edge_descriptor edge_descriptor;

	typedef std::map<std::string, int> InstanceCountMap;
	typedef boost::ptr_map<std::string, Node> NodeMap;
	
public:
	Graph();
	//Graph( const Graph& other );
	~Graph();

	/**
	 * @brief Create a new input node in the current graph,
	 *        to give an input buffer.
	 */
	InputBufferNode& createInputBuffer();

	/**
	 * @brief Create a new node in the current graph.
	 * @param id is the plugin unique string identification (e.g. "fr.tuttle.blur").
	 */
	Node& createNode( const std::string& id );
	/**
	 * @brief Delete a node from the current graph.
	 * This will remove all the connections.
	 */
	void deleteNode( const Node& node );
	/**
	 * @brief Connect nodes (using there unique name in this graph).
	 */
	void connect( const std::string& outNode, const std::string& inNode, const std::string& inAttr = kOfxSimpleSourceAttributeName );
	/**
	 * @brief Connect nodes the list of nodes linearly.
	 */
	void connect( const std::list<std::string>& nodes );
	void connect( const Node& outNode, const Node& inNode );
	void connect( const std::list<Node*>& nodes );
	void connect( const std::vector<Node*>& nodes );
	void connect( const Node& outNode, const Attribute& inAttr );
	//	void unconnectNode( const Node& node );

	memory::MemoryCache compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd );
	memory::MemoryCache compute( const std::list<std::string>& nodes, const int time ) { return compute( nodes, time, time ); }

	memory::MemoryCache compute( const std::list<Node*>& nodes, const int tBegin, const int tEnd )
	{
		std::list<std::string> nodesStr;
		//nodesStr.reserve(nodes.size());
		BOOST_FOREACH( Node * n, nodes )
		{
			nodesStr.push_back( n->getName() );
		}
		return compute( nodesStr, tBegin, tEnd );
	}

	memory::MemoryCache compute( const std::list<Node*>& nodes, const int time ) { return compute( nodes, time, time ); }

	memory::MemoryCache compute( const std::string& node, const int tBegin, const int tEnd )
	{
		std::list<std::string> outputs;
		outputs.push_back( node );
		return compute( outputs, tBegin, tEnd );
	}

	memory::MemoryCache compute( const Node& node, const int tBegin, const int tEnd )
	{
		return compute( node.getName(), tBegin, tEnd );
	}

	memory::MemoryCache compute( const std::string& node, const int time )
	{
		return compute( node, time, time );
	}

	memory::MemoryCache compute( const Node& node, const int time )
	{
		return compute( node.getName(), time );
	}

	const InternalGraphImpl& getGraph() const { return _graph; }
	const NodeMap&           getNodes() const { return _nodes; }
	NodeMap&                 getNodes()       { return _nodes; }
	std::list<Node*>         getNodesByContext( const std::string& type );
	std::list<Node*>         getNodesByPlugin( const std::string& pluginId );
	//	const Node&          getNode( const std::string& name ) const { return getNodes()[name]; }
	const Node&             getNode( const std::string& id ) const { return _nodes.at( id ); }
	Node&                   getNode( const std::string& name )     { return getNodes().at( name ); }
	const InstanceCountMap& getInstanceCount() const               { return _instanceCount; }

public:
	#ifndef SWIG
	friend std::ostream& operator<<( std::ostream& os, const Graph& g );
	#endif

	#ifdef SWIG
	%extend
	{
		Node& __getitem__( const std::string& name )
		{
			return self->getNode( name );
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
	void removeFromGraph( Node& node );
};

}
}

#endif

