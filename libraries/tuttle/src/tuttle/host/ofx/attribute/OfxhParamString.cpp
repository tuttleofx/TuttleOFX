#include "OfxhParamString.hpp"

#include <boost/functional/hash.hpp>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

void OfxhParamString::getV( va_list arg ) const OFX_EXCEPTION_SPEC
{
	const char** value = va_arg( arg, const char** );

	this->getValue( _returnValue ); /// @todo tuttle: "I so don't like this, temp storage should be delegated to the implementation"

	*value = _returnValue.c_str();
}

/**
 * implementation of var args function
 */
void OfxhParamString::getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
{
	const char** value = va_arg( arg, const char** );

	this->getValueAtTime( time, _returnValue ); // I so don't like this, temp storage should be delegated to the implementation

	*value = _returnValue.c_str();
}

/**
 * implementation of var args function
 */
void OfxhParamString::setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
{
	char* value = va_arg( arg, char* );

	this->setValue( value, change );
}

/**
 * implementation of var args function
 */
void OfxhParamString::setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
{
	char* value = va_arg( arg, char* );

	this->setValueAtTime( time, value, change );
}

std::size_t OfxhParamString::getHashAtTime( const OfxTime time ) const
{
	std::string value;
	getValueAtTime( time, value );
	return boost::hash_value( value );
}

}
}
}
}

