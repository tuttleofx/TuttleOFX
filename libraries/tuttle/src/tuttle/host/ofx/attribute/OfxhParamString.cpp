#include "OfxhParamString.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {


void OfxhParamString::getV( va_list arg ) const OFX_EXCEPTION_SPEC
{
	const char** value = va_arg( arg, const char** );
	get( _returnValue ); /// @todo tuttle: "I so don't like this, temp storage should be delegated to the implementation"

	*value = _returnValue.c_str();
}

/**
 * implementation of var args function
 */
void OfxhParamString::getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
{
	const char** value = va_arg( arg, const char** );
	get( time, _returnValue ); // I so don't like this, temp storage should be delegated to the implementation

	*value = _returnValue.c_str();
}

/**
 * implementation of var args function
 */
void OfxhParamString::setV( va_list arg ) OFX_EXCEPTION_SPEC
{
	char* value = va_arg( arg, char* );
	set( value );
}

/**
 * implementation of var args function
 */
void OfxhParamString::setV( const OfxTime time, va_list arg ) OFX_EXCEPTION_SPEC
{
	char* value = va_arg( arg, char* );
	set( time, value );
}


}
}
}
}

