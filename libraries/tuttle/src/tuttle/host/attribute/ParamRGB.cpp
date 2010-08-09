#include "ParamRGB.hpp"

namespace tuttle {
namespace host {
namespace attribute {

ParamRGB::ParamRGB( ImageEffectNode& effect,
                              const std::string& name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect ),
	ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, name, effect )
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

void ParamRGB::getAtTime( const OfxTime time, double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC
{
	_controls[0]->getAtTime(time, r);
	_controls[1]->getAtTime(time, g);
	_controls[2]->getAtTime(time, b);
}

void ParamRGB::set( const double &r, const double &g, const double &b, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls[0]->set(r, ofx::attribute::eChangeNone);
	_controls[1]->set(g, ofx::attribute::eChangeNone);
	_controls[2]->set(b, ofx::attribute::eChangeNone);
	this->paramChanged( change );
}

void ParamRGB::setAtTime( const OfxTime time, const double &r, const double &g, const double &b, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls[0]->setAtTime(time, r, ofx::attribute::eChangeNone);
	_controls[1]->setAtTime(time, g, ofx::attribute::eChangeNone);
	_controls[2]->setAtTime(time, b, ofx::attribute::eChangeNone);
	this->paramChanged( change );
}

}
}
}

