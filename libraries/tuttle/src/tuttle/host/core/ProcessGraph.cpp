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
: _nodes( graph.getNodes() )
, _instanceCount( graph.getInstanceCount() )
{
	_graph.copyTransposed( graph.getGraph() );
	relink();
}

ProcessGraph::~ProcessGraph()
{}

void ProcessGraph::relink()
{
	Graph::InternalGraph::vertex_range_t vrange = _graph.getVertices();

	for( Graph::InternalGraph::vertex_iter it = vrange.first; it != vrange.second; ++it )
	{
		graph::Vertex& v = _graph.instance( *it );
		v.setProcessNode( &_nodes.at( v.getProcessNode()->getName() ) );
	}
}

void ProcessGraph::compute( const std::list<std::string>& nodes, const int tBegin, const int tEnd )
{
	std::list<Graph::Descriptor> outputs;
	BOOST_FOREACH( std::string s, nodes )
	{
		outputs.push_back( _graph.getVertexDescriptor( s ) );
		std::cout << "MY OUTPUT " << s << std::endl;
	}

	graph::GraphExporter<graph::Vertex, graph::Edge>::exportAsDOT( _graph, "graphprocess.dot" );

	// Initialize variables
	const int numFramesToRender = tEnd - tBegin;
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
	for( int t = 0; t < numFramesToRender; ++t )
	{
		TCOUT( "________________________________________ frame: " << t );
		Graph::InternalGraph optimizedGraph( _graph );
		defaultOptions._time = t;
		
		// for each outputs
		BOOST_FOREACH( Graph::Descriptor outputNode, outputs )
		{
			TCOUT( "________________________________________ output node : " << optimizedGraph.instance( outputNode ).getName() );
			TCOUT( "---------------------------------------- connectClips" );
			core::dfs_connectClips_visitor<Graph::InternalGraph> connectClipsVisitor( optimizedGraph );
			optimizedGraph.dfs( connectClipsVisitor, outputNode );

			TCOUT( "---------------------------------------- precompute" );
			core::dfs_preCompute_visitor<Graph::InternalGraph> preComputeVisitor( optimizedGraph, defaultOptions );
			optimizedGraph.dfs( preComputeVisitor, outputNode );

			TCOUT( "---------------------------------------- compute" );
			core::dfs_compute_visitor<Graph::InternalGraph> computeVisitor( optimizedGraph );
			optimizedGraph.dfs( computeVisitor, outputNode );

			TCOUT( "---------------------------------------- postcompute" );
			core::dfs_postCompute_visitor<Graph::InternalGraph> postComputeVisitor( optimizedGraph );
			optimizedGraph.dfs( postComputeVisitor, outputNode );
		}
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

