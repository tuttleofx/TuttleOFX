#include "ParamInteger3D.hpp"

namespace tuttle {
namespace host {
namespace attribute {

ParamInteger3D::ParamInteger3D( ImageEffectNode& effect,
                              const std::string& name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect ),
	ofx::attribute::OfxhMultiDimParam<ParamInteger, 3>( descriptor, name, effect )
{
	_controls.push_back( new ParamInteger( effect, name + ".x", descriptor, 0 ) );
	_controls.push_back( new ParamInteger( effect, name + ".y", descriptor, 1 ) );
	_controls.push_back( new ParamInteger( effect, name + ".z", descriptor, 2 ) );
}

Ofx3DPointI ParamInteger3D::getDefault() const
{
	Ofx3DPointI point;

	point.x = _controls[0]->getDefault();
	point.y = _controls[1]->getDefault();
	point.z = _controls[2]->getDefault();
	return point;
}

void ParamInteger3D::get( int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC
{
	_controls[0]->get(x);
	_controls[1]->get(y);
	_controls[2]->get(z);
}

void ParamInteger3D::getAtTime( const OfxTime time, int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC
{
	_controls[0]->getAtTime(time, x);
	_controls[1]->getAtTime(time, y);
	_controls[2]->getAtTime(time, z);
}

void ParamInteger3D::set( const int &x, const int &y, const int &z, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls[0]->set(x, ofx::attribute::eChangeNone);
	_controls[1]->set(y, ofx::attribute::eChangeNone);
	_controls[2]->set(z, ofx::attribute::eChangeNone);
	this->paramChanged( change );
}

void ParamInteger3D::setAtTime( const OfxTime time, const int &x, const int &y, const int &z, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls[0]->setAtTime(time, x, ofx::attribute::eChangeNone);
	_controls[1]->setAtTime(time, y, ofx::attribute::eChangeNone);
	_controls[2]->setAtTime(time, z, ofx::attribute::eChangeNone);
	this->paramChanged( change );
}

}
}
}

