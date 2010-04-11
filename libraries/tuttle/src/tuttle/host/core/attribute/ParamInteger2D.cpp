#include "ParamInteger2D.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamInteger2D::ParamInteger2D( ImageEffectNode&                                   effect,
                                const std::string&                                 name,
                                const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamInteger, 2>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamInteger( effect, name + ".x", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".y", descriptor ) );
	_value = getDefault();
}

OfxPointI ParamInteger2D::getDefault() const
{
	OfxPointI point;

	getProperties().getIntPropertyN( kOfxParamPropDefault, &point.x, 2 );
	return point;
}

void ParamInteger2D::get( int& x, int& y ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamInteger2D::get( const OfxTime time, int& x, int& y ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamInteger2D::set( const int &x, const int &y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

void ParamInteger2D::set( const OfxTime time, const int &x, const int &y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}



}
}
}

