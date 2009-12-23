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


struct dfs_connectClips_visitor : public boost::dfs_visitor<>
{
	public:
		dfs_connectClips_visitor(const ProcessOptions & options)
			: _options(options)
		{}

		template<class EdgeDescriptor, class Graph>
		void examine_edge( EdgeDescriptor e, Graph& g )
		{
			std::cout << "[CONNECT] examine_edge "
					  << get( vertex_properties, g )[target(e, g)]
					  << " TO "
					  << get( vertex_properties, g )[source(e, g)]
			          << std::endl;

			core::ProcessNode * sourceNode = get( vertex_properties, g )[source(e, g)].processNode();
			core::ProcessNode * targetNode = get( vertex_properties, g )[target(e, g)].processNode();
			sourceNode->connect( *targetNode );
		}
		
	private:
		const ProcessOptions & _options;
};

struct dfs_preCompute_visitor : public boost::dfs_visitor<>
{
	public:
		dfs_preCompute_visitor(const ProcessOptions & options)
			: _options(options)
		{}

		template<class VertexDescriptor, class Graph>
		void discover_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "[PRECOMPUTE] discover_vertex "
			          << get( vertex_properties, g )[v] << std::endl;

			get( vertex_properties, g )[v].processNode()->preProcess_initialize(_options);
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "[PRECOMPUTE] finish_vertex "
			          << get( vertex_properties, g )[v] << std::endl;
			get( vertex_properties, g )[v].processNode()->preProcess_finish(_options);
		}

	private:
		const ProcessOptions & _options;
};

struct dfs_compute_visitor : public boost::dfs_visitor<>
{
	public:
		dfs_compute_visitor(const ProcessOptions & options)
			: _options(options)
		{}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "[COMPUTE] finish_vertex "
			          << get( vertex_properties, g )[v] << std::endl;

			get( vertex_properties, g )[v].processNode()->process(_options);
		}

	private:
		const ProcessOptions & _options;
};

struct dfs_postCompute_visitor : public boost::dfs_visitor<>
{
	public:
		dfs_postCompute_visitor(const ProcessOptions & options)
			: _options(options)
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
		const ProcessOptions & _options;
};




} // namespace core
} // namespace host
} // namespace tuttle

#endif

