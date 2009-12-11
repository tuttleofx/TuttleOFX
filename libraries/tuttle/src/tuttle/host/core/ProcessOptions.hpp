#ifndef _TUTTLE_PROCESSOPTIONS_HPP_
#define _TUTTLE_PROCESSOPTIONS_HPP_

#include <ofxCore.h>
#include <string>

namespace tuttle {
namespace host {
namespace core {

class ProcessOptions{

public:
	ProcessOptions(){}

public:
	OfxTime   _startFrame;
	OfxTime   _endFrame;
	OfxTime   _step;
	bool      _interactive;
	OfxTime   _time;

	std::string  _field;
	OfxRectI     _renderRoI;
	OfxPointD    _renderScale;
};

} // namespace core
} // namespace host
} // namespace tuttle

#endif

