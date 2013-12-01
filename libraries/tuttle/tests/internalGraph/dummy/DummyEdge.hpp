#ifndef _TUTTLE_TEST_DUMMYEDGE_HPP_
#define _TUTTLE_TEST_DUMMYEDGE_HPP_

#include <string>

namespace tuttle {
namespace test {

class DummyEdge
{
public:
	DummyEdge() {}

	DummyEdge( const std::string& name )
		: _name( name ) {}

	DummyEdge( const DummyEdge& e )
		: _name( e.getName() ) {}

	virtual ~DummyEdge()
	{}

	// operators
	DummyEdge& operator=( const DummyEdge& e )
	{
		if( this == &e )
			return *this;
		_name = e.getName();
		return *this;
	}

	const std::string& getName() const                { return _name; }
	const std::string& getInAttrName() const          { return _inAttrName; }
	void               setName( const std::string s ) { _name = s; }

	friend std::ostream& operator<<( std::ostream& os, const DummyEdge& v );

private:
	std::string _name;
	std::string _inAttrName;
};

}
}

#endif

