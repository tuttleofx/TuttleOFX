#include "DummyVertex.hpp"

namespace tuttle{
	namespace test{

std::ostream& operator<<(std::ostream& os, const DummyVertex& v)
{
	os << v.name() ;
	return os;
}

} // namespace test
} // namespace tuttle
