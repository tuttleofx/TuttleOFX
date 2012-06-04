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
#include <boost/lexical_cast.hpp>
#include <boost/exception/all.hpp>

#include <stdexcept>
#include <string>
#include <sstream>
#include <map>
#include <list>

namespace tuttle {
namespace host {

struct TimeRange
{
	TimeRange( const int frame )
		: _begin( frame )
		, _end( frame )
		, _step( 1 )
	{}
	TimeRange( const int begin, const int end, const int step = 1 )
		: _begin( begin )
		, _end( end )
		, _step( step )
	{}
	TimeRange( const OfxRangeD& range, const int step = 1 )
		: _begin( boost::lexical_cast<int>(range.min) )
		, _end( boost::lexical_cast<int>(range.max) )
		, _step( step )
	{}
	
	int _begin;
	int _end;
	int _step;
};

enum EVerboseLevel
{
	eVerboseLevelNone,
	eVerboseLevelError,
	eVerboseLevelWarning,
	eVerboseLevelDebug
};

struct ComputeOptions
{
	ComputeOptions()
	{
		setDefault();
	}
	explicit
	ComputeOptions( const int frame )
	{
		setDefault();
		_timeRanges.push_back( TimeRange( frame, frame ) );
	}
	ComputeOptions( const int begin, const int end, const int step = 1 )
	{
		setDefault();
		_timeRanges.push_back( TimeRange( begin, end, step ) );
	}
	
	void setDefault()
	{
		_renderScale.x = 1.0;
		_renderScale.y = 1.0;
		_continueOnError = false;
		_returnBuffers = false;
		_verboseLevel = eVerboseLevelError;
		_interactive = false;
		_forceIdentityNodesProcess = false;
	}
	
	std::list<TimeRange> _timeRanges;
	
	OfxPointD _renderScale;
	bool _continueOnError;
	bool _forceIdentityNodesProcess;
	bool _returnBuffers;
	EVerboseLevel _verboseLevel;
	bool _interactive;
};


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
	
	memory::MemoryCache compute( const std::list<std::string>& nodes, const ComputeOptions& options = ComputeOptions() );

	inline memory::MemoryCache compute( const std::list<Node*>& nodes, const ComputeOptions& options = ComputeOptions() )
	{
		std::list<std::string> nodesStr;
		//nodesStr.reserve(nodes.size());
		BOOST_FOREACH( Node * n, nodes )
		{
			nodesStr.push_back( n->getName() );
		}
		return compute( nodesStr, options );
	}

	inline memory::MemoryCache compute( const std::string& node, const ComputeOptions& options = ComputeOptions() )
	{
		std::list<std::string> outputs;
		outputs.push_back( node );
		return compute( outputs, options );
	}

	inline memory::MemoryCache compute( const Node& node, const ComputeOptions& options = ComputeOptions() )
	{
		return compute( node.getName(), options );
	}

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

