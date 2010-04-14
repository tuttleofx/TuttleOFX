#include "ParamRGB.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamRGB::ParamRGB( ImageEffectNode& effect,
                              const std::string& name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".r", descriptor, 0 ) );
	_controls.push_back( new ParamDouble( effect, name + ".g", descriptor, 1 ) );
	_controls.push_back( new ParamDouble( effect, name + ".b", descriptor, 2 ) );
}

OfxRGBColourD ParamRGB::getDefault() const
{
	OfxRGBColourD rgb;
	rgb.r = _controls[0]->getDefault();
	rgb.g = _controls[1]->getDefault();
	rgb.b = _controls[2]->getDefault();
	return rgb;
}

void ParamRGB::get( double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC
{
	_controls[0]->get(r);
	_controls[1]->get(g);
	_controls[2]->get(b);
}

void ParamRGB::get( const OfxTime time, double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC
{
	_controls[0]->get(time, r);
	_controls[1]->get(time, g);
	_controls[2]->get(time, b);
}

void ParamRGB::set( const double &r, const double &g, const double &b ) OFX_EXCEPTION_SPEC
{
	_controls[0]->set(r);
	_controls[1]->set(g);
	_controls[2]->set(b);
}

void ParamRGB::set( const OfxTime time, const double &r, const double &g, const double &b ) OFX_EXCEPTION_SPEC
{
	_controls[0]->set(time, r);
	_controls[1]->set(time, g);
	_controls[2]->set(time, b);
}

}
}
}

