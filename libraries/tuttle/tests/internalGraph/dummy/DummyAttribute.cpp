#include "DummyAttribute.hpp"

std::ostream& operator<<( std::ostream& os, const DummyAttribute& v )
{
	os << v.name() ;
	return os;
}

