#ifndef _TUTTLE_TEST_DUMMYVERTEX_HPP_
#define _TUTTLE_TEST_DUMMYVERTEX_HPP_

#include <string>
#include <iostream>

// #include <tuttle/host/core/ProcessNode.hpp>

namespace tuttle {
namespace test {

class DummyVertex
{
public:
	typedef std::string Key;
public:
	DummyVertex() {}

	DummyVertex( const std::string& name )
		: _name( name ) {}

	DummyVertex( const DummyVertex& v )
		: _name( v.getName() ) {}

	virtual ~DummyVertex()
	{}

	Key getKey() const { return _name; }
	
	// operators
	DummyVertex& operator=( const DummyVertex& v )
	{
		if( this == &v )
			return *this;
		_name = v.getName();
		return *this;
	}

	const std::string&           getName() const { return _name; }
	friend std::ostream& operator<<( std::ostream& os, const DummyVertex& v );

private:
	std::string _name;
};

} // namespace test
} // namespace tuttle

#endif

