#include "ProcessEdge.hpp"

namespace tuttle {
namespace host {
namespace graph {

std::ostream& operator<<( std::ostream& os, const ProcessEdge& v )
{
	os << v.name() ;
	return os;
}

} // namespace graph
} // namespace host
} // namespace tuttle
