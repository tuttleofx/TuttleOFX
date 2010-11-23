#include "ParamInteger2D.hpp"

#include <tuttle/host/INode.hpp>

namespace tuttle {
namespace host {
namespace attribute {

ParamInteger2D::ParamInteger2D( INode&                           effect,
                                const std::string&                         name,
                                const ofx::attribute::OfxhParamDescriptor& descriptor )
	: Param( effect )
	, ofx::attribute::OfxhMultiDimParam<ParamInteger, 2>( descriptor, name, effect.getParamSet() )
{
	_controls.replace<0>( new ParamInteger( effect, name + ".x", descriptor, 0 ) );
	_controls.replace<1>( new ParamInteger( effect, name + ".y", descriptor, 1 ) );
}

OfxPointI ParamInteger2D::getDefault() const
{
	OfxPointI point;

	point.x = _controls.at<0>().getDefault();
	point.y = _controls.at<1>().getDefault();
	return point;
}

void ParamInteger2D::get( int& x, int& y ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().get( x );
	_controls.at<1>().get( y );
}

void ParamInteger2D::getAtTime( const OfxTime time, int& x, int& y ) const OFX_EXCEPTION_SPEC
{
	_controls.at<0>().getAtTime( time, x );
	_controls.at<1>().getAtTime( time, y );
}

void ParamInteger2D::set( const int& x, const int& y, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().set( x, change );
	_controls.at<1>().set( y, change );
	this->paramChanged( change );
}

void ParamInteger2D::setAtTime( const OfxTime time, const int& x, const int& y, const ofx::attribute::EChange change ) OFX_EXCEPTION_SPEC
{
	_controls.at<0>().setAtTime( time, x, change );
	_controls.at<1>().setAtTime( time, y, change );
	this->paramChanged( change );
}

}
}
}

