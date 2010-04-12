#include "ParamInteger3D.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamInteger3D::ParamInteger3D( ImageEffectNode&                                   effect,
                                const std::string&                                 name,
                                const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamInteger, 3>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamInteger( effect, name + ".x", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".y", descriptor ) );
	_controls.push_back( new ParamInteger( effect, name + ".z", descriptor ) );
	_value = getDefault();
}

Ofx3DPointI ParamInteger3D::getDefault() const
{
	Ofx3DPointI point;

	getProperties().getIntPropertyN( kOfxParamPropDefault, &point.x, 3 );
	return point;
}

void ParamInteger3D::get( int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamInteger3D::get( const OfxTime time, int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamInteger3D::set( const int &x, const int &y, const int &z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
	_value.z = z;
}

void ParamInteger3D::set( const OfxTime time, const int &x, const int &y, const int &z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}


}
}
}

