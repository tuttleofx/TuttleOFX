#include "VertexProcessData.hpp"

#include <boost/foreach.hpp>

#include <iomanip>
#include <sstream>

namespace tuttle {
namespace host {
namespace graph {

std::ostream& operator<<( std::ostream& os, const VertexProcessData& vData )
{
	os << "startFrame:" << vData._startFrame << std::endl;
	os << "endFrame:" << vData._endFrame << std::endl;
	os << "step:" << vData._step << std::endl;
	os << "interactive:" << vData._interactive << std::endl;

	os << "out degree:" << vData._outDegree << std::endl;
	os << "in degree:" << vData._inDegree << std::endl;

	return os;
}

}
}
}

