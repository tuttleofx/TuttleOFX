#ifndef _TUTTLE_HOST_VERTEXPROCESSDATA_HPP_
#define _TUTTLE_HOST_VERTEXPROCESSDATA_HPP_

#include <tuttle/host/INode.hpp>

#include <tuttle/host/ofx/OfxhCore.hpp>

#include <string>

namespace tuttle {
namespace host {
namespace graph {

class VertexProcessData
{
typedef VertexProcessData This;

public:
	VertexProcessData( const INode::ENodeType apiType = INode::eNodeTypeUnknown )
		: _apiType( apiType )
		, _startFrame( 0 )
		, _endFrame( 0 )
		, _step( 0 )
		, _interactive( 0 )
		, _outDegree( 0 )
		, _inDegree( 0 )
	{
		_renderScale.x = 1.0;
		_renderScale.y = 1.0;
	}

	VertexProcessData( const VertexProcessData& other ) { operator=( other ); }
	~VertexProcessData() {}

public:
	friend std::ostream& operator<<( std::ostream& os, const This& vData );

public:
	// const GraphProcessData& _data; /// @todo tuttle: graph common datas, like renderScale
	OfxPointD _renderScale;

	INode::ENodeType _apiType;
	OfxTime _startFrame;
	OfxTime _endFrame;
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

