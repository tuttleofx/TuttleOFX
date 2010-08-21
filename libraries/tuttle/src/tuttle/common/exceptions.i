%include <std_string.i>
%include <exception.i>

%{
#include <tuttle/common/exceptions.hpp>
#include <boost/exception/diagnostic_information.hpp>
%}

%include <tuttle/common/exceptions.hpp>


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

