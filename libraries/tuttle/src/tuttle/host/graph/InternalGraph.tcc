#include "GraphExporter.hpp"

#include <boost/graph/graphviz.hpp>

namespace tuttle {
namespace host {
namespace graph {

template< typename VERTEX, typename EDGE, typename OutEdgeList, typename VertexList, typename EdgeList >
void InternalGraph<VERTEX, EDGE, OutEdgeList, VertexList, EdgeList>::toDominatorTree()
{
	typedef typename boost::property_map<GraphContainer, boost::vertex_index_t>::type IndexMap;
	typedef typename std::vector<vertex_descriptor >::iterator VectorDescIter;
	typedef typename boost::iterator_property_map<VectorDescIter, IndexMap > PredMap;

	std::vector<vertex_descriptor> domTreePredVector;
	IndexMap indexMap( get( boost::vertex_index, _graph ) );
	vertex_iterator uItr;
	vertex_iterator uEnd;
	int j = 0;
	for( boost::tie( uItr, uEnd ) = vertices( _graph ); uItr != uEnd; ++uItr, ++j )
	{
		put( indexMap, *uItr, j );
	}

	// Lengauer-Tarjan dominator tree algorithm
	domTreePredVector = std::vector<vertex_descriptor>( num_vertices( _graph ), boost::graph_traits<GraphContainer>::null_vertex() );
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

template< typename VERTEX, typename EDGE, typename OutEdgeList, typename VertexList, typename EdgeList >
std::vector<typename InternalGraph<VERTEX, EDGE, OutEdgeList, VertexList, EdgeList>::vertex_descriptor>
InternalGraph<VERTEX, EDGE, OutEdgeList, VertexList, EdgeList>::rootVertices()
{
	std::vector<vertex_descriptor> vroots;
	vertex_range_t vrange = getVertices();
	for( vertex_iterator it = vrange.first; it != vrange.second; ++it )
		if( out_degree( *it, _graph ) == 0 )
			vroots.push_back( *it );

	return vroots;
}

template< typename VERTEX, typename EDGE, typename OutEdgeList, typename VertexList, typename EdgeList >
std::vector<typename InternalGraph<VERTEX, EDGE, OutEdgeList, VertexList, EdgeList>::vertex_descriptor>
InternalGraph<VERTEX, EDGE, OutEdgeList, VertexList, EdgeList>::leafVertices()
{
	std::vector<vertex_descriptor> vleafs;
	vertex_range_t vrange = getVertices();
	for( vertex_iterator it = vrange.first; it != vrange.second; ++it )
		if( in_degree( *it, _graph ) == 0 )
			vleafs.push_back( *it );

	return vleafs;
}

template< typename VERTEX, typename EDGE, typename OutEdgeList, typename VertexList, typename EdgeList >
void InternalGraph<VERTEX, EDGE, OutEdgeList, VertexList, EdgeList>::rebuildVertexDescriptorMap()
{
	_vertexDescriptorMap.clear();
	BOOST_FOREACH( vertex_descriptor vd, getVertices() )
	{
		_vertexDescriptorMap[instance( vd ).getKey()] = vd;
	}
}

template< typename VERTEX, typename EDGE, typename OutEdgeList, typename VertexList, typename EdgeList >
std::size_t InternalGraph<VERTEX, EDGE, OutEdgeList, VertexList, EdgeList>::removeUnconnectedVertices( const vertex_descriptor& vroot )
{
	visitor::MarkUsed<This> vis( *this );
	this->depthFirstVisit( vis, vroot );

	std::list<std::string> toRemove;
	BOOST_FOREACH( const vertex_descriptor &vd, getVertices() )
	{
		const Vertex& v = instance( vd );

		if( !v.isUsed() )
		{
			toRemove.push_back( v.getName() );
		}
	}
	BOOST_FOREACH( const std::string & vs, toRemove )
	{
		//TUTTLE_TCOUT( "removeVertex: " << vs );
		this->removeVertex( getVertexDescriptor( vs ) );
	}

	return toRemove.size();
}

template< typename Vertex, typename Edge >
std::ostream& operator<<( std::ostream& os, const InternalGraph<Vertex, Edge>& g )
{
	os << "  vertex count: " << g.getVertexCount() << std::endl
	   << "  edge count: " << g.getEdgeCount() << std::endl;

	exportSimple<Vertex, Edge>( os, g );

	return os;
}

}
}
}
