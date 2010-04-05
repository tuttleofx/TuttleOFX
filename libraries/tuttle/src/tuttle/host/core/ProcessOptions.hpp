#ifndef _TUTTLE_HOST_PROCESSOPTIONS_HPP_
#define _TUTTLE_HOST_PROCESSOPTIONS_HPP_

#include <tuttle/host/ofx/OfxhClipImage.hpp>
#include <tuttle/host/ofx/OfxhCore.hpp>

#include <string>

namespace tuttle {
namespace host {
namespace core {

class ProcessOptions
{
public:
	ProcessOptions() {}
	ProcessOptions( const ProcessOptions& other ) { operator=( other ); }
	~ProcessOptions() {}

public:
	OfxTime _startFrame;
	OfxTime _endFrame;
	OfxTime _step;
	bool _interactive;
	OfxTime _time;

	// imageEffect specific options
	std::string _field;
	OfxRectD _renderRoI;
	OfxPointD _renderScale;

	std::map<tuttle::host::ofx::attribute::OfxhClipImage*, OfxRectD> _inputsRoI; ///<< in which the plugin set the RoI it needs for each input clip
};

}
}
}

#endif

