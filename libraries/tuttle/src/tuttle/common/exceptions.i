%include <std_string.i>
%include <std_except.i>
%include <exception.i>

%{
#include <tuttle/common/exceptions.hpp>
#include <boost/exception/diagnostic_information.hpp>
%}

%include <tuttle/common/exceptions.hpp>

/*
All swig exception types:
SWIG_MemoryError
SWIG_IndexError
SWIG_RuntimeError
SWIG_IOError
SWIG_TypeError
SWIG_OverflowError
SWIG_ValueError

SWIG_DivisionByZero
SWIG_SyntaxError
SWIG_SystemError
*/

%exception {
try
{
	$action
}
catch( boost::exception& e )
{
	// Add the C++ backtrace into the exception log.
	//
	// Python will display the Python backtrace and
	// the exception message, which will contain the C++ backtrace.
	e << tuttle::host::exception::backtrace( boost::trace(e) );
	
	SWIG_exception( SWIG_RuntimeError, boost::diagnostic_information_what(e) );
}
catch( std::out_of_range& e )
{
	SWIG_exception( SWIG_IndexError, e.what() );
}
catch( std::bad_alloc& e )
{
	SWIG_exception( SWIG_MemoryError, e.what() );
}
catch( std::ios_base::failure& e )
{
	SWIG_exception( SWIG_IOError, e.what() );
}
catch( std::bad_cast& e )
{
	SWIG_exception( SWIG_TypeError, e.what() );
}
catch( std::overflow_error& e )
{
	SWIG_exception( SWIG_OverflowError, e.what() );
}
catch( std::underflow_error& e )
{
	SWIG_exception( SWIG_OverflowError, e.what() );
}
catch( std::invalid_argument& e )
{
	SWIG_exception( SWIG_ValueError, e.what() );
}
/*
// C++11
catch( std::system_error& e )
{
	SWIG_exception( SWIG_SystemError, e.what() );
}
*/
catch( std::exception& e )
{
	SWIG_exception( SWIG_RuntimeError, e.what() );
}
catch( Swig::DirectorException& e )
{
	SWIG_fail;
}
catch(...)
{
	SWIG_exception( SWIG_RuntimeError, "Unknown C++ exception" );
}

}


/* If a Python error is raised by a call to a director function, the following
 * code should cause a C++ exception to be thrown.
 */
%feature("director:except") {
    if ($error != NULL) {
        throw Swig::DirectorMethodException();
    }
}

