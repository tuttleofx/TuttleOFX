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

PluginException::PluginException( const char* format, ... )
{
	#ifndef __GNUC__
	size_t n = _snprintf( NULL, 0, format );
	#else
	int n = snprintf( NULL, 0, format );
	#endif
	if( n > 0 )
	{
		char* message = new char[n];
		std::va_list ap;
		va_start( ap, format );
		#ifndef __GNUC__
		vsprintf_s( message, n, format, ap );
		#else
		vsprintf( message, format, ap );
		#endif
		va_end( ap );
		_err = message;
		delete message;
	}
}

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
