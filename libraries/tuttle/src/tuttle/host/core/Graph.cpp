#include "Graph.hpp"

#include <iostream>
#include <sstream>
#include <tuttle/host/graph/GraphExporter.hpp>

namespace tuttle {
namespace host {
namespace core {

Graph::Graph()
	: _nodeCount( 0 )
{}

NodeID Graph::createNode( const std::string& id ) throw( std::logic_error )
{
	ofx::imageEffect::ImageEffectPlugin* plug = Core::instance().getImageEffectPluginById( id );

	if( !plug )
		throw std::logic_error( "Plugin not found. plug" );
	ofx::imageEffect::Instance* plugInst = plug->createInstance( kOfxImageEffectContextFilter, NULL );
	if( !plugInst )
		throw std::logic_error( "Plugin not found. plugInst" );
	EffectInstance* node = dynamic_cast<EffectInstance*>( plugInst );
	if( !node )
		throw std::logic_error( "Plugin not found." );

	std::stringstream uniqueName;
	uniqueName << node->getLabel() << ++_instanceCount[node->getLabel()];
	node->setName( uniqueName.str() );

	addToGraph( *node );

	return _nodeCount++;
}

void Graph::addToGraph( EffectInstance& node )
{
	COUT( "Graph::createNode - getName: " << node.getName() );
	//TODO: set the good attribute name
	graph::Vertex v( node.getName(), &node, std::string( "Attribute" ) );
	_nodes[_nodeCount] = _graph.addVertex( v );
}

void Graph::deleteNode( const EffectInstance& node )
{}

void Graph::connect( const NodeID& out, const NodeID& in )
{
	graph::Edge e( "edge" );

	_graph.addEdge( _nodes[out], _nodes[in], e );
}

void Graph::dumpToStdOut()
{
	std::cout
	<< "graph dump" << std::endl
	<< "\tnode count: " << "-" << std::endl;
	_graph.dumpToStdOut();
}

void Graph::compute()
{
	_graph.test_dfs();

	graph::GraphExporter<graph::Vertex, graph::Edge>::exportAsDOT( _graph, "testDOTExport.dot" );
}

}
}
}

