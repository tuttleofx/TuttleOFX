#ifndef _TUTTLE_VISITORS_HPP_
#define _TUTTLE_VISITORS_HPP_

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
	CycleDetector()
		: _hasCycle( false )
	{}

	template<class EdgeDescriptor, class Graph>
	void back_edge( EdgeDescriptor, const Graph& )
	{
		_hasCycle = true;
	}

public:
	bool _hasCycle;
};

template<class TGraph>
class MarkUsed : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;

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

		//TCOUT( "MarkUsed &&&&& init" << vertex.getName() );
		vertex.setUsed( false );
	}

	/**
	 * Set visited vertex used.
	 */
	template <class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		//COUT_VAR( vertex.getName() );
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
	typedef typename TGraph::Edge Edge;

	Test_dfs( TGraph& graph )
		: _graph( graph )
	{
		COUT_X( 80, "_" );
		TCOUT( "Test_dfs" );
	}

	~Test_dfs()
	{
		TCOUT( "~Test_dfs" );
		COUT_X( 80, "_" );
	}

	template <class VertexDescriptor, class Graph>
	void initialize_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TCOUT( "initialize_vertex: " << vertex );
	}

	template <class VertexDescriptor, class Graph>
	void start_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TCOUT( "start_vertex: " << vertex );
	}

	template <class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TCOUT( "discover_vertex: " << vertex );
	}

	template <class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, const Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TCOUT( "finish_vertex: " << vertex );
	}

	template<class EdgeDescriptor, class Graph>
	void examine_edge( EdgeDescriptor e, Graph& g )
	{
		Edge& edge = _graph.instance( e );

		//		Vertex& vertexSource = _graph.sourceInstance(e);
		//		Vertex& vertexDest   = _graph.targetInstance(e);

		TCOUT( "examine_edge: " << edge );
	}

	template <class EdgeDescriptor, class Graph>
	void tree_edge( EdgeDescriptor e, const Graph& g )
	{
		Edge& edge = _graph.instance( e );

		TCOUT( "tree_edge: " << edge  );
	}

	template <class EdgeDescriptor, class Graph>
	void back_edge( EdgeDescriptor e, const Graph& g )
	{
		Edge& edge = _graph.instance( e );

		TCOUT( "back_edge: " << edge  );
	}

	template <class EdgeDescriptor, class Graph>
	void forward_or_cross_edge( EdgeDescriptor e, const Graph& g )
	{
		Edge& edge = _graph.instance( e );

		TCOUT( "forward_or_cross_edge: " << edge );
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
		std::cout << "initialize_vertex "
		          << g[v] << std::endl;
	}

	template<class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, Graph& g )
	{
		std::cout << "discover_vertex "
		          << g[v]
		          << "  outedges: " << out_degree( v, g )
		          << std::endl;

	}

	template<class VertexDescriptor, class Graph>
	void examine_vertex( VertexDescriptor v, Graph& g )
	{
		std::cout << "examine_vertex "
		          << g[v] << std::endl;
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		std::cout << "finish_vertex "
		          << g[v] << std::endl;
	}

	template<class EdgeDescriptor, class Graph>
	void examine_edge( EdgeDescriptor e, Graph& g )
	{
		std::cout << "examine_edge "
		          << g[e] << std::endl;
	}

	template<class EdgeDescriptor, class Graph>
	void tree_edge( EdgeDescriptor e, Graph& g )
	{
		std::cout << "tree_edge "
		          << g[e] << std::endl;
	}

	template<class EdgeDescriptor, class Graph>
	void non_tree_edge( EdgeDescriptor e, Graph& g )
	{
		std::cout << "non_tree_edge "
		          << g[e] << std::endl;
	}

	template<class EdgeDescriptor, class Graph>
	void gray_target( EdgeDescriptor e, Graph& g )
	{
		std::cout << "gray_target "
		          << g[e] << std::endl;
	}

	template<class EdgeDescriptor, class Graph>
	void black_target( EdgeDescriptor e, Graph& g )
	{
		std::cout << "black_target "
		          << g[e] << std::endl;
	}

};

}
}
}
}

#endif

