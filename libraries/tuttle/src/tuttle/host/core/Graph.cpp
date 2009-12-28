#include "Graph.hpp"
#include "ProcessGraph.hpp"
#include <tuttle/host/ofx/OfxhClipImage.hpp>
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

Graph::Node& Graph::createNode( const std::string& id ) throw( exception::LogicError )
{
	ofx::imageEffect::OfxhImageEffectPlugin* plug = Core::instance().getImageEffectPluginById( id );

	if( !plug )
		throw exception::LogicError( "Plugin not found. plug (" + id + ")" );

	plug->loadAndDescribeActions();

	ofx::imageEffect::OfxhImageEffectNode* plugInst = NULL;
	if( plug->supportsContext( kOfxImageEffectContextFilter ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextFilter );
	}
	else if( plug->supportsContext( kOfxImageEffectContextGenerator ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextGenerator );
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

	_nodes.insert( node->getName(), node );
	addToGraph( *node );

	return *node;
}


void Graph::addToGraph( Node& node )
{
	graph::Vertex v( node.getName(), node );

	std::cout<< node.getName() <<std::endl;
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
	graph::Edge e( out.getName(), inAttr.getNode().getName(), inAttr.getName() );

	_graph.addEdge( _nodesDescriptor[out.getName()], _nodesDescriptor[inAttr.getNode().getName()], e );
}

void Graph::unconnectNode( const Node& node ) throw( exception::LogicError )
{}

void Graph::compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd )
{
	graph::GraphExporter<graph::Vertex, graph::Edge>::exportAsDOT( _graph, "graph.dot" );

	ProcessGraph process( *this );
	process.compute( nodes, tBegin, tEnd );
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

