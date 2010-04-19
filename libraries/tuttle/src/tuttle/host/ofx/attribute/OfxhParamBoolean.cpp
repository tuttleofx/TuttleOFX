#include "OfxhParamBoolean.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {


/**
 * implementation of var args function
 */
void OfxhParamBoolean::getV( va_list arg ) const OFX_EXCEPTION_SPEC
{
	bool v;
	get( v );

	int* value = va_arg( arg, int* );
	*value = v;
}

/**
 * implementation of var args function
 */
void OfxhParamBoolean::getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
{
	bool v;
	get( time, v );

	int* value = va_arg( arg, int* );
	*value = v;
}

/**
 * implementation of var args function
 */
void OfxhParamBoolean::setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
{
	bool value = va_arg( arg, int ) != 0;
	set( value, change );
}

/**
 * implementation of var args function
 */
void OfxhParamBoolean::setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
{
	bool value = va_arg( arg, int ) != 0;
	set( time, value, change );
}


}
}
}
}

