#include "Graph.hpp"
#include "ProcessGraph.hpp"
#include <tuttle/host/ofx/OfxhClipImage.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/graph/GraphExporter.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <sstream>

namespace tuttle {
namespace host {
namespace core {

Graph::Graph()
{}

Graph::~Graph()
{}

Graph::Node& Graph::createNode( const std::string& id )
{
	ofx::imageEffect::OfxhImageEffectPlugin* plug = Core::instance().getImageEffectPluginById( id );

	if( !plug )
		throw exception::LogicError( "Plugin not found. plug (" + id + ")" );

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
		throw exception::LogicError( "Plugin contexts not supported by the host. (" + id + ")" );
	}

	if( !plugInst )
		throw exception::LogicError( "Plugin not found. plugInst (" + id + ")" );
	ImageEffectNode* node = dynamic_cast<ImageEffectNode*>( plugInst );
	if( !node )
		throw exception::LogicError( "Plugin not found (" + id + ")." );

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
	graph::Vertex v( node.getName(), node );

	COUT_DEBUG(node.getName());
	_nodesDescriptor[node.getName()] = _graph.addVertex( v );
}

void Graph::removeFromGraph( Node& node ) throw( exception::LogicError )
{
	//	graph::Vertex v( node.getName(), &node );
	//
	//	_nodesList.find( &node );
	//	_nodes[node.getName()] = node;
	//	_nodesDescriptor[node.getName()] = _graph.addVertex( v );
}

void Graph::deleteNode( const Node& node ) throw( exception::LogicError )
{}

void Graph::connect( const Node& out, const Node& in ) throw( exception::LogicError )
{
	connect( out, in.getSingleInputAttribute() );
}

void Graph::connect( const Node& out, const Attribute& inAttr ) throw( exception::LogicError )
{
	if (_nodesDescriptor.find(inAttr.getNode().getName()) == _nodesDescriptor.end())
	{
		throw exception::LogicError( "Node descriptor " + inAttr.getName() + " not found when connecting !" );
	}
	if (_nodesDescriptor.find(out.getName()) == _nodesDescriptor.end())
	{
		throw exception::LogicError( "Node descriptor " + out.getName() + " not found when connecting !" );
	}

	graph::Edge e( out.getName(), inAttr.getNode().getName(), inAttr.getName() );
	_graph.addEdge( _nodesDescriptor[out.getName()], _nodesDescriptor[inAttr.getNode().getName()], e );
}

void Graph::unconnectNode( const Node& node ) throw( exception::LogicError )
{}

void Graph::compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd )
{
	graph::GraphExporter<graph::Vertex, graph::Edge>::exportAsDOT( _graph, "graph.dot" );

	ProcessGraph process( *this );
	process.process( nodes, tBegin, tEnd );
}

void Graph::dumpToStdOut()
{
	std::cout
	<< "graph dump" << std::endl
	<< "\tnode count: " << "-" << std::endl;
	_graph.dumpToStdOut();
}

}
}
}

