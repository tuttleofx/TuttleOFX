#include "ParamDouble3D.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamDouble3D::ParamDouble3D( INode&                           effect,
                              const std::string&                         name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, name, effect.getParamSet() )
{
	_controls.replace<0>( new ParamDouble( effect, name + ".x", descriptor, 0 ) );
	_controls.replace<1>( new ParamDouble( effect, name + ".y", descriptor, 1 ) );
	_controls.replace<2>( new ParamDouble( effect, name + ".z", descriptor, 2 ) );
}

Ofx3DPointD ParamDouble3D::getDefault() const
{
	Ofx3DPointD point;

	point.x = _controls.at<0>().getDefault();
	point.y = _controls.at<1>().getDefault();
	point.z = _controls.at<2>().getDefault();
	return point;
}

void ParamDouble3D::get( double& x, double& y, double& z ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().get( x );
	_controls.at<1>().get( y );
	_controls.at<2>().get( z );
}

void ParamDouble3D::getAtTime( const OfxTime time, double& x, double& y, double& z ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().getAtTime( time, x );
	_controls.at<1>().getAtTime( time, y );
	_controls.at<2>().getAtTime( time, z );
}

void ParamDouble3D::set( const double& x, const double& y, const double& z, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().set( x, change );
	_controls.at<1>().set( y, change );
	_controls.at<2>().set( z, change );
	this->paramChanged( change );
}

void ParamDouble3D::setAtTime( const OfxTime time, const double& x, const double& y, const double& z, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().setAtTime( time, x, change );
	_controls.at<1>().setAtTime( time, y, change );
	_controls.at<2>().setAtTime( time, z, change );
	this->paramChanged( change );
}

}
}
}

