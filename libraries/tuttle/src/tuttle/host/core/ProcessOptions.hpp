#ifndef _TUTTLE_HOST_PROCESSOPTIONS_HPP_
#define _TUTTLE_HOST_PROCESSOPTIONS_HPP_

#include <tuttle/host/ofx/attribute/OfxhClipImage.hpp>
#include <tuttle/host/ofx/OfxhCore.hpp>

#include <string>

namespace tuttle {
namespace host {
namespace core {

struct ProcessInfos
{
	ProcessInfos()
	: _memory(0)
	, _nodes(0)
	{}

	std::size_t _memory;
	std::size_t _nodes;

	ProcessInfos& operator+=( const ProcessInfos& p )
	{
		_memory += p._memory;
		_nodes += p._nodes;
		return *this;
	}
};


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
	bool    _interactive;
	OfxTime _time;

	ProcessInfos _localUsage;
	ProcessInfos _globalUsage;

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

