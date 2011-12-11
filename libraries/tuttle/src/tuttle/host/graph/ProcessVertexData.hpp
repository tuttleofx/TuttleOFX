#ifndef _TUTTLE_HOST_PROCESSVERTEXDATA_HPP_
#define _TUTTLE_HOST_PROCESSVERTEXDATA_HPP_

#include <tuttle/host/INode.hpp>

#include <tuttle/host/ofx/OfxhCore.hpp>

#include <string>

namespace tuttle {
namespace host {
namespace graph {

class ProcessVertexData
{
typedef ProcessVertexData This;

public:
	ProcessVertexData( const INode::ENodeType apiType = INode::eNodeTypeUnknown )
		: _apiType( apiType )
		, _step( 1 )
		, _interactive( 0 )
		, _outDegree( 0 )
		, _inDegree( 0 )
	{
		_timeDomain.x = kOfxFlagInfiniteMin;
		_timeDomain.y = kOfxFlagInfiniteMax;
		_renderTimeRange.x = kOfxFlagInfiniteMin;
		_renderTimeRange.y = kOfxFlagInfiniteMax;
		_renderScale.x = 1.0;
		_renderScale.y = 1.0;
	}

	ProcessVertexData( const ProcessVertexData& other ) { operator=( other ); }
	~ProcessVertexData() {}

public:
	friend std::ostream& operator<<( std::ostream& os, const This& vData );

public:
	// const GraphProcessData& _data; /// @todo tuttle: graph common datas, like renderScale
	OfxPointD _renderScale;

	INode::ENodeType _apiType;
	OfxPointD _renderTimeRange;
	OfxPointD _timeDomain;
	OfxTime _step;
	bool _interactive;

	std::size_t _outDegree; ///< number of connected input clips
	std::size_t _inDegree; ///< number of nodes using the output of this node

	typedef std::set<OfxTime> TimesSet;
	TimesSet _times;
};

}
}
}

#endif

