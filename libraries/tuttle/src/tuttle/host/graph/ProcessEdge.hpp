#ifndef _TUTTLE_PROCESSEDGE_HPP_
#define _TUTTLE_PROCESSEDGE_HPP_

#include "Edge.hpp"

#include <tuttle/common/utils/global.hpp>

#include <string>

namespace tuttle {
namespace host {
namespace graph {

class ProcessEdge
{
public:
	ProcessEdge( const Edge& edge )
		: _edge( edge ) {}

	~ProcessEdge() {}

	const std::string name() const { return _edge.name(); }

	friend std::ostream& operator<<( std::ostream& os, const ProcessEdge& v );

private:
	const Edge& _edge;
};

}
}
}

#endif

