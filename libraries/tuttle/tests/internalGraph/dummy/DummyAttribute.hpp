#ifndef _TUTTLE_DUMMYATTRIBUTE_HPP_
#define _TUTTLE_DUMMYATTRIBUTE_HPP_

#include <string>

class DummyAttribute
{
public:
	DummyAttribute() {}

	DummyAttribute( const std::string& name )
		: _name( name ) {}

	DummyAttribute( const DummyAttribute& e )
		: _name( e.name() ) {}

	virtual ~DummyAttribute()
	{}

	const std::string& name() const                   { return _name; }
	void               setName( const std::string s ) { _name = s; }

	friend std::ostream& operator<<( std::ostream& os, const DummyAttribute& v );

private:
	std::string _name;
};

#endif

