#include "ProcessGraph.hpp"
#include "ProcessVisitors.hpp"
#include <tuttle/host/graph/GraphExporter.hpp>

#include <boost/foreach.hpp>



//TODO: delete these include
#include <tuttle/host/core/Param.hpp>
#include <tuttle/host/core/ClipImage.hpp>


namespace tuttle {
namespace host {
namespace core {


ProcessGraph::ProcessGraph( Graph& graph )
: _graph( graph.getGraph() )
, _nodes( graph.getNodes() )
, _instanceCount( graph.getInstanceCount() )
{
	relink();
}

ProcessGraph::~ProcessGraph()
{
}

void ProcessGraph::relink()
{
	Graph::InternalGraph::vertex_range_t vrange = _graph.getVertices();
	for( Graph::InternalGraph::vertex_iter it = vrange.first; it != vrange.second; ++it )
	{
		graph::Vertex& v = _graph.instance( *it );
		v.setProcessNode( &_nodes.at( v.processNode()->getName() ) );
	}
}

void ProcessGraph::compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd )
{
	//graph::GraphExporter<graph::Vertex, graph::Edge>::exportAsDOT( _graph, "test.dot" );

	// Initialize variables
	const int numFramesToRender = tEnd - tBegin;
	OfxPointD renderScale = { 1.0, 1.0 };
	OfxRectI renderWindow = { 0, 0,	123, 123 };


	BOOST_FOREACH( Graph::NodeMap::value_type p, _nodes ){

		p.second->createInstanceAction();
		p.second->getClipPreferences();
		p.second->dumpToStdOut();

//		ClipImage& outputClip = dynamic_cast<ClipImage& >( p.second->getClip(kOfxImageEffectOutputClipName) );
//		outputClip.setPixelDepth( kOfxBitDepthByte );
//		outputClip.setComponents( kOfxImageComponentRGBA );
	}
/*
	// Setup parameters
	EffectInstance& firstEffect = dynamic_cast<EffectInstance&>(_nodes.at("PNGReader1"));
	EffectInstance& lastEffect = dynamic_cast<EffectInstance&>(_nodes.at("PNGWriterHd3d1"));
	firstEffect.getParam("Input filename").set("input.png");
	lastEffect.getParam("Output filename").set("output.png");
	firstEffect.paramInstanceChangedAction("Input filename", kOfxChangeUserEdited, OfxTime( 0 ), renderScale );
	lastEffect.paramInstanceChangedAction("Output filename", kOfxChangeUserEdited, OfxTime( 0 ), renderScale );
*/

	// Connecting nodes
	core::dfs_connect_visitor connectVisitor;
	_graph.dfs(connectVisitor);


	//--- BEGIN RENDER
	ProcessOptions processOptions;
	BOOST_FOREACH( Graph::NodeMap::value_type p, _nodes ){
		processOptions._startFrame = tBegin;
		processOptions._endFrame = tEnd;
		processOptions._step = 1;
		processOptions._interactive = false;
		processOptions._renderScale = renderScale;
		p.second->begin(processOptions);
	}


	//--- RENDER
	for( int t = 0; t < numFramesToRender; ++t )
	{
		core::dfs_compute_visitor computeVisitor(processOptions);
		processOptions._time = t;
		processOptions._field = kOfxImageFieldBoth;
		processOptions._renderRoI = renderWindow;
		processOptions._renderScale = renderScale;
		Graph::InternalGraph optimizedGraph(_graph);
		//optimizedGraph.toDominatorTree();
		optimizedGraph.dfs(computeVisitor);
	}


	//--- END RENDER
	BOOST_FOREACH( Graph::NodeMap::value_type p, _nodes ){
		processOptions._startFrame = tBegin;
		processOptions._endFrame = tEnd;
		processOptions._step = 1;
		processOptions._interactive = false;
		processOptions._renderScale = renderScale;
		p.second->end(processOptions);
	}
}

}
}
}

