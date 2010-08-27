#ifndef _TUTTLE_PROCESSVISITORS_HPP_
#define _TUTTLE_PROCESSVISITORS_HPP_

#include "ProcessOptions.hpp"
#include <iostream>
#include <vector>
#include <boost/graph/properties.hpp>
#include <boost/graph/visitors.hpp>

namespace tuttle {
namespace host {
namespace graph {

template<class TGraph>
struct dfs_test_visitor : public boost::dfs_visitor<>
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;

	dfs_test_visitor( TGraph& graph )
		: _graph( graph )
	{
		COUT_X( 80, "_" );
		TCOUT( "dfs_test_visitor" );
	}

	~dfs_test_visitor()
	{
		TCOUT( "~dfs_test_visitor" );
		COUT_X( 80, "_" );
	}

	template<class EdgeDescriptor, class Graph>
	void examine_edge( EdgeDescriptor e, Graph& g )
	{
		Edge& edge = _graph.instance(e);
		Vertex& vertexSource = _graph.sourceInstance(e);
		Vertex& vertexDest   = _graph.targetInstance(e);

		TCOUT( edge << " : " << "examine_edge" );
	}

	template <class VertexDescriptor, class Graph>
	void initialize_vertex(VertexDescriptor v, const Graph& g)
	{
		Vertex& vertex = _graph.instance(v);
		TCOUT( vertex << " : " << "initialize_vertex" );
	}
	template <class VertexDescriptor, class Graph>
	void start_vertex(VertexDescriptor v, const Graph& g)
	{
		Vertex& vertex = _graph.instance(v);
		TCOUT( vertex << " : " << "start_vertex" );
	}
	template <class VertexDescriptor, class Graph>
	void discover_vertex(VertexDescriptor v, const Graph& g)
	{
		Vertex& vertex = _graph.instance(v);
		TCOUT( vertex << " : " << "discover_vertex" );
	}
//	template <class Edge, class Graph>
//	void examine_edge(Edge e, const Graph& g)
//	{
//		TCOUT( u << " : " <<BOOST_CURRENT_FUNCTION );
//	}
	template <class EdgeDescriptor, class Graph>
	void tree_edge(EdgeDescriptor e, const Graph& g)
	{
		Edge& edge = _graph.instance(e);
		TCOUT( edge << " : " << "tree_edge" );
	}
	template <class EdgeDescriptor, class Graph>
	void back_edge(EdgeDescriptor e, const Graph& g)
	{
		Edge& edge = _graph.instance(e);
		TCOUT( edge << " : " << "back_edge" );
	}
	template <class EdgeDescriptor, class Graph>
	void forward_or_cross_edge(EdgeDescriptor e, const Graph& g)
	{
		Edge& edge = _graph.instance(e);
		TCOUT( edge << " : " << "forward_or_cross_edge" );
	}
	template <class VertexDescriptor, class Graph>
	void finish_vertex(VertexDescriptor v, const Graph& g)
	{
		Vertex& vertex = _graph.instance(v);
		TCOUT( vertex << " : " << "finish_vertex" );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
struct dfs_connectClips_visitor : public boost::dfs_visitor<>
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;

	dfs_connectClips_visitor( TGraph& graph )
		: _graph( graph )
	{}

	template<class EdgeDescriptor, class Graph>
	void examine_edge( EdgeDescriptor e, Graph& g )
	{
		Edge& edge = _graph.instance(e);
		Vertex& vertexSource = _graph.sourceInstance(e);
		Vertex& vertexDest   = _graph.targetInstance(e);

		if( vertexDest.isFake() || vertexSource.isFake() )
			return;

		TCOUT( "examine_edge" << vertexSource );
		TCOUT("[CONNECT] examine_edge "
			<< vertexSource
			<< " TO "
			<< vertexDest << "." << edge.inAttrName() );

		Node& sourceNode = *vertexSource.getProcessNode();
		Node& targetNode = *vertexDest.getProcessNode();
		sourceNode.connect( targetNode, sourceNode.getAttribute( edge.inAttrName() ) );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
struct dfs_preProcess1_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_preProcess1_visitor( TGraph& graph )
			: _graph(graph)
		{}

		template<class VertexDescriptor, class Graph>
		void discover_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = _graph.instance(v);
			TCOUT( "[PREPROCESS 1] discover_vertex " << vertex );
			if( vertex.isFake() )
				return;

			vertex.getProcessNode()->preProcess1_initialize( vertex.getProcessOptions() );
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = _graph.instance(v);
			TCOUT( "[PREPROCESS 1] finish_vertex " << vertex );
			if( vertex.isFake() )
				return;

			vertex.getProcessNode()->preProcess1_finish( vertex.getProcessOptions() );
		}

	private:
		TGraph& _graph;
};

template<class TGraph>
struct dfs_preProcess2_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_preProcess2_visitor( TGraph& graph )
			: _graph(graph)
		{}

