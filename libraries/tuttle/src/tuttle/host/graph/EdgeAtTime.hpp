#ifndef _EDGEATTIME_HPP_
#define	_EDGEATTIME_HPP_

#include "IEdge.hpp"
#include "Edge.hpp"
#include "VertexAtTime.hpp"

namespace tuttle {
namespace host {
namespace graph {

class EdgeAtTime : public IEdge
{
public:
	EdgeAtTime();
	EdgeAtTime( const VertexAtTime::Key& out, const VertexAtTime::Key& in, const std::string& inAttrName );
	EdgeAtTime( const Edge& e );
	EdgeAtTime( const EdgeAtTime& other );
	virtual ~EdgeAtTime();

	std::ostream& exportDotDebug( std::ostream& os ) const;
	
	inline EdgeAtTime& operator=( const Edge& e )
	{
		IEdge::operator=( e );
		return *this;
	}
	
	OfxTime _inTime;
	OfxTime _outTime;
};

}
}
}

#endif

