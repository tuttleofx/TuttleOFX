#include "ProcessOptions.hpp"

#include <boost/foreach.hpp>

#include <iomanip>
#include <sstream>

namespace tuttle {
namespace host {
namespace graph {

std::ostream& operator<<( std::ostream& os, const ProcessInfos& infos )
{
	os << "nodes:" << infos._nodes << std::endl;
	std::ostringstream s;
	s << "memory:" << std::setprecision( 2 ) << infos._memory * 1e-6 << "Mo (" << infos._memory << ")" << std::endl;
	os << s.str();
	return os;
}

std::ostream& operator<<( std::ostream& os, const ProcessOptions& options )
{
	os << "startFrame:" << options._startFrame << std::endl;
	os << "endFrame:" << options._endFrame << std::endl;
	os << "step:" << options._step << std::endl;
	os << "interactive:" << options._interactive << std::endl;
	os << "time:" << options._time << std::endl;
	os << "nbInputs:" << options._nbInputs << std::endl;
	os << "nbOutputs:" << options._nbOutputs << std::endl;

	os << "localInfos:" << std::endl << options._localInfos;
	os << "globalInfos:" << std::endl << options._globalInfos;

	// imageEffect specific options
	os << "field:" << options._field << std::endl;
	os << "renderRoI:" << options._renderRoI << std::endl;
	os << "renderScale:" << options._renderScale << std::endl;

	os << "clips:" << options._inputsRoI.size() << std::endl;
	BOOST_FOREACH( const ProcessOptions::MapClipImageRod::value_type & item, options._inputsRoI )
	{
		if( item.first )
		{
			os << "  clip:" << item.first->getName() << std::endl;
			os << "  roi:" << item.second << std::endl;
		}
	}
	return os;
}

}
}
}