		template<class VertexDescriptor, class Graph>
		void discover_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = _graph.instance(v);
			TCOUT( "[PREPROCESS 2] discover_vertex " << vertex );
			if( vertex.isFake() )
				return;

			vertex.getProcessNode()->preProcess2_initialize( vertex.getProcessOptions() );
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = _graph.instance(v);
			TCOUT( "[PREPROCESS 2] finish_vertex " << vertex );
			if( vertex.isFake() )
				return;

			vertex.getProcessNode()->preProcess2_finish( vertex.getProcessOptions() );
		}

	private:
		TGraph& _graph;
};

template<class TGraph>
struct dfs_preProcess3_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_preProcess3_visitor( TGraph& graph )
			: _graph(graph)
		{}

		template<class VertexDescriptor, class Graph>
		void discover_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = _graph.instance(v);
			TCOUT( "[PREPROCESS 3] discover_vertex " << vertex );
			if( vertex.isFake() )
				return;

			vertex.getProcessNode()->preProcess3_initialize( vertex.getProcessOptions() );
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = _graph.instance(v);
			TCOUT( "[PREPROCESS 3] finish_vertex " << vertex );
			if( vertex.isFake() )
				return;

			vertex.getProcessNode()->preProcess3_finish( vertex.getProcessOptions() );
		}

	private:
		TGraph& _graph;
};


template<class TGraph>
struct dfs_optimizeGraph_visitor : public boost::dfs_visitor<>
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;
	typedef typename TGraph::vertex_descriptor vertex_descriptor;
	typedef typename TGraph::edge_descriptor edge_descriptor;
	typedef typename TGraph::in_edge_iterator in_edge_iterator;
	typedef typename TGraph::out_edge_iterator out_edge_iterator;

	dfs_optimizeGraph_visitor( TGraph& graph )
		: _graph( graph )
	{
		COUT_X( 80, ":" );
	}

	template <class VertexDescriptor, class Graph>
	void finish_vertex(VertexDescriptor v, const Graph& g)
	{
		using namespace boost;
		using namespace boost::graph;
		Vertex& vertex = _graph.instance(v);

		out_edge_iterator oe, oeEnd;
		tie(oe, oeEnd) = out_edges( v, _graph.getGraph() );
//		in_edge_iterator ie, ieEnd;
//		tie(ie, ieEnd) = in_edges( v, _graph.getGraph() );
		ProcessOptions& procOptions = vertex.getProcessOptions();

		// compute global infos for inputs

		// direct dependencies (originally the node inputs)
		procOptions._nbInputs = _graph.getOutDegree( v );
		for( ; oe != oeEnd; ++oe )
		{
//			Edge& outEdge = _graph.instance(*oe);
			Vertex& outVertex = _graph.targetInstance(*oe);
			procOptions._globalInfos += outVertex.getProcessOptions()._localInfos;
		}

//		// direct node usages (originally the node outputs)
		procOptions._nbOutputs = _graph.getInDegree( v );
//		for( ; ie != ieEnd; ++ie )
//		{
//			Edge& e = _graph.instance(*ie);
//		}

		if( ! vertex.isFake() )
		{
			// compute local infos, need to be a real node !
			vertex.getProcessNode()->preProcess_infos( procOptions._localInfos );
		}

		COUT_X( 80, "." );
		COUT( vertex.getName() );
		COUT( procOptions );
		COUT_X( 80, "." );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
struct dfs_process_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_process_visitor( TGraph& graph )
			: _graph(graph)
		{}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = _graph.instance(v);
			COUT( "[PROCESS] finish_vertex " << vertex );
			if( vertex.isFake() )
				return;

			vertex.getProcessNode()->process( vertex.getProcessOptions() );
		}

	private:
		TGraph& _graph;
};

template<class TGraph>
struct dfs_postProcess_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_postProcess_visitor( TGraph& graph )
			: _graph(graph)
		{}

		template<class VertexDescriptor, class Graph>
		void initialize_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = _graph.instance(v);
			TCOUT("[POSTPROCESS] initialize_vertex " << vertex);
			if( vertex.isFake() )
				return;
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = _graph.instance(v);
			TCOUT("[POSTPROCESS] finish_vertex " << vertex);
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

#endif

