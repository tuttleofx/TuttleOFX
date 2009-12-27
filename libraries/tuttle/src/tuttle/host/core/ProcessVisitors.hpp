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
		dfs_connectClips_visitor( TGraph& graph )
			: _graph( graph.getGraph() )
		{}

		template<class EdgeDescriptor, class Graph>
		void examine_edge( EdgeDescriptor e, Graph& g )
		{
			std::cout << "[CONNECT] examine_edge "
			          << get( vertex_properties, g )[target( e, g )]
			          << " TO "
			          << get( vertex_properties, g )[source( e, g )]
			          << std::endl;

			core::ProcessNode* sourceNode = get( vertex_properties, _graph )[source( e, _graph )].getProcessNode();
			core::ProcessNode* targetNode = get( vertex_properties, _graph )[target( e, _graph )].getProcessNode();
			sourceNode->connect( *targetNode );
		}

	private:
		typename TGraph::GraphContainer& _graph;
};

template<class TGraph>
struct dfs_preCompute_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_preCompute_visitor( TGraph& graph, ProcessOptions& defaultOptions )
			: _graph(graph.getGraph())
			, _defaultOptions(defaultOptions)
		{}

		template<class VertexDescriptor, class Graph>
		void discover_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			std::cout << "[PRECOMPUTE] discover_vertex "
			          << vertex << std::endl;

			vertex.setProcessOptions( _defaultOptions );
			
			vertex.getProcessNode()->preProcess_initialize( vertex.getProcessOptions() );
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			std::cout << "[PRECOMPUTE] finish_vertex "
			          << vertex << std::endl;
			
			vertex.getProcessNode()->preProcess_finish( vertex.getProcessOptions() );
		}

	private:
		GraphContainer& _graph;
		ProcessOptions& _defaultOptions;
};

template<class TGraph>
struct dfs_compute_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_compute_visitor( TGraph& graph )
			: _graph(graph.getGraph())
		{}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			std::cout << "[COMPUTE] finish_vertex "
			          << vertex << std::endl;

			vertex.getProcessNode()->process( vertex.getProcessOptions() );
		}

	private:
		GraphContainer& _graph;
};

template<class TGraph>
struct dfs_postCompute_visitor : public boost::dfs_visitor<>
{
	public:
		typedef typename TGraph::GraphContainer GraphContainer;
		typedef typename TGraph::Vertex Vertex;

		dfs_postCompute_visitor( TGraph& graph )
			: _graph(graph.getGraph())
		{}

		template<class VertexDescriptor, class Graph>
		void initialize_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			std::cout << "[POSTCOMPUTE] initialize_vertex "
			          << vertex << std::endl;
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			Vertex& vertex = get( vertex_properties, _graph )[v];
			std::cout << "[POSTCOMPUTE] finish_vertex "
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

