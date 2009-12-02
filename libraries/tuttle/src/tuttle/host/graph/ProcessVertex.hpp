#ifndef _TUTTLE_PROCESSVERTEX_HPP_
#define _TUTTLE_PROCESSVERTEX_HPP_

#include "Vertex.hpp"

#include <tuttle/host/core/ProcessNode.hpp>

#include <string>
#include <iostream>

namespace tuttle {
namespace host {
namespace graph {

class ProcessVertex
{
public:
	ProcessVertex( const Vertex& vertex )
		: _vertex( vertex ) {}

	~ProcessVertex() {}

	const std::string& name() const        { return _vertex.name(); }
	core::ProcessNode* processNode() const { return _vertex.processNode(); }

	friend std::ostream& operator<<( std::ostream& os, const ProcessVertex& v );

private:
	const Vertex& _vertex;
};

} // namespace graph
} // namespace host
} // namespace tuttle

#endif

