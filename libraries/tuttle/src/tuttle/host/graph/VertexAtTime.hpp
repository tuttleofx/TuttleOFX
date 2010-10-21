#ifndef _TUTTLE_HOST_VERTEXATTIME_HPP_
#define	_TUTTLE_HOST_VERTEXATTIME_HPP_

#include "IVertex.hpp"
#include "Vertex.hpp"
#include "VertexProcessData.hpp"
#include "VertexAtTimeProcessData.hpp"

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

	Key getKey() const { return Key(getName(), _data._time); }

	const VertexProcessData& getProcessData() const { return *_data._nodeData; }
	VertexAtTimeProcessData&       getProcessDataAtTime()       { return _data; }
	const VertexAtTimeProcessData& getProcessDataAtTime() const { return _data; }

	std::ostream& exportDotDebug( std::ostream& os ) const;
	friend std::ostream& operator<<( std::ostream& os, const VertexAtTime& v );
	friend std::ostream& operator<<( std::ostream& os, const Key& v );

public:
	VertexAtTimeProcessData _data;

};

std::ostream& operator<<( std::ostream& os, const VertexAtTime& v );
std::ostream& operator<<( std::ostream& os, const VertexAtTime::Key& v );

}
}
}

#endif

