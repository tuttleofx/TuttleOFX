#include "ProcessGraph.hpp"
#include "ProcessVisitors.hpp"
#include <tuttle/host/graph/GraphExporter.hpp>

//TODO: delete this include
#include <tuttle/host/core/ParamInstance.hpp>


namespace tuttle {
namespace host {
namespace core {


ProcessGraph::ProcessGraph( const Graph& graph )
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
	const int numFramesToRender = 1;
	OfxPointD renderScale = { 1.0, 1.0 };
	OfxRectI renderWindow = { 0, 0,
							123, 123 };




	//TODO dog-faced   :/
	Graph::InternalGraph::vertex_range_t vrange = _graph.getVertices();
	for( Graph::InternalGraph::vertex_iter it = vrange.first; it != vrange.second; ++it )
	{
		dynamic_cast<EffectInstance*>(_graph.instance(*it).processNode())->createInstanceAction();
		dynamic_cast<EffectInstance*>(_graph.instance(*it).processNode())->getClipPreferences();
	}

	// Setup parameters
	EffectInstance& firstEffect = dynamic_cast<EffectInstance&>(_nodes.at("PNGReaderHd3d1"));
	EffectInstance& lastEffect = dynamic_cast<EffectInstance&>(_nodes.at("PNGWriterHd3d1"));

	firstEffect.getParam("Input filename").set("input.png");
	lastEffect.getParam("Output filename").set("output.png");
	firstEffect.paramInstanceChangedAction("Input filename", kOfxChangeUserEdited, OfxTime( 0 ), renderScale );
	lastEffect.paramInstanceChangedAction("Output filename", kOfxChangeUserEdited, OfxTime( 0 ), renderScale );

	ProcessOptions processOptions;
	//--- BEGIN RENDER
	for( Graph::InternalGraph::vertex_iter it = vrange.first; it != vrange.second; ++it )
	{
		graph::Vertex& v = _graph.instance( *it );
		processOptions._startFrame = tBegin;
		processOptions._endFrame = tEnd;
		processOptions._step = 1;
		processOptions._interactive = false;
		processOptions._renderScale = renderScale;
		v.processNode()->begin(processOptions);
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
	for( Graph::InternalGraph::vertex_iter it = vrange.first; it != vrange.second; ++it )
	{
		graph::Vertex& v = _graph.instance( *it );
		v.processNode()->end(processOptions);
	}

//	graph::GraphExporter<graph::Vertex, graph::Edge>::exportAsDOT( optimizedGraph, "test.dot" );
}

}
}
}

