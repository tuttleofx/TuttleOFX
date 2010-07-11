#include "GraphExporter.hpp"

namespace tuttle {
namespace host {
namespace graph {

template< typename Vertex, typename Edge >
std::ostream& operator<<( std::ostream& os, const InternalGraph<Vertex, Edge>& g )
{
	os
	<< "\tvertex count: " << g.getVertexCount() << std::endl
	<< "\tedge count: " << g.getEdgeCount() << std::endl;

	exportSimple<Vertex, Edge>( g, os );
//	exportAsDot<Vertex, Edge>( g, os );
	return os;
}

}
}
}
