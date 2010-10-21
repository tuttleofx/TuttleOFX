#ifndef _TUTTLE_HOST_PROCESSVERTEXATTIME_HPP_
#define	_TUTTLE_HOST_PROCESSVERTEXATTIME_HPP_

#include "IVertex.hpp"
#include "ProcessVertex.hpp"
#include "ProcessVertexData.hpp"
#include "ProcessVertexAtTimeData.hpp"

#include <ostream>

namespace tuttle {
namespace host {
namespace graph {

class ProcessVertexAtTime : public IVertex
{
public:
	struct Key : public std::pair<std::string, OfxTime>
	{
		typedef std::pair<std::string, OfxTime> Parent;
		Key( const std::string& s, const OfxTime& t ):Parent(s, t){}
	};
public:
	ProcessVertexAtTime( );
	ProcessVertexAtTime( const ProcessVertex& v, const OfxTime t );
	ProcessVertexAtTime( const ProcessVertexAtTime& other );
	virtual ~ProcessVertexAtTime( );

	ProcessVertexAtTime& operator=( const ProcessVertexAtTime& v )
	{
		IVertex::operator=(v);
		_data           = v._data;
		return *this;
	}

	Key getKey() const { return Key(getName(), _data._time); }

	const ProcessVertexData& getProcessData() const { return *_data._nodeData; }
	ProcessVertexAtTimeData&       getProcessDataAtTime()       { return _data; }
	const ProcessVertexAtTimeData& getProcessDataAtTime() const { return _data; }

	std::ostream& exportDotDebug( std::ostream& os ) const;
	friend std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTime& v );
	friend std::ostream& operator<<( std::ostream& os, const Key& v );

public:
	ProcessVertexAtTimeData _data;

};

std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTime& v );
std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTime::Key& v );

}
}
}

#endif

