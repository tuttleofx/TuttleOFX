#include "VertexAtTime.hpp"

namespace tuttle {
namespace host {
namespace graph {

VertexAtTime::VertexAtTime( )
: _time(9876)
{ }

VertexAtTime::VertexAtTime( const Vertex& v, const OfxTime t )
: IVertex( v )
, _time(t)
{ }

VertexAtTime::VertexAtTime( const VertexAtTime& other )
: IVertex( other )
, _time(other._time)
{ }

VertexAtTime::~VertexAtTime( )
{ }


std::ostream& operator<<( std::ostream& os, const VertexAtTime& v )
{
	os << v.getName() << "_" << v._time;
	return os;
}

std::ostream& operator<<( std::ostream& os, const VertexAtTime::Key& v )
{
	os << v.first << "_" << v.second;
	return os;
}

}
}
}
