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

void ParamString::getAtTime( const OfxTime time, std::string& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamString::set( const std::string& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = v;
	this->paramChanged( change );
}

void ParamString::setAtTime( const OfxTime time, const std::string& v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
	this->paramChanged( change );
}


}
}
}

