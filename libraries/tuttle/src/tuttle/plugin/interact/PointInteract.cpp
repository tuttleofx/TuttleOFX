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
	, _offset( 0, 0 )
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

EMoveType PointInteract::selectIfIntesect( const OFX::PenArgs& args )
{
	const Point2 mouse = ofxToGil( args.penPosition );
	Point2 p           = getPoint();

	_offset = p - mouse;
	double margeCanonical = getMarge() * args.pixelScale.x;
	EMoveType m           = clicPoint( p, mouse, margeCanonical );
	return m;
}

bool PointInteract::selectIfIsIn( const OfxRectD& rect )
{
	Point2 p = getPoint();

	if( p.x >= rect.x1 && p.x <= rect.x2 &&
	    p.y >= rect.y1 && p.y <= rect.y2 )
	{
		_offset = Point2( 0, 0 );
		return true;
	}
	return false;
}

bool PointInteract::moveXYSelected( const Point2& point )
{
	setPoint( point.x + _offset.x, point.y + _offset.y );
	return true;
}

bool PointInteract::moveXSelected( const Point2& point )
{
	setPoint( point.x + _offset.x, getPoint().y );
	return true;
}

bool PointInteract::moveYSelected( const Point2& point )
{
	setPoint( getPoint().x, point.y + _offset.y );
	return true;
}

}
}
}
