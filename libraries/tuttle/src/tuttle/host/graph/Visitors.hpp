#ifndef _TUTTLE_VISITORS_HPP_
#define _TUTTLE_VISITORS_HPP_

#include <iostream>
#include <vector>
#include <boost/graph/properties.hpp>
#include <boost/graph/visitors.hpp>

namespace tuttle {
namespace host {
namespace graph {

//visitor: cycle detector
struct cycle_detector : public boost::dfs_visitor<>
{
	public:
		cycle_detector( bool& has_cycle )
			: _has_cycle( has_cycle )
		{
			_has_cycle = false;
		}

		template<class EdgeDescriptor, class Graph>
		void back_edge( EdgeDescriptor, Graph& )
		{
			_has_cycle = true;
		}

	protected:
		bool& _has_cycle;
};

//visitor: test_dfs
struct test_dfs_visitor : public boost::dfs_visitor<>
{
	public:
		test_dfs_visitor() {}

		template<class VertexDescriptor, class Graph>
		void initialize_vertex( VertexDescriptor v, Graph& g )
		{
			TCOUT("initialize_vertex: " << get( vertex_properties, g )[v]);
		}

		template<class VertexDescriptor, class Graph>
		void start_vertex( VertexDescriptor v, Graph& g )
		{
			TCOUT("start_vertex: " << get( vertex_properties, g )[v]);
		}

		template<class VertexDescriptor, class Graph>
		void discover_vertex( VertexDescriptor v, Graph& g )
		{
			TCOUT("discover_vertex: " << get( vertex_properties, g )[v]);
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			TCOUT("finish_vertex: " << get( vertex_properties, g )[v]);
		}

		template<class EdgeDescriptor, class Graph>
		void examine_edge( EdgeDescriptor e, Graph& g )
		{
			TCOUT("examine_edge: " << get( edge_properties, g )[e]);
		}

		template<class EdgeDescriptor, class Graph>
		void tree_edge( EdgeDescriptor e, Graph& g )
		{
			TCOUT("tree_edge: " << get( edge_properties, g )[e] << "  source: ");
			TCOUT(get( vertex_properties, g )[source( e, g )]);
		}

		template<class EdgeDescriptor, class Graph>
		void back_edge( EdgeDescriptor e, Graph& g )
		{
			TCOUT("back_edge: " << get( edge_properties, g )[e]);
		}

		template<class EdgeDescriptor, class Graph>
		void forward_or_cross_edge( EdgeDescriptor e, Graph& g )
		{
			TCOUT("forward_or_cross_edge: " << get( edge_properties, g )[e] );
		}

};

//visitor: test_bfs
struct test_bfs_visitor : public boost::bfs_visitor<>
{
	public:
		test_bfs_visitor() {}

		template<class VertexDescriptor, class Graph>
		void initialize_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "initialize_vertex "
			          << get( vertex_properties, g )[v] << std::endl;
		}

		template<class VertexDescriptor, class Graph>
		void discover_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "discover_vertex "
			          << get( vertex_properties, g )[v]
			          << "  outedges: " << out_degree( v, g )
			          << std::endl;

		}

		template<class VertexDescriptor, class Graph>
		void examine_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "examine_vertex "
			          << get( vertex_properties, g )[v] << std::endl;
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex( VertexDescriptor v, Graph& g )
		{
			std::cout << "finish_vertex "
			          << get( vertex_properties, g )[v] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void examine_edge( EdgeDescriptor e, Graph& g )
		{
			std::cout << "examine_edge "
			          << get( edge_properties, g )[e] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void tree_edge( EdgeDescriptor e, Graph& g )
		{
			std::cout << "tree_edge "
			          << get( edge_properties, g )[e] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void non_tree_edge( EdgeDescriptor e, Graph& g )
		{
			std::cout << "non_tree_edge "
			          << get( edge_properties, g )[e] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void gray_target( EdgeDescriptor e, Graph& g )
		{
			std::cout << "gray_target "
			          << get( edge_properties, g )[e] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void black_target( EdgeDescriptor e, Graph& g )
		{
			std::cout << "black_target "
			          << get( edge_properties, g )[e] << std::endl;
		}

};

}
}
}

#endif

