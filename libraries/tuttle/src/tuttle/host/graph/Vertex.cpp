#include "Vertex.hpp"

namespace tuttle{
	namespace host{
		namespace graph{

std::ostream& operator<<(std::ostream& os, const Vertex& v)
{
	os << v.name() ;
	return os;
}

} // namespace graph
} // namespace host
} // namespace tuttle
