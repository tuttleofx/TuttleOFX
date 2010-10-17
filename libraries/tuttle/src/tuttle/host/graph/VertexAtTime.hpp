#ifndef _VERTEXATTIME_HPP_
#define	_VERTEXATTIME_HPP_

#include "IVertex.hpp"
#include "Vertex.hpp"

#include <ostream>

namespace tuttle {
namespace host {
namespace graph {

class VertexAtTime : public IVertex
{
public:
	struct Key : public std::pair<std::string, OfxTime>
	{
		typedef std::pair<std::string, OfxTime> Parent;
		Key( const std::string& s, const OfxTime& t ):Parent(s, t){}
	};
public:
	VertexAtTime( );
	VertexAtTime( const Vertex& v, const OfxTime t );
	VertexAtTime( const VertexAtTime& other );
	virtual ~VertexAtTime( );

	VertexAtTime& operator=( const Vertex& v )
	{
		IVertex::operator=(v);
//		_time           = v._time;
		return *this;
	}

	Key getKey() const { return Key(getName(), _time); }

	friend std::ostream& operator<<( std::ostream& os, const VertexAtTime& v );
	friend std::ostream& operator<<( std::ostream& os, const Key& v );

public:
	OfxTime _time;

};

std::ostream& operator<<( std::ostream& os, const VertexAtTime& v );
std::ostream& operator<<( std::ostream& os, const VertexAtTime::Key& v );

}
}
}

#endif

