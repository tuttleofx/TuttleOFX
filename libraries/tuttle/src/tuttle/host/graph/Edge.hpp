#ifndef _TUTTLE_EDGE_HPP_
#define _TUTTLE_EDGE_HPP_

#include "IEdge.hpp"
#include "Vertex.hpp"

#include <map>
#include <set>

namespace tuttle {
namespace host {
namespace graph {

class Edge : public IEdge
{
public:
	Edge();
	Edge( const Vertex::Key& out, const Vertex::Key& in, const std::string& inAttrName );
	Edge( const Edge& e );
	~Edge();

	inline Edge& operator=( const Edge& e )
	{
		IEdge::operator=( e );
		_timesNeeded = e._timesNeeded;
		return *this;
	}

	std::ostream& exportDotDebug( std::ostream& os ) const;

public:
	typedef std::set<OfxTime> TimesSet;
	typedef std::map<OfxTime, TimesSet> TimeMap;
	TimeMap _timesNeeded;
};

}
}
}

#endif

