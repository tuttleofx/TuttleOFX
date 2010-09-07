#ifndef _TUTTLE_PROCESSVISITORS_HPP_
#define _TUTTLE_PROCESSVISITORS_HPP_

#include "ProcessOptions.hpp"
#include <iostream>
#include <vector>
#include <boost/graph/properties.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>

namespace tuttle {
namespace host {
namespace graph {
namespace visitor {

template<class TGraph>
class ConnectClips : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;

	ConnectClips( TGraph& graph )
		: _graph( graph )
	{}

	template<class EdgeDescriptor, class Graph>
	void examine_edge( EdgeDescriptor e, Graph& g )
	{
		Edge& edge           = _graph.instance( e );
		Vertex& vertexSource = _graph.sourceInstance( e );
		Vertex& vertexDest   = _graph.targetInstance( e );

		if( vertexDest.isFake() || vertexSource.isFake() )
			return;

//		TCOUT( "examine_edge" << vertexSource );
//		TCOUT( "[CONNECT] examine_edge "
//		    << vertexSource
//		    << " TO "
//		    << vertexDest << "." << edge.getInAttrName() );

		INode& sourceNode = *vertexSource.getProcessNode();
		INode& targetNode = *vertexDest.getProcessNode();
		sourceNode.connect( targetNode, sourceNode.getAttribute( edge.getInAttrName() ) );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class PreProcess1 : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	PreProcess1( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TCOUT( "[PREPROCESS 1] discover_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode()->preProcess1_initialize( vertex.getProcessOptions() );
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TCOUT( "[PREPROCESS 1] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode()->preProcess1_finish( vertex.getProcessOptions() );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class PreProcess2 : public boost::default_bfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	PreProcess2( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TCOUT( "[PREPROCESS 2] discover_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode()->preProcess2_initialize( vertex.getProcessOptions() );
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TCOUT( "[PREPROCESS 2] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode()->preProcess2_finish( vertex.getProcessOptions() );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class PreProcess3 : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	PreProcess3( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TCOUT( "[PREPROCESS 3] discover_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode()->preProcess3_initialize( vertex.getProcessOptions() );
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TCOUT( "[PREPROCESS 3] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode()->preProcess3_finish( vertex.getProcessOptions() );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class OptimizeGraph : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;
	typedef typename TGraph::vertex_descriptor vertex_descriptor;
	typedef typename TGraph::edge_descriptor edge_descriptor;
	typedef typename TGraph::in_edge_iterator in_edge_iterator;
	typedef typename TGraph::out_edge_iterator out_edge_iterator;

	OptimizeGraph( TGraph& graph )
		: _graph( graph )
	{
//		COUT_X( 80, ":" );
	}

	template <class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, const Graph& g )
	{
		using namespace boost;
		using namespace boost::graph;
		Vertex& vertex = _graph.instance( v );

		ProcessOptions& procOptions = vertex.getProcessOptions();
		if( !vertex.isFake() )
		{
			// compute local infos, need to be a real node !
			vertex.getProcessNode()->preProcess_infos( procOptions._localInfos );
		}

		// compute global infos for inputs

		// direct dependencies (originally the node inputs)
		BOOST_FOREACH( const edge_descriptor& oe, out_edges( v, _graph.getGraph() ) )
		{
//			Edge& outEdge = _graph.instance(*oe);
			Vertex& outVertex = _graph.targetInstance( oe );
			procOptions._inputsInfos += outVertex.getProcessOptions()._localInfos;
			procOptions._globalInfos += outVertex.getProcessOptions()._globalInfos;
		}
		procOptions._globalInfos += procOptions._localInfos;

//		BOOST_FOREACH( const edge_descriptor& ie, in_edges( v, _graph.getGraph() ) )
//		{
//			Edge& e = _graph.instance( ie );
//		}


//		COUT_X( 80, "." );
//		COUT( vertex.getName() );
//		COUT( procOptions );
//		COUT_X( 80, "." );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class Process : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	Process( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

//		COUT( "[PROCESS] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode()->process( vertex.getProcessOptions() );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class PostProcess : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	PostProcess( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void initialize_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

//		TCOUT( "[POSTPROCESS] initialize_vertex " << vertex );
		if( vertex.isFake() )
			return;
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

//		TCOUT( "[POSTPROCESS] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode()->postProcess( vertex.getProcessOptions() );
	}

private:
	TGraph& _graph;
};

}
}
}
}

#endif

