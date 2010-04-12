#include "ParamRGBA.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamRGBA::ParamRGBA( ImageEffectNode&                                   effect,
                      const std::string&                                 name,
                      const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 4>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".r", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".g", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".b", descriptor ) );
	_controls.push_back( new ParamDouble( effect, name + ".a", descriptor ) );

	_value = getDefault();
}

OfxRGBAColourD ParamRGBA::getDefault() const
{
	OfxRGBAColourD color;

	getProperties().getDoublePropertyN( kOfxParamPropDefault, &color.r, 4 );
	return color;
}

void ParamRGBA::get( double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
	a = _value.a;
}

void ParamRGBA::get( const OfxTime time, double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC
{
	r = _value.r;
	g = _value.g;
	b = _value.b;
	a = _value.a;
}

void ParamRGBA::set( const double &r, const double &g, const double &b, const double &a ) OFX_EXCEPTION_SPEC
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
	_value.a = a;
}

void ParamRGBA::set( const OfxTime time, const double &r, const double &g, const double &b, const double &a ) OFX_EXCEPTION_SPEC
{
	_value.r = r;
	_value.g = g;
	_value.b = b;
	_value.a = a;
}


}
}
}

