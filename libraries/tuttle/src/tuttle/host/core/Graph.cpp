#include "Graph.hpp"
#include "ProcessGraph.hpp"
#include <tuttle/host/ofx/ofxhClipImage.hpp>
#include <tuttle/host/graph/GraphExporter.hpp>
#include <iostream>
#include <sstream>

namespace tuttle {
namespace host {
namespace core {

Graph::Graph()
{
}

Graph::Graph( const Graph& other )
: _graph(other._graph)
, _nodes(other._nodes)
, _instanceCount(other._instanceCount)
{
	relink();
}

Graph::~Graph()
{
}

Graph::Node& Graph::createNode( const std::string& id ) throw( exception::LogicError )
{
	ofx::imageEffect::OfxhImageEffectPlugin* plug = Core::instance().getImageEffectPluginById( id );

	if( !plug )
		throw exception::LogicError( "Plugin not found. plug ("+id+")" );

	plug->loadAndDescribeActions(); ///< @todo tuttle keep here or move inside getImageEffectPluginById

	ofx::imageEffect::OfxhImageEffect* plugInst = NULL;
	if( plug->supportsContext( kOfxImageEffectContextFilter ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextFilter, NULL );
	}
	else if( plug->supportsContext( kOfxImageEffectContextGenerator ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextGenerator, NULL );
	}
	else if( plug->supportsContext( kOfxImageEffectContextGeneral ) )
	{
		plugInst = plug->createInstance( kOfxImageEffectContextGeneral, NULL );
	}
	else
	{
		throw exception::LogicError( "Plugin contexts not supported by the host. ("+id+")" );
	}


	if( !plugInst )
		throw exception::LogicError( "Plugin not found. plugInst ("+id+")" );
	EffectInstance* node = dynamic_cast<EffectInstance*>( plugInst );
	if( !node )
		throw exception::LogicError( "Plugin not found ("+id+")." );

	std::stringstream uniqueName;
	uniqueName << node->getLabel() << ++_instanceCount[node->getLabel()];
	node->setName( uniqueName.str() );

	_nodes.insert( node->getName(), node );
	addToGraph( *node );

	return *node;
}

void Graph::relink()
{
	InternalGraph::vertex_range_t vrange = _graph.getVertices();
	for( InternalGraph::vertex_iter it = vrange.first; it != vrange.second; ++it )
	{
		graph::Vertex & v = _graph.instance( *it );
		v.setProcessNode( &_nodes.at(v.processNode()->getName()) );
	}
}

void Graph::addToGraph( EffectInstance& node )
{
	graph::Vertex v( node.getName(), node );
	_nodesDescriptor[node.getName()] = _graph.addVertex( v );
}

void Graph::removeFromGraph( EffectInstance& node ) throw( exception::LogicError )
{
	//	graph::Vertex v( node.getName(), &node );
	//
	//	_nodesList.find( &node );
	//	_nodes[node.getName()] = node;
	//	_nodesDescriptor[node.getName()] = _graph.addVertex( v );
}

void Graph::deleteNode( const EffectInstance& node ) throw( exception::LogicError )
{

}

void Graph::connect( const Node& out, const Node& in ) throw( exception::LogicError )
{
	const ofx::attribute::OfxhClipImageSet::ClipImageVector& inClips = in.getClipsByOrder();
	const ofx::attribute::OfxhClipImageSet::ClipImageMap& inClipsMap = in.getClips();

	const ofx::attribute::AttributeInstance* inAttr;

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


void Graph::connect( const Node& out, const Node& in, const ofx::attribute::AttributeInstance& inAttr ) throw( exception::LogicError )
{
	graph::Edge e( out.getName(), in.getName(), inAttr.getName() );
	_graph.addEdge( _nodesDescriptor[out.getName()], _nodesDescriptor[in.getName()], e );
}

void Graph::unconnectNode( const EffectInstance& node ) throw( exception::LogicError )
{

}

void Graph::compute(const std::list<std::string>& nodes, const int first, const int last)
{
	ProcessGraph process(*this);
	for( int t = first; t<last; ++t )
	{
		process.compute( nodes, t );
	}
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

