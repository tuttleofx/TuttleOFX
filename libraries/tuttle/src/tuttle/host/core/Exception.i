%include <std_string.i>
%include <exception.i>

%module tuttlePython
%{
#include <tuttle/host/core/Exception.hpp>
%}

%include <tuttle/host/core/Exception.hpp>


%exception {
try
{
	$action
}
catch( tuttle::host::core::exception::LogicError& e )
{
	SWIG_exception(SWIG_RuntimeError, (std::string("LogicError: ") + e.what()).c_str() );
}
//catch(std::exception& e)
//{
//	SWIG_exception(SWIG_RuntimeError,"Standard c++ exception...");
//}
catch(...)
{
	SWIG_exception(SWIG_RuntimeError,"Unknown exception...");
}

}

