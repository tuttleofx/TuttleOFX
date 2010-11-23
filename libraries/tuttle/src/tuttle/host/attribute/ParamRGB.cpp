#include "ParamRGB.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamRGB::ParamRGB( INode&                           effect,
                    const std::string&                         name,
                    const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhMultiDimParam<ParamDouble, 3>( descriptor, name, effect.getParamSet() )
{
	_controls.replace<0>( new ParamDouble( effect, name + ".r", descriptor, 0 ) );
	_controls.replace<1>( new ParamDouble( effect, name + ".g", descriptor, 1 ) );
	_controls.replace<2>( new ParamDouble( effect, name + ".b", descriptor, 2 ) );
}

OfxRGBColourD ParamRGB::getDefault() const
{
	OfxRGBColourD rgb;

	rgb.r = _controls.at<0>().getDefault();
	rgb.g = _controls.at<1>().getDefault();
	rgb.b = _controls.at<2>().getDefault();
	return rgb;
}

void ParamRGB::get( double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().get( r );
	_controls.at<1>().get( g );
	_controls.at<2>().get( b );
}

void ParamRGB::getAtTime( const OfxTime time, double& r, double& g, double& b ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().getAtTime( time, r );
	_controls.at<1>().getAtTime( time, g );
	_controls.at<2>().getAtTime( time, b );
}

void ParamRGB::set( const double& r, const double& g, const double& b, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().set( r, change );
	_controls.at<1>().set( g, change );
	_controls.at<2>().set( b, change );
	this->paramChanged( change );
}

void ParamRGB::setAtTime( const OfxTime time, const double& r, const double& g, const double& b, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().setAtTime( time, r, change );
	_controls.at<1>().setAtTime( time, g, change );
	_controls.at<2>().setAtTime( time, b, change );
	this->paramChanged( change );
}

}
}
}

