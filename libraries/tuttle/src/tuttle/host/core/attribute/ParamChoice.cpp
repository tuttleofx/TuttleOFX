#include "ParamChoice.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamChoice::ParamChoice( ImageEffectNode&                                   effect,
                          const std::string&                                 name,
                          const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamChoice( descriptor, name, effect ),
	_effect( effect )

{
	_value = getDefault();
}

int ParamChoice::getDefault() const
{
	return getProperties().getIntProperty( kOfxParamPropDefault );
}

void ParamChoice::get( int& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamChoice::get( const OfxTime time, int& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamChoice::set( const int &v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = v;
	this->paramChanged( change );
}

void ParamChoice::set( const OfxTime time, const int &v, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
	this->paramChanged( change );
}


}
}
}

