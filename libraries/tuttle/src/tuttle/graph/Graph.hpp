#ifndef _TUTTLE_GRAPH_HPP_
#define _TUTTLE_GRAPH_HPP_

#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/graphviz.hpp>

#include <boost/graph/visitors.hpp>
#include <boost/graph/depth_first_search.hpp>

// definition of basic boost::graph properties
enum vertex_properties_t { vertex_properties };
enum edge_properties_t { edge_properties };
namespace boost {
        BOOST_INSTALL_PROPERTY(vertex, properties);
        BOOST_INSTALL_PROPERTY(edge, properties);
}


namespace tuttle {


//visitor: cycle detector
struct cycle_detector : public boost::dfs_visitor<>
{
	public:
		cycle_detector( bool& has_cycle)
			: _has_cycle(has_cycle) {}

		template <class Edge, class Graph>
		void back_edge(Edge, Graph&) {
			_has_cycle = true;
		}

	protected:
		bool& _has_cycle;
};


// Directed acyclic graph
template < typename VERTEXPROPERTIES, typename EDGEPROPERTIES >
class Graph
{
	public:

		// Dag
		typedef boost::adjacency_list<
			boost::setS,			// disallow parallel edges
			boost::listS,			// vertex container
			boost::bidirectionalS,	// directed graph
			boost::property<vertex_properties_t, VERTEXPROPERTIES,
				boost::property<boost::vertex_index_t, unsigned int> >,
			boost::property<edge_properties_t, EDGEPROPERTIES>
		> GraphContainer;


		// a bunch of graph-specific typedefs
		typedef typename boost::graph_traits<GraphContainer>::vertex_descriptor Vertex;
		typedef typename boost::graph_traits<GraphContainer>::edge_descriptor Edge;

		typedef typename boost::graph_traits<GraphContainer>::vertex_iterator vertex_iter;
		typedef typename boost::graph_traits<GraphContainer>::edge_iterator edge_iter;
		typedef typename boost::graph_traits<GraphContainer>::adjacency_iterator adjacency_iter;
		typedef typename boost::graph_traits<GraphContainer>::out_edge_iterator out_edge_iter;

		typedef typename boost::graph_traits<GraphContainer>::degree_size_type degree_t;

		typedef std::pair<adjacency_iter, adjacency_iter> adjacency_vertex_range_t;
		typedef std::pair<out_edge_iter, out_edge_iter> out_edge_range_t;
		typedef std::pair<vertex_iter, vertex_iter> vertex_range_t;
		typedef std::pair<edge_iter, edge_iter> edge_range_t;


		// constructors etc.
		Graph()
		{}

		Graph(const Graph& g)
			: graph(g.graph)
		{}

		virtual ~Graph()
		{}


		// structure modification methods
		void Clear()
		{
			graph.clear();
		}

		Vertex AddVertex(const VERTEXPROPERTIES& prop)
		{
			Vertex v = add_vertex(graph);
			properties(v) = prop;

			vertexCount++;
			typename boost::property_map<GraphContainer, boost::vertex_index_t>::type param
				= get(boost::vertex_index, graph);
			param[v] = vertexCount;

			return v;
		}

		void RemoveVertex(const Vertex& v)
		{
			clear_vertex(v, graph);
			remove_vertex(v, graph);
		}

		Edge AddEdge(const Vertex& v1, const Vertex& v2, const EDGEPROPERTIES& prop)
		{
			Edge addedEdge = add_edge(v1, v2, graph).first;
			properties(addedEdge) = prop;

			return addedEdge;
		}

		// property access
		VERTEXPROPERTIES& properties(const Vertex& v)
		{
			typename boost::property_map<GraphContainer, vertex_properties_t>::type param = get(vertex_properties, graph);
			return param[v];
		}

		const VERTEXPROPERTIES& properties(const Vertex& v) const
		{
			typename boost::property_map<GraphContainer, vertex_properties_t>::const_type param = get(vertex_properties, graph);
			return param[v];
		}

		EDGEPROPERTIES& properties(const Edge& v)
		{
			typename boost::property_map<GraphContainer, edge_properties_t>::type param = get(edge_properties, graph);
			return param[v];
		}

		const EDGEPROPERTIES& properties(const Edge& v) const
		{
			typename boost::property_map<GraphContainer, edge_properties_t>::const_type param = get(edge_properties, graph);
			return param[v];
		}


		// selectors and properties
		const GraphContainer& getGraph() const
		{
			return graph;
		}

		vertex_range_t getVertices() const
		{
			return vertices(graph);
		}

		adjacency_vertex_range_t getAdjacentVertices(const Vertex& v) const
		{
			return adjacent_vertices(v, graph);
		}

		int getVertexCount() const
		{
			return num_vertices(graph);
		}

		int getEdgeCount() const
		{
			return num_edges(graph);
		}

		int getVertexDegree(const Vertex& v) const
		{
			return out_degree(v, graph);
		}


		// operators
		Graph& operator=(const Graph &g)
		{
			graph = g.graph;
			return *this;
		}

		bool has_cycle(){
			bool has_cycle = false;
			cycle_detector vis(has_cycle);
			boost::depth_first_search(graph, visitor(vis));
			std::cout << "The graph has a cycle? " << has_cycle << std::endl;
			return has_cycle;
		}


		// tools
		void exportAsDOT(const char * filename) const
		{
			std::map<std::string,std::string> graph_attr, vertex_attr, edge_attr;
			graph_attr["size"] 			= "6,6";
			graph_attr["rankdir"] 		= "LR";
			graph_attr["ratio"] 		= "fill";
			graph_attr["label"] 		= "TuttleOFX";
  			vertex_attr["shape"] 		= "circle";
  			vertex_attr["color"] 		= "dodgerblue4";
  			vertex_attr["fontcolor"] 	= "dodgerblue4";
  			edge_attr["style"] 			= "dashed";
  			edge_attr["minlen"] 		= "1";
  			edge_attr["color"] 			= "darkslategray";
  			edge_attr["fontcolor"] 		= "darkslategray";

			std::ofstream ofs(filename);
			write_graphviz( ofs
						  , graph
						  , boost::make_label_writer(get(vertex_properties, graph))
						  , boost::make_label_writer(get(edge_properties, graph))
						  , boost::make_graph_attributes_writer(graph_attr, vertex_attr, edge_attr) );
		}


	protected:

        GraphContainer graph;
        unsigned int vertexCount;
};

} // namespace tuttle

#endif

