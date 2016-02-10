#include "Graph.hpp"
#include "Node.hpp"
#include "graph/ProcessGraph.hpp"

#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/attribute/OfxhClipImage.hpp>
#include <tuttle/host/graph/GraphExporter.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/exception/all.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <sstream>

namespace tuttle {
namespace host {

Graph::Graph()
{}

Graph::~Graph()
{}

InputBufferWrapper Graph::createInputBuffer()
{
	Node& node = createNode( "tuttle.inputbuffer" );
	InputBufferWrapper nodeWrapper( node );
	
	return nodeWrapper;
}

OutputBufferWrapper Graph::createOutputBuffer()
{
	Node& node = createNode( "tuttle.outputbuffer" );
	OutputBufferWrapper nodeWrapper( node );
	
	return nodeWrapper;
}

Graph::Node& Graph::createNode( const std::string& pluginName )
{
	INode* node = tuttle::host::createNode( pluginName );
	return addNode( *node );
}

Graph::Node& Graph::addNode( const NodeInit& node )
{
	return addNode( node.release() ); // transfer ownership
}

Graph::Node& Graph::addNode( INode& node )
{
	std::stringstream uniqueName;
	uniqueName << node.getLabel() << "_" << ++_instanceCount[node.getLabel()];
	node.setName( uniqueName.str() );

	std::string key( node.getName() ); // for constness
	_nodesMap.insert( key, &node ); // acquire the ownership
	addToInternalGraph( node );
	
	return node;
}

std::vector<INode*> Graph::addNodes( const std::vector<NodeInit>& nodes )
{
	std::vector<INode*> nodePtrs;
	BOOST_FOREACH( const NodeInit& node, nodes )
	{
		INode& newNode = addNode( node ); // tranfer nodes ownership to the graph
		nodePtrs.push_back( & newNode );
	}
	return nodePtrs;
}

std::vector<INode*> Graph::addConnectedNodes( const std::vector<NodeInit>& nodes )
{
	std::vector<INode*> nodePtrs = addNodes( nodes );
	if( nodePtrs.size() > 1 )
		connect( nodePtrs );
	return nodePtrs;
}

void Graph::renameNode( Graph::Node& node, const std::string& newUniqueName )
{
	// it's the same name, nothing to do.
	if( node.getName() == newUniqueName )
		return;
	
	TUTTLE_LOG_INFO( "Graph::renameNode: from: " << node.getName() << " -> to: " << newUniqueName );
	{
		// check if newUniqueName is not already in the graph
		NodeMap::iterator itNew = _nodesMap.find( newUniqueName );
		if( itNew != _nodesMap.end() )
		{
			BOOST_THROW_EXCEPTION( exception::Value()
				<< exception::user() + "New node name " + quotes(newUniqueName) + " already exists." );
		}
	}
	NodeMap::iterator it = _nodesMap.find( node.getName() );
	if( it == _nodesMap.end() )
	{
		BOOST_THROW_EXCEPTION( exception::Value()
			<< exception::user() + "Node " + quotes(node.getName()) + " is not in the graph." );
	}
	
	// warning: loose all connections !!!
	removeFromInternalGraph( node );
	
	// rename the key into the map
	NodeMap::auto_type n = _nodesMap.release( it );
	n->setName( newUniqueName );
	std::string key( newUniqueName ); // for constness
	_nodesMap.insert( key, n.release() );
	
	addToInternalGraph( node );
	
	node.setName( newUniqueName );
}

void Graph::addToInternalGraph( Node& node )
{
	//TUTTLE_LOG_INFO( "Graph::addToInternalGraph: " << node.getName() );
	Vertex v( node.getName(), node );
	_graph.addVertex( v );
}

void Graph::removeFromInternalGraph( Node& node )
{
	//TUTTLE_LOG_INFO( "Graph::removeFromInternalGraph: " << node.getName() );
	const unsigned int id = _graph.getVertexDescriptor( node.getName() );
	_graph.removeVertex( id );
}

void Graph::deleteNode( Node& node )
{
	NodeMap::iterator it = _nodesMap.find( node.getName() );
	if( it == _nodesMap.end() )
	{
		BOOST_THROW_EXCEPTION( exception::Value()
			<< exception::user("Node not found.") );
	}
	removeFromInternalGraph( node );
	_nodesMap.erase( it ); // will delete the node
}

std::size_t Graph::deleteUnconnectedNodes( const Node& node )
{
	std::vector<vertex_descriptor> toRemove = _graph.getUnconnectedVertices( _graph.getVertexDescriptor( node.getName() ) );
	BOOST_FOREACH( const vertex_descriptor nodeId, toRemove )
	{
		deleteNode( _graph.instance( nodeId ).getProcessNode() );
	}
	return toRemove.size();
}

void Graph::clear()
{
	_graph.clear();
	_nodesMap.clear();
	_instanceCount.clear();
}

void Graph::connect( const std::string& outNode, const std::string& inNode, const std::string& inAttr )
{
	connect( getNode(outNode), getNode(inNode).getAttribute(inAttr) );
}

void Graph::connect( const std::list<std::string>& nodes )
{
	typedef std::list<std::string>::const_iterator ConstIterator;
	if( nodes.size() <= 1 )
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Needs multiple nodes to connect them together." ) );

	ConstIterator itA = nodes.begin(), itB = itA;
	++itB;
	ConstIterator itEnd = nodes.end();
	for( ;
	     itB != itEnd;
	     ++itA, ++itB )
	{
		this->connect( *itA, *itB );
	}
}

void Graph::connect( const Node& outNode, const Node& inNode )
{
	connect( outNode, inNode.getSingleInputAttribute() );
}

void Graph::connect( const std::list<Node*>& nodes )
{
	typedef std::list<Node*>::const_iterator ConstIterator;
	if( nodes.size() <= 1 )
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Needs multiple nodes to connect them together." ) );

	ConstIterator itA = nodes.begin(), itB = itA;
	++itB;
	ConstIterator itEnd = nodes.end();
	for( ;
	     itB != itEnd;
	     ++itA, ++itB )
	{
		this->connect( **itA, **itB );
	}
}

void Graph::connect( const std::vector<Node*>& nodes )
{
	typedef std::vector<Node*>::const_iterator ConstIterator;
	if( nodes.size() <= 1 )
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Needs multiple clips to connect them together." ) );

	ConstIterator itA = nodes.begin(), itB = itA;
	++itB;
	ConstIterator itEnd = nodes.end();
	for( ;
	     itB != itEnd;
	     ++itA, ++itB )
	{
		BOOST_ASSERT( *itA != NULL );
		BOOST_ASSERT( *itB != NULL );
		
		this->connect( **itA, **itB );
	}
}

void Graph::connect( const Node& outNode, const Attribute& inAttr )
{
	_graph.connect( outNode.getName(), inAttr.getNode().getName(), inAttr.getName() );
}

void Graph::connect( const Attribute& outAttr, const Attribute& inAttr )
{
	_graph.connect( outAttr.getNode().getName(), inAttr.getNode().getName(), inAttr.getName() );
}

void Graph::unconnect( const Attribute& outAttr, const Attribute& inAttr )
{
	_graph.unconnect( outAttr.getNode().getName(), inAttr.getNode().getName(), inAttr.getName() );
}

namespace {
template<class TGraph>
inline void graphConnectClips( TGraph& graph )
{
	BOOST_FOREACH( typename TGraph::edge_descriptor ed, graph.getEdges() )
	{
		typename TGraph::Edge& edge           = graph.instance( ed );
		typename TGraph::Vertex& vertexSource = graph.sourceInstance( ed );
		typename TGraph::Vertex& vertexDest   = graph.targetInstance( ed );

		//TUTTLE_LOG_INFO( "[connectClips] " << edge );
		//TUTTLE_LOG_INFO( vertexSource << "->" << vertexDest );
		
		if( ! vertexDest.isFake() && ! vertexSource.isFake() )
		{
			INode& sourceNode = vertexSource.getProcessNode();
			INode& targetNode = vertexDest.getProcessNode();
			targetNode.connect( sourceNode, targetNode.getAttribute( edge.getInAttrName() ) );
		}
	}
}
}

void Graph::init()
{
	graphConnectClips<InternalGraphImpl>( _graph );
}

void Graph::unconnect( const Node& node )
{
	_graph.clearVertex( _graph.getVertexDescriptor(node.getName()) );
}

void Graph::replaceNodeConnections( const Node& fromNode, const Node& toNode )
{
	BOOST_FOREACH( edge_descriptor e, _graph.getInEdges( _graph.getVertexDescriptor( fromNode.getName() ) ) )
	{
#ifdef DEBUG
		// The current node should have the declared input attribute.
		_graph.targetInstance(e).getProcessNode().getAttribute( _graph.instance(e).getInAttrName() );
#endif
		// Check that the new node has all needed attributes, before to start to modify the graph.
		toNode.getAttribute( _graph.instance(e).getInAttrName() );
	}
	BOOST_FOREACH( edge_descriptor e, _graph.getInEdges( _graph.getVertexDescriptor( fromNode.getName() ) ) )
	{
		// fromNode == targetInstance
		// So replace targetInstance with toNode
		connect(
			_graph.sourceInstance(e).getProcessNode(),
			toNode.getAttribute( _graph.instance(e).getInAttrName() ) );
	}
	BOOST_FOREACH( edge_descriptor e, _graph.getOutEdges( _graph.getVertexDescriptor( fromNode.getName() ) ) )
	{
		// fromNode == sourceInstance
		// So replace sourceInstance with toNode
		connect(
			toNode,
			_graph.targetInstance(e).getProcessNode().getAttribute( _graph.instance(e).getInAttrName() ) );
	}
	unconnect( fromNode );
}

std::size_t Graph::getNbInputConnections( const Node& node ) const
{
	return _graph.getInDegree( _graph.getVertexDescriptor( node.getName() ) );
}

std::size_t Graph::getNbOutputConnections( const Node& node ) const
{
	return _graph.getOutDegree( _graph.getVertexDescriptor( node.getName() ) );
}

void Graph::setup()
{
	const ComputeOptions options;
	const std::list<std::string> outputNodes;
	graph::ProcessGraph procGraph( options, *this, outputNodes, core().getMemoryCache() );
	procGraph.setup();
}

void Graph::setupAtTime( const OfxTime time, const NodeListArg& outputNodes )
{
	const ComputeOptions options;
	graph::ProcessGraph procGraph( options, *this, outputNodes.getNodes(), core().getMemoryCache() );
	procGraph.setupAtTime( time );
}

void Graph::computeGlobalHashAtTime( NodeHashContainer& outNodesHash, const OfxTime time, const NodeListArg& outputNodes )
{
	const ComputeOptions options;
	graph::ProcessGraph procGraph( options, *this, outputNodes.getNodes(), core().getMemoryCache() );
	procGraph.setup();
	procGraph.setupAtTime( time );
	procGraph.computeHashAtTime( outNodesHash, time );
}

bool Graph::compute( const ComputeOptions& options )
{
	return compute( NodeListArg(), options );
}

bool Graph::compute( const NodeListArg& nodes, const ComputeOptions& options )
{
	const_cast<ComputeOptions&>(options).setReturnBuffers( false );
	
	memory::MemoryCache emptyMemoryCache;
	return compute( emptyMemoryCache, nodes, options );
}

bool Graph::compute( memory::IMemoryCache& memoryCache, const ComputeOptions& options )
{
	return compute( memoryCache, NodeListArg(), options );
}

bool Graph::compute( memory::IMemoryCache& memoryCache, const NodeListArg& nodes, const ComputeOptions& options )
{
	return compute( memoryCache, nodes, options, core().getMemoryCache() );
}

bool Graph::compute( memory::IMemoryCache& memoryCache, const NodeListArg& nodes,
		const ComputeOptions& options, memory::IMemoryCache& internMemoryCache )
{
#if(TUTTLE_EXPORT_PROCESSGRAPH_DOT)
	graph::exportAsDOT( "graph.dot", _graph );
#endif

	graph::ProcessGraph procGraph( options, *this, nodes.getNodes(), internMemoryCache );
	return procGraph.process( memoryCache );
}

std::vector<const Graph::Node*> Graph::getNodes() const
{
	std::vector<const Graph::Node*> nodes;
	nodes.reserve( getNodesMap().size() );
	for( NodeMap::const_iterator it = getNodesMap().cbegin(), itEnd = getNodesMap().cend();
	     it != itEnd;
	     ++it )
	{
		nodes.push_back( it->second );
	}
	return nodes;
}

std::vector<Graph::Node*> Graph::getNodes()
{
	std::vector<Graph::Node*> nodes;
	nodes.reserve( getNodesMap().size() );
	for( NodeMap::iterator it = getNodesMap().begin(), itEnd = getNodesMap().end();
	     it != itEnd;
	     ++it )
	{
		nodes.push_back( it->second );
	}
	return nodes;
}

std::vector<Graph::Node*> Graph::getConnectedNodes( const Node& node )
{
	std::vector<Node*> connectedNodes;
	std::vector<vertex_descriptor> toRemove = _graph.getConnectedVertices( _graph.getVertexDescriptor( node.getName() ) );
	connectedNodes.reserve( toRemove.size() );
	
	BOOST_FOREACH( const vertex_descriptor nodeId, toRemove )
	{
		connectedNodes.push_back( &_graph.instance( nodeId ).getProcessNode() );
	}
	
	return connectedNodes;
}

std::vector<Graph::Node*> Graph::getUnconnectedNodes( const Node& node )
{
	std::vector<Node*> unconnectedNodes;
	std::vector<vertex_descriptor> toRemove = _graph.getUnconnectedVertices( _graph.getVertexDescriptor( node.getName() ) );
	unconnectedNodes.reserve( toRemove.size() );
	
	BOOST_FOREACH( const vertex_descriptor nodeId, toRemove )
	{
		unconnectedNodes.push_back( &_graph.instance( nodeId ).getProcessNode() );
	}
	
	return unconnectedNodes;
}

std::vector<Graph::Node*> Graph::getNodesByContext( const std::string& context )
{
	std::vector<Node*> selectedNodes;
	for( NodeMap::iterator it = getNodesMap().begin(), itEnd = getNodesMap().end();
	     it != itEnd;
	     ++it )
	{
		try
		{
			/// @todo tuttle: use INode here !
			ImageEffectNode& ie = it->second->asImageEffectNode();
			
			if( ie.getContext() == context )
				selectedNodes.push_back( &ie );
		}
		catch(...)
		{
		}
	}
	return selectedNodes;
}

std::vector<Graph::Node*> Graph::getNodesByPlugin( const std::string& pluginId )
{
	std::vector<Node*> selectedNodes;
	for( NodeMap::iterator it = getNodesMap().begin(), itEnd = getNodesMap().end();
	     it != itEnd;
	     ++it )
	{
		try
		{
			/// @todo tuttle: use INode here !
			ImageEffectNode& ie = it->second->asImageEffectNode();

			if( boost::iequals( ie.getPlugin().getIdentifier(), pluginId ) )
				selectedNodes.push_back( &ie );
		}
		catch(...)
		{
		}
	}
	return selectedNodes;
}

void Graph::exportDot( const std::string& filename, const EDotExportLevel level ) const
{
	switch( level )
	{
		case eDotExportLevelSimple:
			graph::exportAsDOT( filename, _graph );
			break;
		case eDotExportLevelDetailed:
			graph::exportAsDOT( filename, _graph );
			break;
	}
}

std::ostream& operator<<( std::ostream& os, const Graph& g )
{
	os << "Graph" << std::endl
	                  << g.getGraph();
	return os;
}

}
}

