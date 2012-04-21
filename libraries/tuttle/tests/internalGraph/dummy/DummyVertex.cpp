#include "DummyVertex.hpp"

namespace tuttle {
namespace test {

std::ostream& operator<<( std::ostream& os, const DummyVertex& v )
{
	os << v.getName() ;
	return os;
}

} // namespace test
} // namespace tuttle
