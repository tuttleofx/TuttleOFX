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

void ParamDouble2D::get( const OfxTime time, double& x, double& y ) const OFX_EXCEPTION_SPEC
{
	_controls[0]->get(time, x);
	_controls[1]->get(time, y);
}

void ParamDouble2D::set( const double &x, const double &y ) OFX_EXCEPTION_SPEC
{
	_controls[0]->set(x);
	_controls[1]->set(y);
}

void ParamDouble2D::set( const OfxTime time, const double &x, const double &y ) OFX_EXCEPTION_SPEC
{
	_controls[0]->set(time, x);
	_controls[1]->set(time, y);
}


}
}
}

