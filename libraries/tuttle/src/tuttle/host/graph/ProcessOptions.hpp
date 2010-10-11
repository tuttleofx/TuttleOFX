#ifndef _TUTTLE_HOST_PROCESSOPTIONS_HPP_
#define _TUTTLE_HOST_PROCESSOPTIONS_HPP_

#include <tuttle/host/ofx/attribute/OfxhClipImage.hpp>
#include <tuttle/host/ofx/OfxhCore.hpp>

#include <string>

namespace tuttle {
namespace host {
namespace graph {

class ProcessInfos
{
typedef ProcessInfos This;

public:
	ProcessInfos()
		: _nodes( 0 )
		, _memory( 0 )
	{}

	std::size_t _nodes;
	std::size_t _memory;

	ProcessInfos& operator+=( const ProcessInfos& p )
	{
		_nodes  += p._nodes;
		_memory += p._memory;
		return *this;
	}

public:
	friend std::ostream& operator<<( std::ostream& os, const This& g );
};

class ProcessOptions
{
typedef ProcessOptions This;

public:
	ProcessOptions()
		: _startFrame( 0 )
		, _endFrame( 0 )
		, _step( 0 )
		, _interactive( 0 )
		, _time( 0 )
		, _finalNode( false )
		, _outDegree( 0 )
		, _inDegree( 0 )
	{
		_localInfos._nodes = 1; // local infos can contain only 1 node by definition...
	}

	ProcessOptions( const ProcessOptions& other ) { operator=( other ); }
	~ProcessOptions() {}

public:
	friend std::ostream& operator<<( std::ostream& os, const This& g );

public:
	OfxTime _startFrame;
	OfxTime _endFrame;
	OfxTime _step;
	bool _interactive;
	OfxTime _time;

	bool _finalNode;
	std::size_t _outDegree;
	std::size_t _inDegree; ///< number of nodes using the output of this node

	ProcessInfos _localInfos;
	ProcessInfos _inputsInfos;
	ProcessInfos _globalInfos;

	// imageEffect specific options
	std::string _field;
	OfxRectD _renderRoD; // is it a good thing to store this here ?
	OfxRectD _renderRoI;
	OfxPointD _renderScale;

	typedef std::map<tuttle::host::ofx::attribute::OfxhClipImage*, OfxRectD> MapClipImageRod;
	MapClipImageRod _inputsRoI; ///<< in which the plugin set the RoI it needs for each input clip

};

}
}
}

#endif

