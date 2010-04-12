#include "ParamDouble3D.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamDouble3D::ParamDouble3D( ImageEffectNode&                                   effect,
                              const std::string&                                 name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".x", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".y", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".z", descriptor ) );
	_value = getDefault();
}

Ofx3DPointD ParamDouble3D::getDefault() const
{
	Ofx3DPointD point;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &point.x, 3 );
	return point;
}

void ParamDouble3D::get( double& x, double& y, double& z ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamDouble3D::get( const OfxTime time, double& x, double& y, double& z ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
	z = _value.z;
}

void ParamDouble3D::set( const double &x, const double &y, const double &z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
	_value.z = z;
}

void ParamDouble3D::set( const OfxTime time, const double &x, const double &y, const double &z ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}


}
}
}

