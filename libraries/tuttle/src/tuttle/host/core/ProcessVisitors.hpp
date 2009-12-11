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

struct dfs_compute_visitor : public boost::dfs_visitor<>
{
	public:
		dfs_compute_visitor(const ProcessOptions & options)
			: _options(options)
		{}

		template<class VertexDescriptor, class Graph>
		void initialize_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "[PROCESS] initialize_vertex "
			          << get( vertex_properties, g )[v] << std::endl;

			get( vertex_properties, g )[v].processNode()->process(_options);

		}

	private:
		const ProcessOptions & _options;
};

} // namespace core
} // namespace host
} // namespace tuttle

#endif

