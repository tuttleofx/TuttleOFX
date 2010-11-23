#include "ParamInteger3D.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamInteger3D::ParamInteger3D( INode&                           effect,
                                const std::string&                         name,
                                const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhMultiDimParam<ParamInteger, 3>( descriptor, name, effect.getParamSet() )
{
	_controls.replace<0>( new ParamInteger( effect, name + ".x", descriptor, 0 ) );
	_controls.replace<1>( new ParamInteger( effect, name + ".y", descriptor, 1 ) );
	_controls.replace<2>( new ParamInteger( effect, name + ".z", descriptor, 2 ) );
}

Ofx3DPointI ParamInteger3D::getDefault() const
{
	Ofx3DPointI point;

	point.x = _controls.at<0>().getDefault();
	point.y = _controls.at<1>().getDefault();
	point.z = _controls.at<2>().getDefault();
	return point;
}

void ParamInteger3D::get( int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().get( x );
	_controls.at<1>().get( y );
	_controls.at<2>().get( z );
}

void ParamInteger3D::getAtTime( const OfxTime time, int& x, int& y, int& z ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().getAtTime( time, x );
	_controls.at<1>().getAtTime( time, y );
	_controls.at<2>().getAtTime( time, z );
}

void ParamInteger3D::set( const int& x, const int& y, const int& z, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().set( x, change );
	_controls.at<1>().set( y, change );
	_controls.at<2>().set( z, change );
	this->paramChanged( change );
}

void ParamInteger3D::setAtTime( const OfxTime time, const int& x, const int& y, const int& z, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().setAtTime( time, x, change );
	_controls.at<1>().setAtTime( time, y, change );
	_controls.at<2>().setAtTime( time, z, change );
	this->paramChanged( change );
}

}
}
}

