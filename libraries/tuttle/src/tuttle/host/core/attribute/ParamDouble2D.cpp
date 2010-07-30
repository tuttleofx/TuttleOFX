#include "ParamDouble2D.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamDouble2D::ParamDouble2D( ImageEffectNode& effect,
                              const std::string& name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamDouble, 2>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamDouble( effect, name + ".x", descriptor, 0 ) );
	_controls.push_back( new ParamDouble( effect, name + ".y", descriptor, 1 ) );
}

OfxPointD ParamDouble2D::getDefault() const
{
	OfxPointD point;

	point.x = _controls[0]->getDefault();
	point.y = _controls[1]->getDefault();
	return point;
}

void ParamDouble2D::get( double& x, double& y ) const OFX_EXCEPTION_SPEC
{
	_controls[0]->get(x);
	_controls[1]->get(y);
}

void ParamDouble2D::getAtTime( const OfxTime time, double& x, double& y ) const OFX_EXCEPTION_SPEC
{
	_controls[0]->getAtTime(time, x);
	_controls[1]->getAtTime(time, y);
}

void ParamDouble2D::set( const double& x, const double& y, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls[0]->set(x, ofx::attribute::eChangeNone);
	_controls[1]->set(y, ofx::attribute::eChangeNone);
	this->paramChanged( change );
}

void ParamDouble2D::setAtTime( const OfxTime time, const double& x, const double& y, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls[0]->setAtTime(time, x, ofx::attribute::eChangeNone);
	_controls[1]->setAtTime(time, y, ofx::attribute::eChangeNone);
	this->paramChanged( change );
}


}
}
}

