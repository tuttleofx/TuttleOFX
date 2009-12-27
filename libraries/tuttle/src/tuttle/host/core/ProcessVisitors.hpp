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
		dfs_connectClips_visitor( TGraph & graph )
			: _graph(graph)
		{}

		template<class EdgeDescriptor, class Graph>
		void examine_edge( EdgeDescriptor e, Graph& g )
		{
			std::cout << "[CONNECT] examine_edge "
					  << get( vertex_properties, g )[target(e, g)]
					  << " TO "
					  << get( vertex_properties, g )[source(e, g)]
			          << std::endl;

			core::ProcessNode * sourceNode = get( vertex_properties, _graph )[source(e, _graph)].getProcessNode();
			core::ProcessNode * targetNode = get( vertex_properties, _graph )[target(e, _graph)].getProcessNode();
			sourceNode->connect( *targetNode );
		}
		
	private:
		TGraph & _graph;
};

template<class TGraph>
struct dfs_preCompute_visitor : public boost::dfs_visitor<>
{
	public:
		dfs_preCompute_visitor( TGraph graph, ProcessOptions & options )
			: _graph(graph)
			, _options(options)
		{}

		template<class VertexDescriptor, class Graph>
		void discover_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "[PRECOMPUTE] discover_vertex "
			          << get( vertex_properties, g )[v] << std::endl;

			get( vertex_properties, _graph )[v].getProcessNode()->preProcess_initialize(_options);
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "[PRECOMPUTE] finish_vertex "
			          << get( vertex_properties, g )[v] << std::endl;
			get( vertex_properties, _graph )[v].getProcessNode()->preProcess_finish(_options);
		}

	private:
		TGraph & _graph;
		ProcessOptions & _options;
};

template<class TGraph>
struct dfs_compute_visitor : public boost::dfs_visitor<>
{
	public:
		dfs_compute_visitor( TGraph& graph, const ProcessOptions & options )
			: _graph(graph)
			, _options(options)
		{}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "[COMPUTE] finish_vertex "
			          << get( vertex_properties, g )[v] << std::endl;

			get( vertex_properties, _graph )[v].getProcessNode()->process(_options);
		}

	private:
		TGraph & _graph;
		const ProcessOptions & _options;
};

template<class TGraph>
struct dfs_postCompute_visitor : public boost::dfs_visitor<>
{
	public:
		dfs_postCompute_visitor( TGraph& graph, ProcessOptions & options )
			: _graph(graph)
			, _options(options)
		{}

		template<class VertexDescriptor, class Graph>
		void initialize_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "[POSTCOMPUTE] initialize_vertex "
			          << get( vertex_properties, g )[v] << std::endl;
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "[POSTCOMPUTE] finish_vertex "
			          << get( vertex_properties, g )[v] << std::endl;
		}

	private:
		TGraph & _graph;
		ProcessOptions & _options;
};




}
}
}

#endif

