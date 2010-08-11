#ifndef _TUTTLE_INTERNALGRAPH_HPP_
#define _TUTTLE_INTERNALGRAPH_HPP_

#include <tuttle/host/Exception.hpp>

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
#include <boost/exception/all.hpp>

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
	typedef typename boost::graph_traits<GraphContainer>::vertex_descriptor vertex_descriptor;
	typedef typename boost::graph_traits<GraphContainer>::edge_descriptor edge_descriptor;

	typedef typename boost::graph_traits<GraphContainer>::vertex_iterator vertex_iterator;
	typedef typename boost::graph_traits<GraphContainer>::edge_iterator edge_iterator;
	typedef typename boost::graph_traits<GraphContainer>::adjacency_iterator adjacency_iterator;
	typedef typename boost::graph_traits<GraphContainer>::out_edge_iterator out_edge_iterator;
	typedef typename boost::graph_traits<GraphContainer>::in_edge_iterator in_edge_iterator;

	typedef typename boost::graph_traits<GraphContainer>::degree_size_type degree_t;

	typedef std::pair<adjacency_iterator, adjacency_iterator> adjacency_vertex_range_t;
	typedef std::pair<out_edge_iterator, out_edge_iterator> out_edge_range_t;
	typedef std::pair<vertex_iterator, vertex_iterator> vertex_range_t;
	typedef std::pair<edge_iterator, edge_iterator> edge_range_t;

	// constructors etc.
	InternalGraph() : _count( 0 )
	{
	}

	InternalGraph( const This& g )
	{
		*this = g; // using operator=
	}

	This& operator=( const This& g )
	{
		if( this == &g )
			return *this;

		boost::copy_graph( g._graph, _graph );
		_count = g._count;
		rebuildVertexDescriptorMap();
		return *this;
	}

	/**
	 * @warning unused and untested...
	 */
	template<typename V, typename E>
	This& operator=( const InternalGraph<V,E>& g )
	{
		boost::copy_graph( g._graph, _graph );
		_count = g._count;
		rebuildVertexDescriptorMap();
		return *this;
	}

	virtual ~InternalGraph()
	{
	}

	// structure modification methods
	void clear()
	{
		_graph.clear();
		_vertexDescriptorMap.clear();
	}

	vertex_descriptor addVertex( const Vertex& prop )
	{
		vertex_descriptor v = add_vertex( _graph );

		instance( v )                         = prop;
		get( boost::vertex_index, _graph )[v] = _count++;

		_vertexDescriptorMap[prop.getName()] = v;

		return v;
	}

	void removeVertex( const vertex_descriptor& v )
	{
		_vertexDescriptorMap.erase( instance( v ).getName() );
		clear_vertex( v, _graph );
		remove_vertex( v, _graph );
	}

	void connect( const std::string& out, const std::string& in, const std::string& inAttr )
	{
		try
		{
			vertex_descriptor& descOut = getVertexDescriptor( out );
			vertex_descriptor& descIn  = getVertexDescriptor( in );

			Edge e( out, in, inAttr );
			addEdge( descOut, descIn, e );
		}
		catch( boost::exception & e )
		{
			COUT_ERROR( boost::diagnostic_information(e) );
			//e << exception::LogicError( "Error in InternalGraph on connecting \"" + out + "\" -> \"" + in + "::" + inAttr + "\" !" );
			throw;
		}
	}

	edge_descriptor addEdge( const vertex_descriptor& v1, const vertex_descriptor& v2, const Edge& prop )
	{
		edge_descriptor addedEdge = add_edge( v1, v2, _graph ).first;

		instance( addedEdge ) = prop;

		if( has_cycle() )
		{
			BOOST_THROW_EXCEPTION( std::invalid_argument( "graph: cycle detected" ) );
		}

		return addedEdge;
	}

	vertex_descriptor& getVertexDescriptor( const std::string& vertexName )
	{
		return _vertexDescriptorMap[vertexName];
	}

	const vertex_descriptor& getVertexDescriptor( const std::string& vertexName ) const
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
	Vertex& instance( const vertex_descriptor& v )
	{
		//typename boost::property_map<GraphContainer, vertex_properties_t>::type param = get( vertex_properties, _graph );
		//return param[v];
		return get( vertex_properties, _graph )[v];
	}

	Vertex& instance( const vertex_iterator& v ) { return instance(*v); }

	const Vertex& instance( const vertex_descriptor& v ) const
	{
		//typename boost::property_map<GraphContainer, vertex_properties_t>::const_type param = get( vertex_properties, _graph );
		//return param[v];
		return get( vertex_properties, _graph )[v];
	}

	const Vertex& instance( const vertex_iterator& v ) const { return instance(*v); }

	Edge& instance( const edge_descriptor& e )
	{
		//typename boost::property_map<GraphContainer, edge_properties_t>::type param = get( edge_properties, _graph );
		//return param[e];
		return get( edge_properties, _graph )[e];
	}

	const Edge& instance( const edge_descriptor& e ) const
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

	adjacency_vertex_range_t getAdjacentVertices( const vertex_descriptor& v ) const
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

	int getVertexDegree( const vertex_descriptor& v ) const
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
	void dfs( Visitor vis, const vertex_descriptor& vroot )
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

	void bfs( const vertex_descriptor& vroot )
	{
		test_bfs_visitor vis;
		boost::breadth_first_search( _graph, vroot, visitor( vis ) );
	}

	void copyTransposed( const This& g )
	{
		// make a transposed copy of g in _graph
		boost::transpose_graph( g._graph, _graph );
		_count = g._count;
		rebuildVertexDescriptorMap();
	}

	void toDominatorTree();

	std::vector<vertex_descriptor> leaves();

	template< typename Vertex, typename Edge >
	friend std::ostream& operator<<( std::ostream& os, const This& g );
	
private:
	void rebuildVertexDescriptorMap();

protected:
	GraphContainer _graph;
	std::map<std::string, vertex_descriptor> _vertexDescriptorMap;
	int _count; // for vertex_index

};

}
}
}

#include "InternalGraph.tcc"

#endif
