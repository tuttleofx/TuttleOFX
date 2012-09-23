#include "Graph.hpp"
#include "Node.hpp"
#include "graph/ProcessGraph.hpp"

#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/attribute/OfxhClipImage.hpp>
#include <tuttle/host/graph/GraphExporter.hpp>

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
	_nodes.insert( key, &node ); // acquire the ownership
	addToInternalGraph( node );
	
	return node;
}

void Graph::addNodes( const std::vector<NodeInit>& nodes )
{
	BOOST_FOREACH( const NodeInit& node, nodes )
	{
		addNode( node ); // tranfer nodes ownership to the graph
	}
}

void Graph::addConnectedNodes( const std::vector<NodeInit>& nodes )
{
	std::vector<INode*> nodePtrs;
	BOOST_FOREACH( const NodeInit& node, nodes )
	{
		nodePtrs.push_back( &addNode( node ) ); // tranfer nodes ownership to the graph
	}
	connect( nodePtrs );
}

void Graph::renameNode( Graph::Node& node, const std::string& newUniqueName )
{
	// it's the same name, nothing to do.
	if( node.getName() == newUniqueName )
		return;
	
	TUTTLE_TCOUT( "Graph::renameNode: from: " << node.getName() << " -> to: " << newUniqueName );
	{
		// check if newUniqueName is not already in the graph
		NodeMap::iterator itNew = _nodes.find( newUniqueName );
		if( itNew != _nodes.end() )
		{
			BOOST_THROW_EXCEPTION( exception::Value()
				<< exception::user() + "New node name " + quotes(newUniqueName) + " already exists." );
		}
	}
	NodeMap::iterator it = _nodes.find( node.getName() );
	if( it == _nodes.end() )
	{
		BOOST_THROW_EXCEPTION( exception::Value()
			<< exception::user() + "Node " + quotes(node.getName()) + " is not in the graph." );
	}
	
	// warning: loose all connections !!!
	removeFromInternalGraph( node );
	
	// rename the key into the map
	NodeMap::auto_type n = _nodes.release( it );
	n->setName( newUniqueName );
	std::string key( newUniqueName ); // for constness
	_nodes.insert( key, n.release() );
	
	addToInternalGraph( node );
	
	node.setName( newUniqueName );
}

void Graph::addToInternalGraph( Node& node )
{
	//TUTTLE_TCOUT( "Graph::addToInternalGraph: " << node.getName() );
	Vertex v( node.getName(), node );
	_graph.addVertex( v );
}

void Graph::removeFromInternalGraph( Node& node )
{
	//TUTTLE_TCOUT( "Graph::removeFromInternalGraph: " << node.getName() );
	const unsigned int id = _graph.getVertexDescriptor( node.getName() );
	_graph.removeVertex( id );
}

void Graph::deleteNode( Node& node )
{
	NodeMap::iterator it = _nodes.find( node.getName() );
	if( it != _nodes.end() )
	{
		BOOST_THROW_EXCEPTION( exception::Value()
			<< exception::user("Node not found.") );
	}
	removeFromInternalGraph( node );
	_nodes.erase( it ); // will delete the node
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

namespace {
template<class TGraph>
inline void graphConnectClips( TGraph& graph )
{
	BOOST_FOREACH( typename TGraph::edge_descriptor ed, graph.getEdges() )
	{
		typename TGraph::Edge& edge           = graph.instance( ed );
		typename TGraph::Vertex& vertexSource = graph.sourceInstance( ed );
		typename TGraph::Vertex& vertexDest   = graph.targetInstance( ed );

		// TUTTLE_TCOUT( "[connectClips] " << edge );
		// TUTTLE_TCOUT( vertexSource << "->" << vertexDest );
		
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

//void Graph::unconnectNode( const Node& node )
//{}

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

bool Graph::compute( memory::MemoryCache& memoryCache, const ComputeOptions& options )
{
	return compute( memoryCache, NodeListArg(), options );
}

bool Graph::compute( memory::MemoryCache& memoryCache, const NodeListArg& nodes, const ComputeOptions& options )
{
#ifndef TUTTLE_PRODUCTION
	graph::exportAsDOT( "graph.dot", _graph );
#endif
	
	graph::ProcessGraph process( *this, nodes.getNodes() );
	return process.process( memoryCache, options );
}

std::list<Graph::Node*> Graph::getNodesByContext( const std::string& context )
{
	std::list<Node*> selectedNodes;
	for( NodeMap::iterator it = getNodes().begin(), itEnd = getNodes().end();
	     it != itEnd;
	     ++it )
	{
		try
		{
			ImageEffectNode& ie = dynamic_cast<ImageEffectNode&>(*it->second);
			if( ie.getContext() == context )
				selectedNodes.push_back( &ie );
		}
		catch(...)
		{
		}
	}
	return selectedNodes;
}

std::list<Graph::Node*> Graph::getNodesByPlugin( const std::string& pluginId )
{
	std::list<Node*> selectedNodes;
	for( NodeMap::iterator it = getNodes().begin(), itEnd = getNodes().end();
	     it != itEnd;
	     ++it )
	{
		/// @todo tuttle: use INode here !
		ImageEffectNode& ie = dynamic_cast<ImageEffectNode&>(*it->second);

		if( boost::iequals( ie.getPlugin().getIdentifier(), pluginId ) )
			selectedNodes.push_back( &ie );
	}
	return selectedNodes;
}

std::ostream& operator<<( std::ostream& os, const Graph& g )
{
	os << "Graph" << std::endl
	                  << g.getGraph();
	return os;
}

}
}

