#ifndef _TUTTLE_INTERNALGRAPH_HPP_
#define _TUTTLE_INTERNALGRAPH_HPP_

#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/foreach.hpp>

#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/graph/dominator_tree.hpp>

// definition of basic boost::graph properties
enum vertex_properties_t { vertex_properties };
enum edge_properties_t { edge_properties };
namespace boost {
BOOST_INSTALL_PROPERTY( vertex, properties );
BOOST_INSTALL_PROPERTY( edge, properties );
}

// include this file after the definition of basic boost::graph properties
#include <tuttle/host/graph/Visitors.hpp>

namespace tuttle {
namespace host {
namespace graph {

template < typename VERTEX, typename EDGE >
class InternalGraph
{
public:
	// Directed acyclic graph
	typedef boost::adjacency_list<
	    boost::setS,                // disallow parallel edges
	    boost::listS,               // vertex container
	    boost::bidirectionalS,      // directed graph
	    boost::property<boost::vertex_index_t, int,
						//boost::property<boost::vertex_color_t, boost::default_color_type,
	                    boost::property<vertex_properties_t, VERTEX > >,
	    boost::property<edge_properties_t, EDGE>
	    > GraphContainer;

	// a bunch of graph-specific typedefs
	typedef typename boost::graph_traits<GraphContainer>::vertex_descriptor VertexDescriptor;
	typedef typename boost::graph_traits<GraphContainer>::edge_descriptor EdgeDescriptor;

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
	InternalGraph() : _count( 0 )
	{}

	InternalGraph( const InternalGraph& g )
	{
		*this = g; ///< using operator=
	}

	// operators
	InternalGraph& operator=( const InternalGraph& g )
	{
		if( this == &g )
			return *this;

		boost::copy_graph( g._graph, _graph );
		_count = g._count;
		return *this;
	}

	virtual ~InternalGraph()
	{}

	// structure modification methods
	void clear()
	{
		_graph.clear();
	}

	VertexDescriptor addVertex( const VERTEX& prop )
	{
		VertexDescriptor v = add_vertex( _graph );

		instance( v )                         = prop;
		get( boost::vertex_index, _graph )[v] = _count++;

		return v;
	}

	void removeVertex( const VertexDescriptor& v )
	{
		clear_vertex( v, _graph );
		remove_vertex( v, _graph );
	}

	EdgeDescriptor addEdge( const VertexDescriptor& v1, const VertexDescriptor& v2, const EDGE& prop )
	{
		EdgeDescriptor addedEdge = add_edge( v1, v2, _graph ).first;

		instance( addedEdge ) = prop;

		if( has_cycle() )
		{
			throw std::invalid_argument( "graph: cycle detected" );
		}

		return addedEdge;
	}

	// property access
	VERTEX& instance( const VertexDescriptor& v )
	{
		typename boost::property_map<GraphContainer, vertex_properties_t>::type param = get( vertex_properties, _graph );
		return param[v];
	}

	const VERTEX& instance( const VertexDescriptor& v ) const
	{
		typename boost::property_map<GraphContainer, vertex_properties_t>::const_type param = get( vertex_properties, _graph );
		return param[v];
	}

	EDGE& instance( const EdgeDescriptor& v )
	{
		typename boost::property_map<GraphContainer, edge_properties_t>::type param = get( edge_properties, _graph );
		return param[v];
	}

	const EDGE& instance( const EdgeDescriptor& v ) const
	{
		typename boost::property_map<GraphContainer, edge_properties_t>::const_type param = get( edge_properties, _graph );
		return param[v];
	}

	// selectors
	const GraphContainer& getGraph() const
	{
		return _graph;
	}

	vertex_range_t getVertices() const
	{
		return vertices( _graph );
	}

	adjacency_vertex_range_t getAdjacentVertices( const VertexDescriptor& v ) const
	{
		return adjacent_vertices( v, _graph );
	}

	int getVertexCount() const
	{
		return num_vertices( _graph );
	}

	int getEdgeCount() const
	{
		return num_edges( _graph );
	}

	int getVertexDegree( const VertexDescriptor& v ) const
	{
		return out_degree( v, _graph );
	}

