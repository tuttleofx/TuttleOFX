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
//	if( !node )
//		BOOST_THROW_EXCEPTION( exception::Logic()
//		    << exception::user( "Plugin not found (" + id + ")." ) );

	std::stringstream uniqueName;
	uniqueName << node->getLabel() << ++_instanceCount[node->getLabel()];
	node->setName( uniqueName.str() );

	std::string key( node->getName() ); // for constness
	_nodes.insert( key, node );
	addToGraph( *node );

	return *node;
}

Graph::Node& Graph::createNode( const std::string& id )
{
	ofx::imageEffect::OfxhImageEffectPlugin* plug = Core::instance().getImageEffectPluginById( id );

	if( !plug )
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Plugin not found." )
		    << exception::pluginIdentifier( id ) );

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
	else if( plug->supportsContext( kOfxImageEffectContextGenerator ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextGenerator );
	}
	else if( plug->supportsContext( kOfxImageEffectContextFilter ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextFilter );
	}
	else if( plug->supportsContext( kOfxImageEffectContextGeneral ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextGeneral );
	}
	else
	{
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Plugin contexts not supported by the host. (" + id + ")" ) );
	}

	if( !plugInst )
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Plugin not found. plugInst (" + id + ")" ) );
	ImageEffectNode* node = dynamic_cast<ImageEffectNode*>( plugInst );
	if( !node )
		BOOST_THROW_EXCEPTION( exception::Logic()
		    << exception::user( "Plugin not found (" + id + ")." ) );

	std::stringstream uniqueName;
	uniqueName << node->getLabel() << ++_instanceCount[node->getLabel()];
	node->setName( uniqueName.str() );

	std::string key( node->getName() ); // for constness
	_nodes.insert( key, node );
	addToGraph( *node );

	return *node;
}

void Graph::addToGraph( Node& node )
{
	Vertex v( node.getName(), node );

	TUTTLE_COUT_DEBUG( node.getName() );

	_graph.addVertex( v );
}

void Graph::removeFromGraph( Node& node )
{
	//	graph::Vertex v( node.getName(), &node );
	//
	//	_nodesList.find( &node );
	//	_nodes[node.getName()] = node;
	//	_graph.addVertex( v );
}

void Graph::deleteNode( const Node& node )
{}

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

//void Graph::unconnectNode( const Node& node )
//{}

memory::MemoryCache Graph::compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd )
{
#ifndef TUTTLE_PRODUCTION
	graph::exportAsDOT( "graph.dot", _graph );
#endif
	
	graph::ProcessGraph process( *this, nodes );
	return process.process( tBegin, tEnd );
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

