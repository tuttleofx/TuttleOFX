#include "DummyEdge.hpp"

namespace tuttle{
	namespace test{

std::ostream& operator<<(std::ostream& os, const DummyEdge& v)
{
	os << v.name() ;
	return os;
}

} // namespace test
} // namespace tuttle
