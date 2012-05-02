#ifndef _TUTTLE_HOST_PROCESSVERTEXATTIME_HPP_
#define	_TUTTLE_HOST_PROCESSVERTEXATTIME_HPP_

#include "IVertex.hpp"
#include "ProcessVertex.hpp"
#include "ProcessVertexData.hpp"
#include "ProcessVertexAtTimeData.hpp"

#include <ostream>
#include <string>

namespace tuttle {
namespace host {
namespace graph {

class ProcessVertexAtTime : public IVertex
{
public:
	class Key
	{
	public:
		Key()
		{}
		Key( const std::string& name, const OfxTime& time )
		: _name(name)
		, _time(time)
		{}
		
		const std::string& getName() const { return _name; }
		void setName( const std::string& name ) { _name = name; }
		
		OfxTime getTime() const { return _time; }
		void setTime( const OfxTime time ) { _time = time; }
		
		bool operator==( const Key& v ) const;
		std::size_t getHash() const;
		
	private:
		std::string _name;
		OfxTime _time;
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
	std::string _clipName;
	ProcessVertexAtTimeData _data;

};

std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTime& v );
std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTime::Key& v );

}
}
}

namespace boost {
inline std::size_t hash_value( const tuttle::host::graph::ProcessVertexAtTime::Key& k )
{ return k.getHash(); }
}

#endif

