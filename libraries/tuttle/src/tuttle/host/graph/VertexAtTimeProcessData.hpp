#ifndef _TUTTLE_HOST_VERTEXATTIMEPROCESSDATA_HPP_
#define _TUTTLE_HOST_VERTEXATTIMEPROCESSDATA_HPP_

#include "VertexProcessData.hpp"

#include <tuttle/host/ofx/attribute/OfxhClipImage.hpp>
#include <tuttle/host/ofx/OfxhCore.hpp>

#include <string>

namespace tuttle {
namespace host {
namespace graph {

class VertexAtTimeProcessInfo
{
typedef VertexAtTimeProcessInfo This;

public:
	VertexAtTimeProcessInfo()
		: _nodes( 0 )
		, _memory( 0 )
	{}

	std::size_t _nodes;
	std::size_t _memory;

	VertexAtTimeProcessInfo& operator+=( const VertexAtTimeProcessInfo& p )
	{
		_nodes  += p._nodes;
		_memory += p._memory;
		return *this;
	}

public:
	friend std::ostream& operator<<( std::ostream& os, const This& g );
};

class VertexAtTimeProcessData
{
typedef VertexAtTimeProcessData This;

public:
	VertexAtTimeProcessData()
		: _nodeData( NULL )
		, _time( 0 )
		, _finalNode( false )
		, _outDegree( 0 )
		, _inDegree( 0 )
	{
		_localInfos._nodes = 1; // local infos can contain only 1 node by definition...
	}

	VertexAtTimeProcessData( const VertexProcessData& nodeData, const OfxTime time )
		: _nodeData( &nodeData )
		, _time( time )
		, _finalNode( false )
		, _outDegree( 0 )
		, _inDegree( 0 )
	{
		_localInfos._nodes = 1; // local infos can contain only 1 node by definition...
	}

	VertexAtTimeProcessData( const This& other )
		: _nodeData( other._nodeData )
	{
		operator=( other );
	}
	~VertexAtTimeProcessData() {}

	This& operator=( const This& v )
	{
		_time = v._time;
		_finalNode = v._finalNode;
		_outDegree = v._outDegree;
		_inDegree = v._inDegree;
		_localInfos = v._localInfos;
		_inputsInfos = v._inputsInfos;
		_globalInfos = v._globalInfos;

		_apiImageEffect = v._apiImageEffect;
		
		return *this;
	}

public:
	friend std::ostream& operator<<( std::ostream& os, const This& vData );

public:
	const VertexProcessData* _nodeData;

	OfxTime _time;
	bool _finalNode;

	std::size_t _outDegree; ///< number of connected input clips
	std::size_t _inDegree; ///< number of nodes using the output of this node

	VertexAtTimeProcessInfo _localInfos;
	VertexAtTimeProcessInfo _inputsInfos;
	VertexAtTimeProcessInfo _globalInfos;

	/// @group API Specific datas
	/// @{
	/**
	 * @brief imageEffect specific process datas
	 */
	struct ImageEffect
	{
		std::string _field;
		OfxRectD _renderRoD; // is it a good thing to store this here ?
		OfxRectD _renderRoI;

		typedef std::map<tuttle::host::ofx::attribute::OfxhClipImage*, OfxRectD> MapClipImageRod;
		MapClipImageRod _inputsRoI; ///<< in which the plugin set the RoI it needs for each input clip

	} _apiImageEffect;
	/// @}

};

}
}
}

#endif

