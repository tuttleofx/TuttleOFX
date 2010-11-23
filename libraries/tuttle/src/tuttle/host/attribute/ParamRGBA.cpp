#include "ParamRGBA.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamRGBA::ParamRGBA( INode&                           effect,
                      const std::string&                         name,
                      const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhMultiDimParam<ParamDouble, 4>( descriptor, name, effect.getParamSet() )
{
	_controls.replace<0>( new ParamDouble( effect, name + ".r", descriptor, 0 ) );
	_controls.replace<1>( new ParamDouble( effect, name + ".g", descriptor, 1 ) );
	_controls.replace<2>( new ParamDouble( effect, name + ".b", descriptor, 2 ) );
	_controls.replace<3>( new ParamDouble( effect, name + ".a", descriptor, 3 ) );
}

OfxRGBAColourD ParamRGBA::getDefault() const
{
	OfxRGBAColourD rgb;

	rgb.r = _controls.at<0>().getDefault();
	rgb.g = _controls.at<1>().getDefault();
	rgb.b = _controls.at<2>().getDefault();
	rgb.a = _controls.at<3>().getDefault();
	return rgb;
}

void ParamRGBA::get( double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().get( r );
	_controls.at<1>().get( g );
	_controls.at<2>().get( b );
	_controls.at<3>().get( a );
}

void ParamRGBA::getAtTime( const OfxTime time, double& r, double& g, double& b, double& a ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().getAtTime( time, r );
	_controls.at<1>().getAtTime( time, g );
	_controls.at<2>().getAtTime( time, b );
	_controls.at<3>().getAtTime( time, a );
}

void ParamRGBA::set( const double& r, const double& g, const double& b, const double& a, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().set( r, change );
	_controls.at<1>().set( g, change );
	_controls.at<2>().set( b, change );
	_controls.at<3>().set( a, change );
	this->paramChanged( change );
}

void ParamRGBA::setAtTime( const OfxTime time, const double& r, const double& g, const double& b, const double& a, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().setAtTime( time, r, change );
	_controls.at<1>().setAtTime( time, g, change );
	_controls.at<2>().setAtTime( time, b, change );
	_controls.at<3>().setAtTime( time, a, change );
	this->paramChanged( change );
}

}
}
}

