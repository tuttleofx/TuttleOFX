#include "ParamBoolean.hpp"

namespace tuttle {
namespace host {
namespace core {


ParamBoolean::ParamBoolean( ImageEffectNode&                                   effect,
                            const std::string&                                 name,
                            const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhParamBoolean( descriptor, name, effect ),
	_effect( effect )

{
	_value = getDefault();
}

bool ParamBoolean::getDefault() const
{
	return static_cast<bool>( getProperties().getIntProperty( kOfxParamPropDefault ) != 0 );
}

void ParamBoolean::get( bool& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamBoolean::get( const OfxTime time, bool& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamBoolean::set( const bool &v ) OFX_EXCEPTION_SPEC
{
	_value = v;
}

void ParamBoolean::set( const OfxTime time, const bool &v ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
}


}
}
}

