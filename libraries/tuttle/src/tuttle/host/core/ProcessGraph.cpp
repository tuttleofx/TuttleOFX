#include "ProcessGraph.hpp"
#include "ProcessVisitors.hpp"
#include <tuttle/host/graph/GraphExporter.hpp>

#include <boost/foreach.hpp>

namespace tuttle {
namespace host {
namespace core {

ProcessGraph::ProcessGraph( Graph& graph )
: _nodes( graph.getNodes() )
, _instanceCount( graph.getInstanceCount() )
{
	_graph.copyTransposed( graph.getGraph() );
	relink();
}

ProcessGraph::~ProcessGraph()
{}

/**
 * @brief The graph copy needs to link on vertex owns in ProcessGraph (_nodes) and not link to Vertex inside Graph.
 */
void ProcessGraph::relink()
{
	Graph::InternalGraph::vertex_range_t vrange = _graph.getVertices();

	for( Graph::InternalGraph::vertex_iter it = vrange.first; it != vrange.second; ++it )
	{
		graph::Vertex& v = _graph.instance( *it );
		v.setProcessNode( &_nodes.at( v.getProcessNode()->getName() ) );
	}
}

void ProcessGraph::process( const std::list<std::string>& nodes, const int tBegin, const int tEnd )
{
	std::list<Graph::Descriptor> outputs;
	BOOST_FOREACH( std::string s, nodes )
	{
		outputs.push_back( _graph.getVertexDescriptor( s ) );
		COUT_DEBUG( "MY OUTPUT " << s );
	}

	graph::exportAsDOT( _graph, "graphprocess.dot" );

	// Initialize variables
	OfxPointD renderScale       = { 1.0, 1.0 };
	OfxRectD renderWindow       = { 0, 123, 0, 123 };

	//--- BEGIN RENDER
	ProcessOptions defaultOptions;
	defaultOptions._startFrame  = tBegin;
	defaultOptions._endFrame    = tEnd;
	defaultOptions._step        = 1;
	defaultOptions._interactive = false;
	// imageEffect specific...
	defaultOptions._field       = kOfxImageFieldBoth;
	defaultOptions._renderScale = renderScale;
	defaultOptions._renderRoI   = renderWindow;

	BOOST_FOREACH( Graph::NodeMap::value_type p, _nodes )
	{
		p.second->begin( defaultOptions );
	}

	//--- RENDER
	// at each frame
	for( int t = tBegin; t <= tEnd; ++t )
	{
		COUT( "________________________________________ frame: " << t );
		Graph::InternalGraph optimizedGraph( _graph );
		defaultOptions._time = t;
		
		// for each outputs
		BOOST_FOREACH( Graph::Descriptor outputNode, outputs )
		{
			COUT( "________________________________________ output node : " << optimizedGraph.instance( outputNode ).getName() );
			COUT( "---------------------------------------- connectClips" );
			core::dfs_connectClips_visitor<Graph::InternalGraph> connectClipsVisitor( optimizedGraph );
			optimizedGraph.dfs( connectClipsVisitor, outputNode );

			COUT( "---------------------------------------- preprocess" );
			core::dfs_preProcess_finish_visitor<Graph::InternalGraph> preProcessFinishVisitor( optimizedGraph, defaultOptions );
			optimizedGraph.dfs( preProcessFinishVisitor, outputNode );
			core::dfs_preProcess_initialize_visitor<Graph::InternalGraph> preProcessInitializeVisitor( optimizedGraph, defaultOptions );
			optimizedGraph.dfs( preProcessInitializeVisitor, outputNode );

			COUT( "---------------------------------------- process" );
			core::dfs_process_visitor<Graph::InternalGraph> processVisitor( optimizedGraph );
			optimizedGraph.dfs( processVisitor, outputNode );

			COUT( "---------------------------------------- postprocess" );
			core::dfs_postProcess_visitor<Graph::InternalGraph> postProcessVisitor( optimizedGraph );
			optimizedGraph.dfs( postProcessVisitor, outputNode );
		}
		core::Core::instance().getMemoryCache().clearAll();
	}

	//--- END RENDER
	BOOST_FOREACH( Graph::NodeMap::value_type p, _nodes )
	{
		p.second->end( defaultOptions ); // node option... or no option here ?
	}

}

}
}
}

