#include "ProcessVertexAtTimeData.hpp"
#include "tuttle/host/INode.hpp"

#include <boost/foreach.hpp>

#include <iomanip>
#include <sstream>

namespace tuttle {
namespace host {
namespace graph {

std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTimeInfo& infos )
{
	os << "nodes:" << infos._nodes << std::endl;
	std::ostringstream s;
	s << "memory:" << std::setprecision( 2 ) << infos._memory * 1e-6 << "Mo (" << infos._memory << ")" << std::endl;
	os << s.str();
	return os;
}

std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTimeData& vData )
{
	os << "time:" << vData._time << std::endl;

	os << "out degree:" << vData._outDegree << std::endl;
	os << "in degree:" << vData._inDegree << std::endl;

	os << "__________" << std::endl;
	os << "localInfos:" << std::endl << vData._localInfos;
	os << "__________" << std::endl;
	os << "inputsInfos:" << std::endl << vData._inputsInfos;
	os << "__________" << std::endl;
	os << "globalInfos:" << std::endl << vData._globalInfos;
	os << "__________" << std::endl;

	// imageEffect specific options
	switch( vData._nodeData->_apiType )
	{
		case INode::eNodeTypeImageEffect:
			os << "api: Image effect" << std::endl;
			os << "field:" << vData._apiImageEffect._field << std::endl;
			os << "renderRoI:" << vData._apiImageEffect._renderRoI << std::endl;
			os << "renderScale:" << vData._nodeData->_renderScale << std::endl;

			os << "clips:" << vData._apiImageEffect._inputsRoI.size() << std::endl;
			BOOST_FOREACH( const ProcessVertexAtTimeData::ImageEffect::MapClipImageRod::value_type & item, vData._apiImageEffect._inputsRoI )
			{
				if( item.first )
				{
					os << "  clip:" << item.first->getName() << std::endl;
					os << "  roi:" << item.second << std::endl;
				}
			}
			break;
		case INode::eNodeTypeGraph:
			os << "api: Graph" << std::endl;
			break;
		case INode::eNodeTypeParam:
			os << "api: Param" << std::endl;
			break;
		case INode::eNodeTypeBuffer:
			os << "api: Input Buffer" << std::endl;
			break;
		case INode::eNodeTypeUnknown:
			os << "api: Unknown" << std::endl;
			break;
	}
	return os;
}

}
}
}

