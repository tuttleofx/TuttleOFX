#include "ParamDouble2D.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamDouble2D::ParamDouble2D( INode&                           effect,
                              const std::string&                         name,
                              const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhMultiDimParam<ParamDouble, 2>( descriptor, name, effect.getParamSet() )
{
	_controls.replace<0>( new ParamDouble( effect, name + ".x", descriptor, 0 ) );
	_controls.replace<1>( new ParamDouble( effect, name + ".y", descriptor, 1 ) );
}

OfxPointD ParamDouble2D::getDefault() const
{
	OfxPointD point;

	point.x = _controls.at<0>().getDefault();
	point.y = _controls.at<1>().getDefault();
	return point;
}

void ParamDouble2D::get( double& x, double& y ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().get( x );
	_controls.at<1>().get( y );
}

void ParamDouble2D::getAtTime( const OfxTime time, double& x, double& y ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().getAtTime( time, x );
	_controls.at<1>().getAtTime( time, y );
}

void ParamDouble2D::set( const double& x, const double& y, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	TUTTLE_TCOUT( "ParamDouble2D::set x:" << x << " y:" << y );
	_controls.at<0>().set( x, change );
	_controls.at<1>().set( y, change );
	this->paramChanged( change );
}

void ParamDouble2D::setAtTime( const OfxTime time, const double& x, const double& y, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().setAtTime( time, x, change );
	_controls.at<1>().setAtTime( time, y, change );
	this->paramChanged( change );
}

}
}
}

