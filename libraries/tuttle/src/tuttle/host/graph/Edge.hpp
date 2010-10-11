#ifndef _TUTTLE_EDGE_HPP_
#define _TUTTLE_EDGE_HPP_

#include "IEdge.hpp"

#include <map>
#include <set>

namespace tuttle {
namespace host {
namespace graph {

class Edge : public IEdge
{
public:
	Edge();
	Edge( const std::string& out, const std::string& in, const std::string& inAttrName );
	Edge( const Edge& e );
	~Edge();
	
public:
	typedef std::set<OfxTime> TimesSet;
	typedef std::map<OfxTime, TimesSet> TimeMap;
	TimeMap _timesNeeded;
};

}
}
}

#endif

