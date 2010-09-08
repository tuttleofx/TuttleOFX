#include "Edge.hpp"
#include "GraphExporter.hpp"

namespace tuttle {
namespace host {
namespace graph {

std::ostream& Edge::exportDotDebug( std::ostream& os ) const
{
	std::ostringstream s;
	s << subDotEntry( "label", getName() );
	s << subDotEntry( "id", _localId );
	
	os << "[" << std::endl;
	os << dotEntry( "type", "Edge" ) << ", " << std::endl;
	os << dotEntry( "label", s.str() ) << std::endl;
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
