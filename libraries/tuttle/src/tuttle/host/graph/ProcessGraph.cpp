#include "ProcessGraph.hpp"
#include "ProcessVisitors.hpp"
#include <tuttle/host/graph/GraphExporter.hpp>

#include <boost/foreach.hpp>

namespace tuttle {
namespace host {
namespace graph {

ProcessGraph::ProcessGraph( Graph& graph, const std::list<std::string>& outputNodes )
: _nodes( graph.getNodes() )
, _instanceCount( graph.getInstanceCount() )
{
	_graph.copyTransposed( graph.getGraph() );

	Vertex outputVertex("TUTTLE_FAKE_OUTPUT");
	_output = _graph.addVertex( outputVertex );
	
	BOOST_FOREACH( std::string s, outputNodes )
	{
		_graph.connect( "TUTTLE_FAKE_OUTPUT", s, "Output" );
		COUT_DEBUG( "MY OUTPUT: " << s );
	}
	relink();
}

ProcessGraph::~ProcessGraph()
{}

/**
 * @brief The graph copy needs to link on vertex owns in ProcessGraph (_nodes) and not link to Vertex inside Graph.
 */
void ProcessGraph::relink()
{
	InternalGraphImpl::vertex_range_t vrange = _graph.getVertices();

	for( InternalGraphImpl::vertex_iter it = vrange.first; it != vrange.second; ++it )
	{
		graph::Vertex& v = _graph.instance( *it );
		if( !v.isFake() )
			v.setProcessNode( &_nodes.at( v.getProcessNode()->getName() ) );
	}
}

/*
void removeVertexAndReconnectTo( const VertexDescriptor& v, const VertexDescriptor& other )
{
	InternalGraph::out_edge_iterator oe, oeEnd;
	tie(oe, oeEnd) = out_edges(v, g);
	// InternalGraph::in_edge_iterator ie, ieEnd;
	// tie(ie, ieEnd) = in_edges(v, g);

	for( ; oe != oeEnd; ++oe )
		source( oe )

	_graph.removeVertex( v );
}
*/

void ProcessGraph::process( const int tBegin, const int tEnd )
{
	COUT( "process" );
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

	COUT( "process begin" );
	BOOST_FOREACH( Graph::NodeMap::value_type p, _nodes )
	{
		p.second->begin( defaultOptions );
	}

	COUT( "process render..." );
	//--- RENDER
	// at each frame
	for( int t = tBegin; t <= tEnd; ++t )
	{
		COUT( "________________________________________ frame: " << t );
		Graph::InternalGraphImpl optimizedGraph( _graph );
		defaultOptions._time = t;
		
		COUT( "________________________________________ output node : " << optimizedGraph.instance( _output ).getName() );
		COUT( "---------------------------------------- connectClips" );
		graph::dfs_connectClips_visitor<Graph::InternalGraphImpl> connectClipsVisitor( optimizedGraph );
		optimizedGraph.dfs( connectClipsVisitor, _output );

		COUT( "---------------------------------------- preprocess" );
		graph::dfs_preProcess_finish_visitor<Graph::InternalGraphImpl> preProcessFinishVisitor( optimizedGraph, defaultOptions );
		optimizedGraph.dfs( preProcessFinishVisitor, _output );
		graph::dfs_preProcess_initialize_visitor<Graph::InternalGraphImpl> preProcessInitializeVisitor( optimizedGraph, defaultOptions );
		optimizedGraph.dfs( preProcessInitializeVisitor, _output );

		COUT( "---------------------------------------- process" );
		graph::dfs_process_visitor<Graph::InternalGraphImpl> processVisitor( optimizedGraph );
		optimizedGraph.dfs( processVisitor, _output );

		COUT( "---------------------------------------- postprocess" );
		graph::dfs_postProcess_visitor<Graph::InternalGraphImpl> postProcessVisitor( optimizedGraph );
		optimizedGraph.dfs( postProcessVisitor, _output );

		// end of one frame
		// do some clean memory clean, as temporary solution...
		COUT( "---------------------------------------- clearUnused" );
		Core::instance().getMemoryCache().clearUnused();
		COUT( "---------------------------------------- updateMemoryAuthorizedWithRAM" );
		Core::instance().getMemoryPool().updateMemoryAuthorizedWithRAM();
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

