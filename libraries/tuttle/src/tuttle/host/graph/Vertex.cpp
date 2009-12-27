#include "Vertex.hpp"

namespace tuttle {
namespace host {
namespace graph {

std::ostream& operator<<( std::ostream& os, const Vertex& v )
{
	os << v.getName() ;
	return os;
}

}
}
}
