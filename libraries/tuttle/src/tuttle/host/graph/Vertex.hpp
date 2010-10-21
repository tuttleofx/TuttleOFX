#ifndef _TUTTLE_HOST_VERTEX_HPP_
#define _TUTTLE_HOST_VERTEX_HPP_

#include "IVertex.hpp"
#include "VertexProcessData.hpp"

namespace tuttle {
namespace host {
namespace graph {

class Vertex : public IVertex
{
public:
	typedef std::string Key;
public:
	Vertex( const std::string& name = "Undefined" );
	Vertex( const std::string& name, INode& processNode );
	Vertex( const Vertex& v );

	Vertex& operator=( const Vertex& v )
	{
		IVertex::operator=(v);
		_data           = v._data;
		return *this;
	}

	Key getKey() const { return getName(); }

	void setProcessData( const VertexProcessData& d ) { _data = d; }
	VertexProcessData&       getProcessData()       { return _data; }
	const VertexProcessData& getProcessData() const { return _data; }
	
	std::ostream& exportDotDebug( std::ostream& os ) const;
	friend std::ostream& operator<<( std::ostream& os, const Vertex& v );

public:
	VertexProcessData _data;
};

}
}
}

#endif

