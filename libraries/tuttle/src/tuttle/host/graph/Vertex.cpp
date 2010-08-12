#include "Vertex.hpp"

namespace tuttle {
namespace host {
namespace graph {

int Vertex::_count = 0;

std::ostream& operator<<( std::ostream& os, const Vertex& v )
{
	os << v.getName() ;
	return os;
}

}
}
}
