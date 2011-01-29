#include "PointInteract.hpp"

#include "interact.hpp"
#include "overlay.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

PointInteract::PointInteract( const InteractInfos& infos )
	: _infos( infos )
{}

PointInteract::~PointInteract() {}

bool PointInteract::draw( const OFX::DrawArgs& args ) const
{
	double margeCanonical = getMarge() * args.pixelScale.x;

	Point2 p( getPoint() );

	glEnable( GL_LINE_STIPPLE );
	glColor3d( 1.0, 1.0, 1.0 );
	glLineStipple( 1, 0xAAAA );
	overlay::displayPointRect( p, margeCanonical );
	glLineStipple( 1, 0xFFFF );
	overlay::displayCross( p, 3.0 * margeCanonical );

	glDisable( GL_LINE_STIPPLE );
	return true;
}

EMoveType PointInteract::intersect( const OFX::PenArgs& args, Point2& offset )
{
	const Point2 mouse = ofxToGil( args.penPosition );
	const Point2 p = getPoint();

	offset = p - mouse;
	const double margeCanonical = getMarge() * args.pixelScale.x;
	const EMoveType m           = clicPoint( p, mouse, margeCanonical );
	return m;
}

bool PointInteract::isIn( const OfxRectD& rect )
{
	Point2 p = getPoint();

	if( p.x >= rect.x1 && p.x <= rect.x2 &&
	    p.y >= rect.y1 && p.y <= rect.y2 )
	{
		return true;
	}
	return false;
}

bool PointInteract::moveXYSelected( const Point2& point )
{
	setPoint( point.x, point.y );
	return true;
}

bool PointInteract::moveXSelected( const Scalar& x )
{
	setPoint( x, getPoint().y );
	return true;
}

bool PointInteract::moveYSelected( const Scalar& y )
{
	setPoint( getPoint().x, y );
	return true;
}

}
}
}
