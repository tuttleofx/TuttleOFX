#ifndef _TUTTLE_PROCESSVERTEX_HPP_
#define _TUTTLE_PROCESSVERTEX_HPP_

#include "Vertex.hpp"

#include <tuttle/host/Node.hpp>

#include <string>
#include <iostream>

namespace tuttle {
namespace host {
namespace graph {

class ProcessVertex
{
public:
	ProcessVertex( Vertex& vertex )
		: _vertex( vertex ) {}

	~ProcessVertex() {}

	const std::string&             getName() const        { return _vertex.getName(); }
	Node*             getProcessNode()       { return _vertex.getProcessNode(); }
	const Node* const getProcessNode() const { return _vertex.getProcessNode(); }

	friend std::ostream& operator<<( std::ostream& os, const ProcessVertex& v );

private:
	Vertex& _vertex;
};

}
}
}

#endif

