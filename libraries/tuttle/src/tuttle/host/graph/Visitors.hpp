#ifndef _TUTTLE_VISITORS_HPP_
#define _TUTTLE_VISITORS_HPP_

#include <iostream>
#include <vector>
#include <boost/graph/properties.hpp>
#include <boost/graph/visitors.hpp>


namespace tuttle{
	namespace host{
		namespace graph{

//visitor: cycle detector
struct cycle_detector: public boost::dfs_visitor<> {
	public:
		cycle_detector(bool& has_cycle) :
			_has_cycle(has_cycle) {
			_has_cycle = false;
		}

		template<class EdgeDescriptor, class Graph>
		void back_edge(EdgeDescriptor, Graph&) {
			_has_cycle = true;
		}

	protected:
		bool& _has_cycle;
};

/*
//visitor: leaves_detector
template<class VertexDesc >
struct leaves_detector: public boost::bfs_visitor<> {
	public:
	leaves_detector(std::vector<VertexDesc >& leaves)
		: _leaves(leaves) {}

		template<class VertexDescriptor, class Graph>
		void discover_vertex(VertexDescriptor v, Graph& g) {
			std::cout << out_degree(v, g) << std::endl;
			if(out_degree(v, g) == 0)
				_leaves.push_back(v);
		}

	protected:
		std::vector<VertexDesc >& _leaves;
};
*/

//visitor: test_dfs
struct test_dfs_visitor: public boost::dfs_visitor<> {
	public:
	test_dfs_visitor(){}

		template<class VertexDescriptor, class Graph>
		void initialize_vertex(VertexDescriptor v, Graph& g) {
			std::cout << "initialize_vertex "
				<< get(vertex_properties, g)[v] << std::endl;
		}

		template<class VertexDescriptor, class Graph>
		void start_vertex(VertexDescriptor v, Graph& g) {
			std::cout << "start_vertex "
				<< get(vertex_properties, g)[v] << std::endl;
		}

		template<class VertexDescriptor, class Graph>
		void discover_vertex(VertexDescriptor v, Graph& g) {
			std::cout << "discover_vertex "
				<< get(vertex_properties, g)[v] << std::endl;
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex(VertexDescriptor v, Graph& g) {
			std::cout << "finish_vertex "
				<< get(vertex_properties, g)[v] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void examine_edge(EdgeDescriptor e, Graph& g) {
			std::cout << "examine_edge "
				<< get(edge_properties, g)[e] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void tree_edge(EdgeDescriptor e, Graph& g) {
			std::cout << "tree_edge "
				<< get(edge_properties, g)[e]
				<< "  source: " ;
			std::cout << get(vertex_properties, g)[source(e,g)] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void back_edge(EdgeDescriptor e, Graph& g) {
			std::cout << "back_edge "
				<< get(edge_properties, g)[e] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void forward_or_cross_edge(EdgeDescriptor e, Graph& g) {
			std::cout << "forward_or_cross_edge "
				<< get(edge_properties, g)[e] << std::endl;
		}
};

//visitor: test_bfs
struct test_bfs_visitor: public boost::bfs_visitor<> {
	public:
	test_bfs_visitor(){}

		template<class VertexDescriptor, class Graph>
		void initialize_vertex(VertexDescriptor v, Graph& g) {
			std::cout << "initialize_vertex "
				<< get(vertex_properties, g)[v] << std::endl;
		}

		template<class VertexDescriptor, class Graph>
		void discover_vertex(VertexDescriptor v, Graph& g) {
			std::cout << "discover_vertex "
				<< get(vertex_properties, g)[v]
				<< "  outedges: " << out_degree(v, g)
				<< std::endl;

		}

		template<class VertexDescriptor, class Graph>
		void examine_vertex(VertexDescriptor v, Graph& g) {
			std::cout << "examine_vertex "
				<< get(vertex_properties, g)[v] << std::endl;
		}

		template<class VertexDescriptor, class Graph>
		void finish_vertex(VertexDescriptor v, Graph& g) {
			std::cout << "finish_vertex "
				<< get(vertex_properties, g)[v] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void examine_edge(EdgeDescriptor e, Graph& g) {
			std::cout << "examine_edge "
				<< get(edge_properties, g)[e] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void tree_edge(EdgeDescriptor e, Graph& g) {
			std::cout << "tree_edge "
				<< get(edge_properties, g)[e] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void non_tree_edge(EdgeDescriptor e, Graph& g) {
			std::cout << "non_tree_edge "
				<< get(edge_properties, g)[e] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void gray_target(EdgeDescriptor e, Graph& g) {
			std::cout << "gray_target "
				<< get(edge_properties, g)[e] << std::endl;
		}

		template<class EdgeDescriptor, class Graph>
		void black_target(EdgeDescriptor e, Graph& g) {
			std::cout << "black_target "
				<< get(edge_properties, g)[e] << std::endl;
		}
};

} // namespace graph
} // namespace host
} // namespace tuttle

#endif

