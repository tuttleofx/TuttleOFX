#include "Graph.hpp"
//#include "ClipInstance.hpp"
#include <tuttle/host/ofx/ofxhClipImage.h>
#include <tuttle/host/graph/GraphExporter.hpp>
#include <iostream>
#include <sstream>

namespace tuttle {
namespace host {
namespace core {

Graph::Graph()
	: _nodeCount( 0 )
{}

Graph::~Graph()
{
	for( std::list<Node*>::iterator it = _nodesList.begin(), itEnd = _nodesList.end();
	     it != itEnd;
	     ++it )
	{
		delete *it;
	}
}

Graph::Node& Graph::createNode( const std::string& id ) throw( std::logic_error )
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

	++_nodeCount;

	return *node;
}

void Graph::addToGraph( EffectInstance& node )
{
	graph::Vertex v( node.getName(), &node );

	_nodesList.push_back( &node );
	_nodes[node.getName()]           = &node;
	_nodesDescriptor[node.getName()] = _graph.addVertex( v );
}

void Graph::removeFromGraph( EffectInstance& node )
{
	//	graph::Vertex v( node.getName(), &node );
	//
	//	_nodesList.find( &node );
	//	_nodes[node.getName()] = node;
	//	_nodesDescriptor[node.getName()] = _graph.addVertex( v );
}

void Graph::deleteNode( const EffectInstance& node )
{}

void Graph::connect( const Node& out, const Node& in ) throw( Exception )
{
	const ofx::attribute::ClipImageInstanceSet::ClipImageVector& inClips = in.getClipsByOrder();
	const ofx::attribute::ClipImageInstanceSet::ClipImageMap& inClipsMap = in.getClips();
	std::string inAttrName;
	if( inClips.size() == 1 )
	{
		inAttrName = inClips[0].getName();
	}
	else if( inClips.size() > 1 )
	{
		const ofx::attribute::ClipImageInstanceSet::ClipImageMap::const_iterator it = inClipsMap.find( kOfxSimpleSourceAttributeName );
		if( it != inClipsMap.end() )
		{
			inAttrName = it->second->getName();
		}
		else
		{
			// search "Source"
			inAttrName = inClips[0].getName();
		}
	}
	else // if( inClips.empty() )
	{
		throw Exception( "Connection failed : no clip." );
	}

	graph::Edge e( out.getName() + "." + kOfxOutputAttributeName + "-->" + in.getName() + "." + inAttrName );

	_graph.addEdge( _nodesDescriptor[out.getName()], _nodesDescriptor[in.getName()], e );
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

