#include "ParamString.hpp"

namespace tuttle {
namespace host {
namespace core {


ParamString::ParamString( ImageEffectNode&                                   effect,
                          const std::string&                                 name,
                          const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamString( descriptor, name, effect ),
	_effect( effect )
{
	_value = getDefault();
}

const std::string& ParamString::getDefault() const
{
	return getProperties().getStringProperty( kOfxParamPropDefault );
}

void ParamString::get( std::string& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamString::get( const OfxTime time, std::string& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamString::set( const char* v ) OFX_EXCEPTION_SPEC
{
	_value = std::string( v );
}

void ParamString::set( const OfxTime time, const char* v ) OFX_EXCEPTION_SPEC
{
	_value = std::string( v ); ///< @todo: in time !
}


}
}
}

