/**
 * This file helps managing exception for OfxPlugin.
 */

#ifndef _TUTTLE_PLUGIN_EXCEPTION_HPP
#define _TUTTLE_PLUGIN_EXCEPTION_HPP

// Include standard C++ headers.
#include <cstdarg>

// Include stl lib
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

namespace tuttle {
namespace plugin {

class BadConversion : public std::runtime_error
{
public:
	BadConversion( const std::string& s )
		: std::runtime_error( s ) {}
};

std::string stringify( double x );

class PluginException : public std::exception
{
protected:
	std::string _err;

public:
	PluginException();

	virtual ~PluginException() throw( );

	PluginException( const std::string& err );

	//PluginException( const char* format, ... );

	friend std::ostream& operator<<( std::ostream& s, const PluginException& exc )
	{
		s << exc._err;
		return s;
	}

	PluginException& operator<<( const PluginException& exc );

	PluginException& operator<<( const char* cString );

	PluginException& operator<<( const double real );

	friend PluginException operator+( const PluginException& e1, const PluginException& e2 )
	{
		return PluginException( e1._err + e2._err );
	}

	friend PluginException operator+( const PluginException& e1, const char* cString )
	{
		return PluginException( e1._err + cString );
	}

	friend PluginException operator+( const PluginException& e1, const double real )
	{
		return PluginException( e1._err + stringify( real ) );
	}

	virtual const char* what() const throw( );
};

class UnknownException : public PluginException
{
public:
	UnknownException()
	{
		_err = "Unknown exception !";
	}

};

class ImageNotReadyException : public PluginException
{
public:
	ImageNotReadyException( const std::string& s = "" )
	{
		_err  = "Image not ready !";
		_err += s;
	}

};

class InputMismatchException : public PluginException
{
public:
	InputMismatchException()
	{
		_err = "Image dimensions are not the same between input tensors and input src !";
	}

};

class InOutMismatchException : public PluginException
{
public:
	InOutMismatchException()
	{
		_err = "Image dimensions are not the same between input src and output src !";
	}

};

class BitDepthMismatchException : public PluginException
{
public:
	BitDepthMismatchException()
	{
		_err = "Bit depth mismatch !";
	}

};

class WrongRowBytesException : public PluginException
{
public:
	WrongRowBytesException()
	{
		_err = "Wrong row bytes !";
	}

};

class ExceptionAbort : public PluginException
{
public:
	ExceptionAbort()
	{
		_err = "Effect aborted !";
	}

};

class NullImageProvidedException : public PluginException
{
public:
	NullImageProvidedException()
	{
		_err = "Null image provided !";
	}

};

}
}

#endif