	bool has_cycle()
	{
		// we use a depth first search visitor
		bool has_cycle = false;
		cycle_detector vis( has_cycle );

		boost::depth_first_search( _graph, visitor( vis ) );
		return has_cycle;
	}

	void dumpToStdOut()
	{
		std::cout
		<< "internalGraph dump" << std::endl
		<< "\tvertex count: " << getVertexCount() << std::endl
		<< "\tedge count: " << getEdgeCount() << std::endl;
	}

	template<class Visitor>
	void dfs(Visitor vis, const VertexDescriptor& vroot)
	{
		std::vector<boost::default_color_type > colormap(boost::num_vertices(_graph));
		boost::depth_first_visit( _graph
				, vroot
				, vis
				, boost::make_iterator_property_map(colormap.begin(), boost::get(boost::vertex_index, _graph))
		);
		/*
		vertex_iter uItr;
		vertex_iter uEnd;
		boost::tie( uItr, uEnd ) = vertices( _graph );
		++uItr;
		++uItr;
		const VertexDescriptor& vroot = *uItr;

		std::vector<boost::default_color_type > colormap(boost::num_vertices(_graph));
		boost::depth_first_visit( _graph
				, vroot
				, vis
				, boost::make_iterator_property_map(colormap.begin(), boost::get(boost::vertex_index, _graph))
		);
		*/
	}

	template<class Visitor>
	void dfs(Visitor vis)
	{
		boost::depth_first_search( _graph, visitor(vis) );
	}

	void bfs( const VertexDescriptor& vroot )
	{
		test_bfs_visitor vis;
		boost::breadth_first_search( _graph, vroot, visitor( vis ) );
	}

	void transpose()
	{
		GraphContainer g;
		boost::transpose_graph(_graph, g);
		_graph = g;
	}

	void toDominatorTree()
	{
		typedef typename boost::property_map<GraphContainer, boost::vertex_index_t>::type IndexMap;
		typedef typename std::vector<VertexDescriptor >::iterator VectorDescIter;
		typedef typename boost::iterator_property_map<VectorDescIter, IndexMap > PredMap;

		std::vector<VertexDescriptor> domTreePredVector;
		IndexMap indexMap( get( boost::vertex_index, _graph ) );
		vertex_iter uItr;
		vertex_iter uEnd;
		int j = 0;
		for( boost::tie( uItr, uEnd ) = vertices( _graph ); uItr != uEnd; ++uItr, ++j )
		{
			put( indexMap, *uItr, j );
		}

		// Lengauer-Tarjan dominator tree algorithm
		domTreePredVector = std::vector<VertexDescriptor>( num_vertices( _graph ), boost::graph_traits<GraphContainer>::null_vertex() );
		PredMap domTreePredMap =
		    boost::make_iterator_property_map( domTreePredVector.begin(), indexMap );

		boost::lengauer_tarjan_dominator_tree( _graph, vertex( 0, _graph ), domTreePredMap );

		for( boost::tie( uItr, uEnd ) = vertices( _graph ); uItr != uEnd; ++uItr )
			boost::clear_vertex( *uItr, _graph );

		for( boost::tie( uItr, uEnd ) = vertices( _graph ); uItr != uEnd; ++uItr )
		{
			if( get( domTreePredMap, *uItr ) != boost::graph_traits<GraphContainer>::null_vertex() )
			{
				add_edge( get( domTreePredMap, *uItr ), *uItr, _graph );
				//get(indexMap, *uItr) indice du vertex courant
				//get(domTreePredMap, *uItr) descriptor du dominator
				//get(indexMap, get(domTreePredMap, *uItr)) indice du dominator
			}
		}
	}

	std::vector<VertexDescriptor > leaves()
	{

		std::vector<VertexDescriptor > vleaves;
		vertex_range_t vrange = getVertices();
		for( vertex_iter it = vrange.first; it != vrange.second; ++it )
			if( out_degree( *it, _graph ) == 0 )
				vleaves.push_back( *it );

		return vleaves;
	}

protected:
	GraphContainer _graph;
	int _count; // for vertex_index

};

} // namespace graph
} // namespace host
} // namespace tuttle

#endif
