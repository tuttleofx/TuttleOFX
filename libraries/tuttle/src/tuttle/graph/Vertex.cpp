#include "Vertex.hpp"

std::ostream& operator<<(std::ostream& os, const Vertex& v)
{
	os << v.name() ;
	return os;
}
