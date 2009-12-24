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


void Graph::addToGraph( ImageEffectNode& node )
{
	graph::Vertex v( node.getName(), node );

	std::cout<< node.getName() <<std::endl;
	_nodesDescriptor[node.getName()] = _graph.addVertex( v );
}

void Graph::removeFromGraph( ImageEffectNode& node ) throw( exception::LogicError )
{
	//	graph::Vertex v( node.getName(), &node );
	//
	//	_nodesList.find( &node );
	//	_nodes[node.getName()] = node;
	//	_nodesDescriptor[node.getName()] = _graph.addVertex( v );
}

void Graph::deleteNode( const ImageEffectNode& node ) throw( exception::LogicError )
{}

void Graph::connect( const Node& out, const Node& in ) throw( exception::LogicError )
{
	const ofx::attribute::OfxhClipImageSet::ClipImageVector& inClips = in.getClipsByOrder();
	const ofx::attribute::OfxhClipImageSet::ClipImageMap& inClipsMap = in.getClips();

	const ofx::attribute::OfxhAttribute* inAttr;

	if( inClips.size() == 1 )
	{
		inAttr = &inClips[0];
	}
	else if( inClips.size() > 1 )
	{
		const ofx::attribute::OfxhClipImageSet::ClipImageMap::const_iterator it = inClipsMap.find( kOfxSimpleSourceAttributeName );
		if( it != inClipsMap.end() )
		{
			inAttr = it->second;
		}
		else
		{
			// search "Source"
			inAttr = &inClips[0];
		}
	}
	else // if( inClips.empty() )
	{
		throw exception::LogicError( "Connection failed : no clip." );
	}
	connect( out, in, *inAttr );
}

void Graph::connect( const Node& out, const Node& in, const ofx::attribute::OfxhAttribute& inAttr ) throw( exception::LogicError )
{
	graph::Edge e( out.getName(), in.getName(), inAttr.getName() );

	_graph.addEdge( _nodesDescriptor[out.getName()], _nodesDescriptor[in.getName()], e );
}

void Graph::unconnectNode( const ImageEffectNode& node ) throw( exception::LogicError )
{}

void Graph::compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd )
{
	graph::GraphExporter<graph::Vertex, graph::Edge>::exportAsDOT( _graph, "graph.dot" );

	ProcessGraph process( *this );

	std::list<InternalGraph::VertexDescriptor> outputs;
	BOOST_FOREACH(std::string s, nodes){
		outputs.push_back(_nodesDescriptor[s]);
		std::cout << "MY OUTPUT " << s << std::endl;
	}
	process.compute( outputs, tBegin, tEnd );
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

