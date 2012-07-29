#include "Graph.hpp"
#include "InputBufferNode.hpp"
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

InputBufferNode& Graph::createInputBuffer()
{
	InputBufferNode* node = new InputBufferNode();

	std::stringstream uniqueName;
	uniqueName << node->getLabel() << ++_instanceCount[node->getLabel()];
	node->setName( uniqueName.str() );

	std::string key( node->getName() ); // for constness
	_nodes.insert( key, node );
	addToInternalGraph( *node );

	return *node;
}

Graph::Node& Graph::createNode( const std::string& id )
{
	ofx::imageEffect::OfxhImageEffectPlugin* plug = Core::instance().getImageEffectPluginById( id );

	if( !plug )
	{
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Plugin not found." )
		    << exception::pluginIdentifier( id ) );
	}

	plug->loadAndDescribeActions();

	ofx::imageEffect::OfxhImageEffectNode* plugInst = NULL;
	if( plug->supportsContext( kOfxImageEffectContextReader ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextReader );
	}
	else if( plug->supportsContext( kOfxImageEffectContextWriter ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextWriter );
	}
	else if( plug->supportsContext( kOfxImageEffectContextGeneral ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextGeneral );
	}
	else if( plug->supportsContext( kOfxImageEffectContextGenerator ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextGenerator );
	}
	else if( plug->supportsContext( kOfxImageEffectContextFilter ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextFilter );
	}
	else
	{
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Plugin contexts not supported by the host. (" + id + ")" ) );
	}

	if( !plugInst )
	{
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Plugin not found. plugInst (" + id + ")" ) );
	}
	ImageEffectNode* node = dynamic_cast<ImageEffectNode*>( plugInst );
	if( !node )
	{
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Plugin not found (" + id + ")." ) );
	}

	std::stringstream uniqueName;
	uniqueName << node->getLabel() << "_" << ++_instanceCount[node->getLabel()];
	node->setName( uniqueName.str() );

	std::string key( node->getName() ); // for constness
	_nodes.insert( key, node );
	addToInternalGraph( *node );

	return *node;
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
	_graph.connect( outNode, inNode, kOfxSimpleSourceAttributeName );
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

// shortcut
void Graph::compute( const NodeListArg& nodes, const ComputeOptions& options )
{
	ComputeOptions realOptions( options );
	realOptions.setReturnBuffers( false );
	
	memory::MemoryCache emptyMemoryCache;
	privateCompute( emptyMemoryCache, nodes, options );
}

void Graph::compute( memory::MemoryCache& memoryCache, const NodeListArg& nodes, const ComputeOptions& options )
{
	privateCompute( memoryCache, nodes, options );
}

void Graph::privateCompute( memory::MemoryCache& memoryCache, const NodeListArg& nodes, const ComputeOptions& options )
{
#ifndef TUTTLE_PRODUCTION
	graph::exportAsDOT( "graph.dot", _graph );
#endif
	
	graph::ProcessGraph process( *this, nodes.getNodes() );
	process.process( memoryCache, options );
}

void Graph::privateStaticCompute( Graph& graph, memory::MemoryCache& memoryCache, const NodeListArg& nodes, const ComputeOptions& options )
{
	graph.privateCompute( memoryCache, nodes, options );
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

