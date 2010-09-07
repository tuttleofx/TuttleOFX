#include "Edge.hpp"
#include "GraphExporter.hpp"

namespace tuttle {
namespace host {
namespace graph {

std::ostream& Edge::exportDotDebug( std::ostream& os ) const
{
	os << "[" << std::endl;
	os << dotEntry( "type", "Edge" ) << ", " << std::endl;
	os << dotEntry( "label", getName() ) << std::endl;
	os << "]" << std::endl;
	return os;
}

std::ostream& operator<<( std::ostream& os, const Edge& v )
{
	os << v.getName() ;
	return os;
}

}
}
}
