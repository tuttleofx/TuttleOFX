#ifndef _TUTTLE_VERTEX_HPP_
#define _TUTTLE_VERTEX_HPP_

#include "IVertex.hpp"

namespace tuttle {
namespace host {
namespace graph {

class Vertex : public IVertex
{
public:
	Vertex( const std::string& name = "Undefined" );
	Vertex( const std::string& name, INode& processNode );
	Vertex( const Vertex& v );

	Vertex& operator=( const Vertex& v )
	{
		IVertex::operator=(v);
		_times           = v._times;
		return *this;
	}

	std::ostream& exportDotDebug( std::ostream& os ) const;
	friend std::ostream& operator<<( std::ostream& os, const Vertex& v );

public:
	typedef std::set<OfxTime> TimesSet;
	TimesSet _times;
};

}
}
}

#endif

