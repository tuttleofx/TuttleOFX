#include "ParamInteger.hpp"

namespace tuttle {
namespace host {
namespace core {


ParamInteger::ParamInteger( ImageEffectNode&                                   effect,
                            const std::string&                                 name,
                            const ofx::attribute::OfxhParamDescriptor& descriptor,
							const std::size_t index )
	: ofx::attribute::OfxhParamInteger( descriptor, name, effect, index ),
	_effect( effect )
{
	_value = getDefault();
}


int ParamInteger::getDefault() const
{
	return getProperties().getIntProperty( kOfxParamPropDefault );
}

void ParamInteger::get( int& v ) const OFX_EXCEPTION_SPEC
{
	v = _value;
}

void ParamInteger::get( const OfxTime time, int& v ) const OFX_EXCEPTION_SPEC
{
	v = _value; ///< @todo: in time !
}

void ParamInteger::set( const int &v ) OFX_EXCEPTION_SPEC
{
	_value = v;
}

void ParamInteger::set( const OfxTime time, const int &v ) OFX_EXCEPTION_SPEC
{
	_value = v; ///< @todo: in time !
}


}
}
}

