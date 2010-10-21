#include "ProcessVertexData.hpp"

#include <boost/foreach.hpp>

#include <iomanip>
#include <sstream>

namespace tuttle {
namespace host {
namespace graph {

std::ostream& operator<<( std::ostream& os, const ProcessVertexData& vData )
{
	os << "render start frame:" << vData._renderTimeRange.x << std::endl;
	os << "render end frame:" << vData._renderTimeRange.y << std::endl;
	os << "step:" << vData._step << std::endl;
	os << "interactive:" << vData._interactive << std::endl;

	os << "out degree:" << vData._outDegree << std::endl;
	os << "in degree:" << vData._inDegree << std::endl;

	return os;
}

}
}
}

