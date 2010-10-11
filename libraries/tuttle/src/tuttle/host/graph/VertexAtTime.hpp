#ifndef _VERTEXATTIME_HPP_
#define	_VERTEXATTIME_HPP_

#include "IVertex.hpp"

namespace tuttle {
namespace host {
namespace graph {

class VertexAtTime : public IVertex
{
public:
	VertexAtTime( );
	VertexAtTime( const VertexAtTime& other );
	virtual ~VertexAtTime( );

public:
	OfxTime _time;


};

}
}
}

#endif

