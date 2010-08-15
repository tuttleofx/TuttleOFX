%include <std_string.i>
%include <exception.i>

%module tuttlePython
%{
#include <tuttle/host/exceptions.hpp>
#include <boost/exception/diagnostic_information.hpp>
%}

%include <tuttle/host/exceptions.hpp>


%exception {
try
{
	$action
}
catch( ... )
{
	SWIG_exception( SWIG_RuntimeError, boost::current_exception_diagnostic_information().c_str() );
}

}

