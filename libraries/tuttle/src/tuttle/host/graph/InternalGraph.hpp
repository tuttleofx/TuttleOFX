#ifndef _TUTTLE_INTERNALGRAPH_HPP_
#define _TUTTLE_INTERNALGRAPH_HPP_

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/graph/dominator_tree.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>

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
	typedef VERTEX Vertex;
	typedef EDGE Edge;
	typedef InternalGraph<Vertex, Edge> This;

	// Directed acyclic graph
	typedef boost::adjacency_list<
	    boost::setS,                // disallow parallel edges
	    boost::listS,               // vertex container
	    boost::bidirectionalS,      // directed graph
	    boost::property<boost::vertex_index_t, int,
	                    //boost::property<boost::vertex_color_t, boost::default_color_type,
	                    boost::property<vertex_properties_t, Vertex > >,
	    boost::property<edge_properties_t, Edge>
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

	InternalGraph( const This& g )
	{
		*this = g; // using operator=
	}

	// operators
	This& operator=( const This& g )
	{
		if( this == &g )
			return *this;

		boost::copy_graph( g._graph, _graph );
		_count = g._count;
		rebuildVertexDescriptorMap();
		return *this;
	}

	virtual ~InternalGraph()
	{}

	// structure modification methods
	void clear()
	{
		_graph.clear();
		_vertexDescriptorMap.clear();
	}

	VertexDescriptor addVertex( const Vertex& prop )
	{
		VertexDescriptor v = add_vertex( _graph );

		instance( v )                         = prop;
		get( boost::vertex_index, _graph )[v] = _count++;

		_vertexDescriptorMap[prop.getName()] = v;

		return v;
	}

	void removeVertex( const VertexDescriptor& v )
	{
		_vertexDescriptorMap.erase( instance( v ).getName() );
		clear_vertex( v, _graph );
		remove_vertex( v, _graph );
	}

	EdgeDescriptor addEdge( const VertexDescriptor& v1, const VertexDescriptor& v2, const Edge& prop )
	{
		EdgeDescriptor addedEdge = add_edge( v1, v2, _graph ).first;

		instance( addedEdge ) = prop;

		if( has_cycle() )
		{
			throw std::invalid_argument( "graph: cycle detected" );
		}

		return addedEdge;
	}

	VertexDescriptor& getVertexDescriptor( const std::string& vertexName )
	{
		return _vertexDescriptorMap[vertexName];
	}

	const VertexDescriptor& getVertexDescriptor( const std::string& vertexName ) const
	{
		return _vertexDescriptorMap[vertexName];
	}

	Vertex& getVertex( const std::string& vertexName )
	{
		return instance( _vertexDescriptorMap[vertexName] );
	}

	const Vertex& getVertex( const std::string& vertexName ) const
	{
		return instance( _vertexDescriptorMap[vertexName] );
	}

	// property access
	Vertex& instance( const VertexDescriptor& v )
	{
		//typename boost::property_map<GraphContainer, vertex_properties_t>::type param = get( vertex_properties, _graph );
		//return param[v];
		return get( vertex_properties, _graph )[v];
	}

	const Vertex& instance( const VertexDescriptor& v ) const
	{
		//typename boost::property_map<GraphContainer, vertex_properties_t>::const_type param = get( vertex_properties, _graph );
		//return param[v];
		return get( vertex_properties, _graph )[v];
	}

	Edge& instance( const EdgeDescriptor& e )
	{
		//typename boost::property_map<GraphContainer, edge_properties_t>::type param = get( edge_properties, _graph );
		//return param[e];
		return get( edge_properties, _graph )[e];
	}

	const Edge& instance( const EdgeDescriptor& e ) const
	{
		//typename boost::property_map<GraphContainer, edge_properties_t>::const_type param = get( edge_properties, _graph );
		//return param[e];
		return get( edge_properties, _graph )[e];
	}

	GraphContainer& getGraph()
	{
		return _graph;
	}

	const GraphContainer& getGraph() const
	{
		return _graph;
	}

	edge_range_t getEdges() const
	{
		return edges( _graph );
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

	template<class Visitor>
	void dfs( Visitor vis, const VertexDescriptor& vroot )
	{
		std::vector<boost::default_color_type > colormap( boost::num_vertices( _graph ) );
		boost::depth_first_visit( _graph,
		                          vroot,
		                          vis,
		                          boost::make_iterator_property_map( colormap.begin(), boost::get( boost::vertex_index, _graph ) )
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
	void dfs( Visitor vis )
	{
		boost::depth_first_search( _graph, visitor( vis ) );
	}

	void bfs( const VertexDescriptor& vroot )
	{
		test_bfs_visitor vis;

		boost::breadth_first_search( _graph, vroot, visitor( vis ) );
	}

	void copyTransposed( const InternalGraph& g )
	{
		// make a transposed copy of g in _graph
		boost::transpose_graph( g._graph, _graph );
		rebuildVertexDescriptorMap();
	}

	void copy( const InternalGraph& g )
	{
		// make a transposed copy of g in _graph
		boost::copy_graph( g._graph, _graph );
		rebuildVertexDescriptorMap();
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

	template< typename Vertex, typename Edge >
	friend std::ostream& operator<<( std::ostream& os, const This& g );
	
private:
	void rebuildVertexDescriptorMap()
	{
		_vertexDescriptorMap.clear();
		for( vertex_iter i = vertices( getGraph() ).first, iEnd = vertices( getGraph() ).second;
		     i != iEnd;
		     ++i )
		{
			//			TCOUT( "pp: "<< get(vertex_properties, graphT.getGraph())[*i]._name );
			//			TCOUT( "pp: "<< graphT.getGraph()[*i]._name ); // if no boost::property_map
			_vertexDescriptorMap[get( vertex_properties, getGraph() )[*i].getName()] = *i;
		}
	}

protected:
	GraphContainer _graph;
	std::map<std::string, VertexDescriptor> _vertexDescriptorMap;
	int _count; // for vertex_index

};

}
}
}

#include "InternalGraph.tcc"

#endif
