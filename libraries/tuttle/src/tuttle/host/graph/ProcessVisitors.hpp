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
struct dfs_connectClips_visitor : public boost::dfs_visitor<>
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;

	dfs_connectClips_visitor( TGraph& graph )
		: _graph( graph.getGraph() )
	{}

	template<class EdgeDescriptor, class Graph>
	void examine_edge( EdgeDescriptor e, Graph& g )
	{
		Edge& edge = get( edge_properties, _graph )[e];
		Vertex& vertexSource = get( vertex_properties, _graph )[source( e, _graph )];
		Vertex& vertexDest   = get( vertex_properties, _graph )[target( e, _graph )];

		if( vertexDest.isFake() || vertexSource.isFake() )
			return;

	  TCOUT( "examine_edge" << vertexSource );
		TCOUT("[CONNECT] examine_edge "
			  << vertexSource
			  << " TO "
			  << vertexDest << "." << edge.inAttrName() );

		ProcessNode& sourceNode = *vertexSource.getProcessNode();
		ProcessNode& targetNode = *vertexDest.getProcessNode();
		sourceNode.connect( targetNode, sourceNode.getAttribute( edge.inAttrName() ) );
	}

	template <class VertexDescriptor, class Graph>
	void initialize_vertex(VertexDescriptor v, const Graph& g) {
		Vertex& vertex = get( vertex_properties, _graph )[v];
	  TCOUT( vertex << " : " << "initialize_vertex" );
	}
	template <class VertexDescriptor, class Graph>
	void start_vertex(VertexDescriptor v, const Graph& g) {
		Vertex& vertex = get( vertex_properties, _graph )[v];
	  TCOUT( vertex << " : " << "start_vertex" );
	}
	template <class VertexDescriptor, class Graph>
	void discover_vertex(VertexDescriptor v, const Graph& g) {
		Vertex& vertex = get( vertex_properties, _graph )[v];
	  TCOUT( vertex << " : " << "discover_vertex" );
	}
//	template <class Edge, class Graph>
//	void examine_edge(Edge u, const Graph& g) {
//	  TCOUT( u << " : " <<BOOST_CURRENT_FUNCTION );
//	}
	template <class EdgeDescriptor, class Graph>
	void tree_edge(EdgeDescriptor e, const Graph& g) {
		Edge& edge = get( edge_properties, _graph )[e];
	  TCOUT( edge << " : " << "tree_edge" );
	}
	template <class EdgeDescriptor, class Graph>
	void back_edge(EdgeDescriptor e, const Graph& g) {
		Edge& edge = get( edge_properties, _graph )[e];
	  TCOUT( edge << " : " << "back_edge" );
	}
	template <class EdgeDescriptor, class Graph>
	void forward_or_cross_edge(EdgeDescriptor e, const Graph& g) {
		Edge& edge = get( edge_properties, _graph )[e];
	  TCOUT( edge << " : " << "forward_or_cross_edge" );
	}
	template <class VertexDescriptor, class Graph>
	void finish_vertex(VertexDescriptor v, const Graph& g) {
		Vertex& vertex = get( vertex_properties, _graph )[v];
	  TCOUT( vertex << " : " << "finish_vertex" );
	}

private:
	GraphContainer& _graph;
};

template<class TGraph>
struct dfs_preProcess_finish_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_preProcess_finish_visitor( TGraph& graph, ProcessOptions& defaultOptions )
			: _graph(graph.getGraph())
			, _defaultOptions(defaultOptions)
		{}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			if( vertex.isFake() )
				return;

			vertex.setProcessOptions( _defaultOptions );
			vertex.getProcessNode()->preProcess_finish( vertex.getProcessOptions() );
		}

	private:
		GraphContainer& _graph;
		ProcessOptions& _defaultOptions;
};

template<class TGraph>
struct dfs_preProcess_initialize_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_preProcess_initialize_visitor( TGraph& graph, ProcessOptions& defaultOptions )
			: _graph(graph.getGraph())
			, _defaultOptions(defaultOptions)
		{}

		template<class VertexDescriptor, class Graph>
		void discover_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			TCOUT("[PREPROCESS] discover_vertex " << vertex);
			if( vertex.isFake() )
				return;

			vertex.getProcessNode()->preProcess_initialize( vertex.getProcessOptions() );
		}

	private:
		GraphContainer& _graph;
		ProcessOptions& _defaultOptions;
};

template<class TGraph>
struct dfs_process_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_process_visitor( TGraph& graph )
			: _graph(graph.getGraph())
		{}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			COUT( "[PROCESS] finish_vertex " << vertex );
			if( vertex.isFake() )
				return;

			vertex.getProcessNode()->process( vertex.getProcessOptions() );
		}

	private:
		GraphContainer& _graph;
};

template<class TGraph>
struct dfs_postProcess_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_postProcess_visitor( TGraph& graph )
			: _graph(graph.getGraph())
		{}

		template<class VertexDescriptor, class Graph>
		void initialize_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			TCOUT("[POSTPROCESS] initialize_vertex " << vertex);
			if( vertex.isFake() )
				return;
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			TCOUT("[POSTPROCESS] finish_vertex " << vertex);
			if( vertex.isFake() )
				return;

			vertex.getProcessNode()->postProcess( vertex.getProcessOptions() );
		}

	private:
		GraphContainer& _graph;
};

}
}
}

#endif

