#include "PluginException.hpp"
#include <cstdio>

namespace tuttle {
namespace plugin {

std::string stringify( double x )
{
	std::ostringstream o;

	if( !( o << x ) )
	{
		throw BadConversion( "file:" __FILE__ ); //" function: " __PRETTY_FUNCTION__ );
	}
	return o.str();
}

PluginException::PluginException()
{}

PluginException::~PluginException() throw( )
{
	// Nothing
}

PluginException::PluginException( const std::string& err )
{
	_err = err;
}

PluginException& PluginException::operator<<( const PluginException& exc )
{
	_err = _err + exc._err;
	return *this;
}

PluginException& PluginException::operator<<( const char* cString )
{
	_err = _err + cString;
	return *this;
}

PluginException& PluginException::operator<<( const double real )
{
	_err = _err + stringify( real );
	return *this;
}

const char* PluginException::what()  const throw ( )
{
	return _err.c_str();
}

}
}
