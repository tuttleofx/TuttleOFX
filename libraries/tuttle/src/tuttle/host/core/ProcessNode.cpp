#include "ProcessNode.hpp"

namespace tuttle{
	namespace host{
		namespace core{

std::ostream& operator<<(std::ostream& os, const ProcessNode& v)
{
	os << v.name() ;
	return os;
}

} // namespace core
} // namespace host
} // namespace tuttle