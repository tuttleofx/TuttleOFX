%include <std_string.i>
%include <exception.i>

%module tuttlePython
%{
#include <tuttle/host/Exception.hpp>
#include <boost/exception/diagnostic_information.hpp>
%}

%include <tuttle/host/Exception.hpp>


%exception {
try
{
	$action
}
catch( tuttle::host::exception::LogicError& e )
{
	SWIG_exception( SWIG_RuntimeError, (std::string("LogicError: ") + e.what()).c_str() );
}
catch( boost::exception& e )
{
	SWIG_exception( SWIG_RuntimeError, ("Boost exception...\n" + boost::diagnostic_information(e)).c_str() );
}
catch( std::exception& e )
{
	SWIG_exception( SWIG_RuntimeError, (std::string("Standard c++ exception...\n") + e.what()).c_str() );
}
catch( ... )
{
	SWIG_exception( SWIG_RuntimeError, "Unknown exception..." );
}

}

