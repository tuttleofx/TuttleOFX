#include "ParamInteger2D.hpp"

namespace tuttle {
namespace host {
namespace core {

ParamInteger2D::ParamInteger2D( ImageEffectNode& effect,
                              const std::string& name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: ofx::attribute::OfxhMultiDimParam<ParamInteger, 2>( descriptor, name, effect ),
	_effect( effect )

{
	_controls.push_back( new ParamInteger( effect, name + ".x", descriptor, 0 ) );
	_controls.push_back( new ParamInteger( effect, name + ".y", descriptor, 1 ) );
}

OfxPointI ParamInteger2D::getDefault() const
{
	OfxPointI point;

	point.x = _controls[0]->getDefault();
	point.y = _controls[1]->getDefault();
	return point;
}

void ParamInteger2D::get( int& x, int& y ) const OFX_EXCEPTION_SPEC
{
	_controls[0]->get(x);
	_controls[1]->get(y);
}

void ParamInteger2D::get( const OfxTime time, int& x, int& y ) const OFX_EXCEPTION_SPEC
{
	_controls[0]->get(time, x);
	_controls[1]->get(time, y);
}

void ParamInteger2D::set( const int &x, const int &y, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls[0]->set(x, ofx::attribute::eChangeNone);
	_controls[1]->set(y, ofx::attribute::eChangeNone);
	this->paramChanged( change );
}

void ParamInteger2D::set( const OfxTime time, const int &x, const int &y, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls[0]->set(time, x, ofx::attribute::eChangeNone);
	_controls[1]->set(time, y, ofx::attribute::eChangeNone);
	this->paramChanged( change );
}


}
}
}

