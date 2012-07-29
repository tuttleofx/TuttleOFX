#ifndef _TUTTLE_HOST_CORE_GRAPH_HPP_
#define _TUTTLE_HOST_CORE_GRAPH_HPP_

#include "ComputeOptions.hpp"
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
#include <boost/lexical_cast.hpp>
#include <boost/exception/all.hpp>

#include <stdexcept>
#include <string>
#include <sstream>
#include <map>
#include <list>

namespace tuttle {
namespace host {

/**
 * @brief An utility class to use as function argument. It allows to create a list of nodes from multiple inputs.
 * 
 * All constructors are not "explicit", so we could automatically convert the
 * input inside when we use it as a function argument.
 */
class NodeListArg
{
public:
	NodeListArg()
	{}
	NodeListArg( const std::list<std::string>& nodes )
	: _nodes( nodes )
	{}
	NodeListArg( const std::list<INode*>& nodes )
	{
		BOOST_FOREACH( INode * n, nodes )
		{
			_nodes.push_back( n->getName() );
		}
	}
	NodeListArg( const std::string& node )
	{
		_nodes.push_back( node );
	}
	NodeListArg( const INode& node )
	{
		_nodes.push_back( node.getName() );
	}

public:
	const std::list<std::string>& getNodes() const { return _nodes; }

private:
	std::list<std::string> _nodes;
};


/**
 * @brief A user graph to manipulate OpenFX nodes.
 */
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
	 * @param id is the plugin unique string identification (e.g. "tuttle.blur").
	 */
	Node& createNode( const std::string& id );

	/**
	 * @brief Rename a node in the current graph.
	 * @param newUniqueName is the new unique node name.
	 * 
	 * @warning you will loose all connections.
	 */
	void renameNode( Node& node, const std::string& newUniqueName );
	
	/**
	 * @brief Delete a node from the current graph.
	 * This will remove all the connections.
	 */
	void deleteNode( Node& node );
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

	/**
	 * @brief Temporary solution ! Prepare the user graph, so we can call getTimeDomain (and maybe others functions) on nodes.
	 */
	void init();
	
	/**
	 * @brief Shortcut without buffer results.
	 */
	void compute( const NodeListArg& nodes = NodeListArg(), const ComputeOptions& options = ComputeOptions() );
	
	void compute( memory::MemoryCache& memoryCache, const NodeListArg& nodes = NodeListArg(), const ComputeOptions& options = ComputeOptions() );
	
private:
	void privateCompute( memory::MemoryCache& memoryCache, const NodeListArg& nodes, const ComputeOptions& options = ComputeOptions() );
	static void privateStaticCompute( Graph& graph, memory::MemoryCache& memoryCache, const NodeListArg& nodes, const ComputeOptions& options );
	
public:
	inline const InternalGraphImpl& getGraph() const { return _graph; }
	inline const NodeMap&           getNodes() const { return _nodes; }
	inline NodeMap&                 getNodes()       { return _nodes; }
	std::list<Node*>         getNodesByContext( const std::string& type );
	std::list<Node*>         getNodesByPlugin( const std::string& pluginId );
	//	const Node&          getNode( const std::string& name ) const { return getNodes()[name]; }
	inline const Node&             getNode( const std::string& name ) const { return _nodes.at( name ); }
	inline Node&                   getNode( const std::string& name )     { return getNodes().at( name ); }
	inline const InstanceCountMap& getInstanceCount() const               { return _instanceCount; }

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
	void addToInternalGraph( Node& node );
	void removeFromInternalGraph( Node& node );
};

}
}

#endif

