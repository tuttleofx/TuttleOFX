#include "GraphExporter.hpp"

namespace tuttle {
namespace host {
namespace graph {

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
