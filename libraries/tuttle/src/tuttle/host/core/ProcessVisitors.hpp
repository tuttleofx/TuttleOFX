#ifndef _TUTTLE_PROCESSVISITORS_HPP_
#define _TUTTLE_PROCESSVISITORS_HPP_

#include "ProcessOptions.hpp"
#include <iostream>
#include <vector>
#include <boost/graph/properties.hpp>
#include <boost/graph/visitors.hpp>

namespace tuttle {
namespace host {
namespace core {

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
			std::cout << "[CONNECT] examine_edge "
			          << vertexSource
			          << " TO "
			          << vertexDest << "." << edge.inAttrName()
			          << std::endl;

			core::ProcessNode* sourceNode = vertexSource.getProcessNode();
			core::ProcessNode* targetNode = vertexDest.getProcessNode();
			sourceNode->connect( *targetNode, sourceNode->getProcessAttribute( edge.inAttrName() ) );
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
			std::cout << "[PREPROCESS] discover_vertex "
			          << vertex << std::endl;

			vertex.setProcessOptions( _defaultOptions );

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
			std::cout << "[PROCESS] finish_vertex "
			          << vertex << std::endl;

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
			std::cout << "[POSTPROCESS] initialize_vertex "
			          << vertex << std::endl;
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			std::cout << "[POSTPROCESS] finish_vertex "
			          << vertex << std::endl;

			vertex.getProcessNode()->postProcess( vertex.getProcessOptions() );
		}

	private:
		GraphContainer& _graph;
};

}
}
}

#endif

