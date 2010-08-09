#include "GraphExporter.hpp"

namespace tuttle {
namespace host {
namespace graph {


template< typename VERTEX, typename EDGE >
void InternalGraph<VERTEX, EDGE>::toDominatorTree()
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

template< typename VERTEX, typename EDGE >
std::vector<typename InternalGraph<VERTEX, EDGE>::VertexDescriptor> InternalGraph<VERTEX, EDGE>::leaves()
{
	std::vector<VertexDescriptor> vleaves;
	vertex_range_t vrange = getVertices();
	for( vertex_iter it = vrange.first; it != vrange.second; ++it )
		if( out_degree( *it, _graph ) == 0 )
			vleaves.push_back( *it );

	return vleaves;
}

template< typename VERTEX, typename EDGE >
void InternalGraph<VERTEX, EDGE>::rebuildVertexDescriptorMap()
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



template< typename Vertex, typename Edge >
std::ostream& operator<<( std::ostream& os, const InternalGraph<Vertex, Edge>& g )
{
	os
	<< "  vertex count: " << g.getVertexCount() << std::endl
	<< "  edge count: " << g.getEdgeCount() << std::endl;

	exportSimple<Vertex, Edge>( g, os );

	return os;
}

}
}
}
