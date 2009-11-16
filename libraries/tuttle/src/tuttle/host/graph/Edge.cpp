#include "Edge.hpp"

std::ostream& operator<<(std::ostream& os, const Edge& v)
{
	os << v.name() ;
	return os;
}
