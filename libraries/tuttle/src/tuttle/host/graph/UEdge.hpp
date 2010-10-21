#ifndef _TUTTLE_PROCESSEDGE_HPP_
#define _TUTTLE_PROCESSEDGE_HPP_

#include <tuttle/common/utils/global.hpp>

#include <string>

namespace tuttle {
namespace host {
namespace graph {

class UEdge
{
public:
	UEdge()
	{}

	~UEdge() {}

//	const std::string name() const { return _edge.getName(); }

	friend std::ostream& operator<<( std::ostream& os, const UEdge& v );

};

}
}
}

#endif

