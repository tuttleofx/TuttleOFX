#include "ParamRGB.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamRGB::ParamRGB( ImageEffectNode&                                   effect,
                    const std::string&                                 name,
                    const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".r", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".g", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".b", descriptor ) );
	_value = getDefault();
}

OfxRGBColourD ParamRGB::getDefault() const
{
	OfxRGBColourD color;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &color.r, 3 );
	return color;
}

void ParamRGB::get( double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
}

void ParamRGB::get( const OfxTime time, double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
}

void ParamRGB::set( const double &r, const double &g, const double &b ) OFX_EXCEPTION_SPEC
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
}

void ParamRGB::set( const OfxTime time, const double &r, const double &g, const double &b ) OFX_EXCEPTION_SPEC
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
}



}
}
}

