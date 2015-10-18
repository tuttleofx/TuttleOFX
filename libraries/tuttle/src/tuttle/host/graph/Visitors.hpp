#ifndef _TUTTLE_HOST_VISITORS_HPP_
#define _TUTTLE_HOST_VISITORS_HPP_

#include <iostream>
#include <vector>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/visitors.hpp>

namespace tuttle {
namespace host {
namespace graph {
namespace visitor {

/**
 * @brief detect if there is a cycle inside a directed graph
 * or if we can garantee that it's a DAG, Directed Acyclic Graph.
 */
class CycleDetector : public boost::default_dfs_visitor
{
public:
	CycleDetector( bool& hasCycle )
		: _hasCycle( hasCycle )
	{
		_hasCycle = false;
	}

public:
	template<class EdgeDescriptor, class Graph>
	void back_edge( EdgeDescriptor, const Graph& )
	{
		_hasCycle = true;
	}

public:
	bool& _hasCycle;
};

template<class TGraph>
class MarkUsed : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge ProcessEdge;

	MarkUsed( TGraph& graph )
		: _graph( graph )
	{}

	/**
	 * Set all vertex with unused default value.
	 */
	template <class VertexDescriptor, class Graph>
	void initialize_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		//TUTTLE_LOG_INFO( "MarkUsed &&&&& init" << vertex.getName() );
		vertex.setUsed( false );
	}

	/**
	 * Set visited vertex used.
	 */
	template <class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		//TUTTLE_LOG_VAR( TUTTLE_TRACE, vertex.getName() );
		vertex.setUsed();
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class Test_dfs : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge ProcessEdge;

	Test_dfs( TGraph& graph )
		: _graph( graph )
	{
		TUTTLE_LOG_TRACE( "Test_dfs" );
	}

	~Test_dfs()
	{
		TUTTLE_LOG_TRACE( "~Test_dfs" );
	}

	template <class VertexDescriptor, class Graph>
	void initialize_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_LOG_TRACE( "initialize_vertex: " << vertex );
	}

	template <class VertexDescriptor, class Graph>
	void start_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_LOG_TRACE( "start_vertex: " << vertex );
	}

	template <class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_LOG_TRACE( "discover_vertex: " << vertex );
	}

	template <class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_LOG_TRACE( "finish_vertex: " << vertex );
	}

	template<class EdgeDescriptor, class Graph>
	void examine_edge( EdgeDescriptor e, Graph& g )
	{
		ProcessEdge& edge = _graph.instance( e );

		//		Vertex& vertexSource = _graph.sourceInstance(e);
		//		Vertex& vertexDest   = _graph.targetInstance(e);

		TUTTLE_LOG_TRACE( "examine_edge: " << edge );
	}

	template <class EdgeDescriptor, class Graph>
	void tree_edge( EdgeDescriptor e, const Graph& g )
	{
		ProcessEdge& edge = _graph.instance( e );

		TUTTLE_LOG_TRACE( "tree_edge: " << edge  );
	}

	template <class EdgeDescriptor, class Graph>
	void back_edge( EdgeDescriptor e, const Graph& g )
	{
		ProcessEdge& edge = _graph.instance( e );

		TUTTLE_LOG_TRACE( "back_edge: " << edge  );
	}

	template <class EdgeDescriptor, class Graph>
	void forward_or_cross_edge( EdgeDescriptor e, const Graph& g )
	{
		ProcessEdge& edge = _graph.instance( e );

		TUTTLE_LOG_TRACE( "forward_or_cross_edge: " << edge );
	}

private:
	TGraph& _graph;
};

class Test_bfs : public boost::default_bfs_visitor
{
public:
	Test_bfs() {}

	template<class VertexDescriptor, class Graph>
	void initialize_vertex( VertexDescriptor v, Graph& g )
	{
		TUTTLE_LOG_TRACE( "initialize_vertex " << g[v] );
	}

	template<class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, Graph& g )
	{
		TUTTLE_LOG_TRACE( "discover_vertex " << g[v] << " outedges: " << out_degree( v, g ) );
	}

	template<class VertexDescriptor, class Graph>
	void examine_vertex( VertexDescriptor v, Graph& g )
	{
		TUTTLE_LOG_TRACE( "examine_vertex " << g[v] );
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		TUTTLE_LOG_TRACE( "finish_vertex " << g[v] );
	}

	template<class EdgeDescriptor, class Graph>
	void examine_edge( EdgeDescriptor e, Graph& g )
	{
		TUTTLE_LOG_TRACE( "examine_edge " << g[e] );
	}

	template<class EdgeDescriptor, class Graph>
	void tree_edge( EdgeDescriptor e, Graph& g )
	{
		TUTTLE_LOG_TRACE( "tree_edge " << g[e] );
	}

	template<class EdgeDescriptor, class Graph>
	void non_tree_edge( EdgeDescriptor e, Graph& g )
	{
		TUTTLE_LOG_TRACE( "non_tree_edge " << g[e] );
	}

	template<class EdgeDescriptor, class Graph>
	void gray_target( EdgeDescriptor e, Graph& g )
	{
		TUTTLE_LOG_TRACE( "gray_target " << g[e] );
	}

	template<class EdgeDescriptor, class Graph>
	void black_target( EdgeDescriptor e, Graph& g )
	{
		TUTTLE_LOG_TRACE( "black_target " << g[e] );
	}

};

}
}
}
}

#endif

