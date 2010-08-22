#include "ProcessGraph.hpp"
#include "ProcessVisitors.hpp"
#include <tuttle/host/graph/GraphExporter.hpp>

#include <boost/foreach.hpp>


namespace tuttle {
namespace host {
namespace graph {

const std::string ProcessGraph::_outputId( "TUTTLE_FAKE_OUTPUT" );

ProcessGraph::ProcessGraph( Graph& graph, const std::list<std::string>& outputNodes )
: _nodes( graph.getNodes() )
, _instanceCount( graph.getInstanceCount() )
{
	_graph.copyTransposed( graph.getGraph() );

	Vertex outputVertex(_outputId);
	_graph.addVertex( outputVertex );
	
	BOOST_FOREACH( const std::string& s, outputNodes )
	{
		_graph.connect( "TUTTLE_FAKE_OUTPUT", s, "Output" );
		COUT_DEBUG( "MY OUTPUT: " << s );
	}
	relink();
//	removeUnconnectedVertex();
}

ProcessGraph::~ProcessGraph()
{}

/**
 * @brief The graph copy needs to link on vertex owns in ProcessGraph (_nodes) and not link to Vertex inside Graph.
 */
void ProcessGraph::relink()
{
	InternalGraphImpl::vertex_range_t vrange = _graph.getVertices();

	for( InternalGraphImpl::vertex_iterator it = vrange.first; it != vrange.second; ++it )
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
	using namespace boost;
	using namespace boost::graph;
	COUT( "process" );
	graph::exportAsDOT( _graph, "graphprocess.dot" );

	// Initialize variables
	OfxPointD renderScale       = { 1.0, 1.0 };
	OfxRectD renderWindow       = { 0, 123, 0, 123 };

	//--- BEGIN RENDER
	ProcessOptions defaultOptions;
	defaultOptions._time        = tBegin;
	defaultOptions._startFrame  = tBegin;
	defaultOptions._endFrame    = tEnd;
	defaultOptions._step        = 1;
	defaultOptions._interactive = false;
	// imageEffect specific...
	defaultOptions._field       = kOfxImageFieldBoth;
	defaultOptions._renderScale = renderScale;
	defaultOptions._renderRoI   = renderWindow;

	COUT( "process begin" );
	BOOST_FOREACH( NodeMap::value_type p, _nodes )
	{
		p.second->begin( defaultOptions );
	}

	COUT( "process render..." );
	//--- RENDER
	// at each frame
	for( int t = tBegin; t <= tEnd; ++t )
	{
		defaultOptions._time = t;
		COUT( "________________________________________ frame: " << t );
		InternalGraphImpl renderGraph = InternalGraphImpl( _graph );
		InternalGraphImpl::vertex_descriptor& output = renderGraph.getVertexDescriptor( _outputId );

		COUT( "________________________________________ output node : " << renderGraph.getVertex( _outputId ).getName() );

		COUT( "---------------------------------------- set default options" );
		{
			InternalGraphImpl::vertex_iterator v_it, v_end;
			for( tie(v_it, v_end) = renderGraph.getVertices(); v_it != v_end; ++v_it )
			{
				Vertex& v = renderGraph.instance( *v_it );
				v.setProcessOptions( defaultOptions );
			}
		}
		
		COUT( "---------------------------------------- connectClips" );
		graph::dfs_connectClips_visitor<InternalGraphImpl> connectClipsVisitor( renderGraph );
		renderGraph.dfs( connectClipsVisitor, output );

		COUT( "---------------------------------------- preprocess 1" );
		graph::dfs_preProcess_finish_visitor<InternalGraphImpl> preProcessFinishVisitor( renderGraph );
		renderGraph.dfs( preProcessFinishVisitor, output );
		
		COUT( "---------------------------------------- preprocess 2" );
		graph::dfs_preProcess_initialize_visitor<InternalGraphImpl> preProcessInitializeVisitor( renderGraph );
		renderGraph.dfs( preProcessInitializeVisitor, output );

		COUT( "---------------------------------------- optimizeGraph" );
		graph::dfs_optimizeGraph_visitor<InternalGraphImpl> optimizeGraphVisitor( renderGraph );
		renderGraph.dfs( optimizeGraphVisitor, output );

		// remove isIdentity nodes

		COUT( "---------------------------------------- process" );
		graph::dfs_process_visitor<InternalGraphImpl> processVisitor( renderGraph );
		renderGraph.dfs( processVisitor, output );

		COUT( "---------------------------------------- postprocess" );
		graph::dfs_postProcess_visitor<InternalGraphImpl> postProcessVisitor( renderGraph );
		renderGraph.dfs( postProcessVisitor, output );

		// end of one frame
		// do some clean: memory clean, as temporary solution...
		COUT( "---------------------------------------- clearUnused" );
		Core::instance().getMemoryCache().clearUnused();
		COUT( "---------------------------------------- updateMemoryAuthorizedWithRAM" );
		Core::instance().getMemoryPool().updateMemoryAuthorizedWithRAM();
	}

	//--- END RENDER
	BOOST_FOREACH( NodeMap::value_type p, _nodes )
	{
		p.second->end( defaultOptions ); // node option... or no option here ?
	}

}

}
}
}

