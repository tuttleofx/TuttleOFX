#include "ParamDouble2D.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamDouble2D::ParamDouble2D( ImageEffectNode&                                   effect,
                              const std::string&                                 name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 2>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".x", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".y", descriptor ) );
	_value = getDefault();
}

OfxPointD ParamDouble2D::getDefault() const
{
	OfxPointD point;
	getProperties().getDoublePropertyN( kOfxParamPropDefault, &point.x, 2 );
	return point;
}

void ParamDouble2D::get( double& x, double& y ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamDouble2D::get( const OfxTime time, double& x, double& y ) const OFX_EXCEPTION_SPEC
{
	x = _value.x;
	y = _value.y;
}

void ParamDouble2D::set( const double &x, const double &y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}

void ParamDouble2D::set( const OfxTime time, const double &x, const double &y ) OFX_EXCEPTION_SPEC
{
	_value.x = x;
	_value.y = y;
}



}
}
}

