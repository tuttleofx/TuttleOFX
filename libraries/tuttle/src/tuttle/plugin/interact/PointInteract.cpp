#include "PointInteract.hpp"

#include "interact.hpp"
#include "overlay.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

PointInteract::PointInteract( const InteractInfos& infos, const bool normalized )
: _infos(infos)
, _offset(0,0)
, _normalized(normalized)
{
}

PointInteract::~PointInteract( ) { }

bool PointInteract::draw( const OFX::DrawArgs& args ) const
{
	typedef boost::gil::point2<double> Point2;
	double margeCanonical = _infos.normalizedXXToCanonicalXX( _infos._marge );
	
	Point2 p( _infos.pointNormalizedXXcToCanonicalXY( getPoint() ) );

	glEnable( GL_LINE_STIPPLE );
	glColor3d(1.0,1.0,1.0);
	glLineStipple(1, 0xAAAA);
	overlay::displayPointRect( p, margeCanonical );
	glLineStipple(1, 0xFFFF);
	overlay::displayCross( p, 2.0 * margeCanonical );
	
	glDisable(GL_LINE_STIPPLE);
}

EMoveType PointInteract::selectIfIntesect( const Point2& mouse )
{
	Point2 p = getPoint();
	_offset = p - mouse;
	EMoveType m = clicPoint( p, mouse, _infos._marge );
	COUT("selectIfIntesect");
	COUT_VAR(mouse);
	COUT_VAR(p);
	switch(m)
	{
		case eMoveTypeXY:
			COUT("eMoveTypeXY");
			break;
		case eMoveTypeX:
			COUT("eMoveTypeX");
			break;
		case eMoveTypeY:
			COUT("eMoveTypeY");
			break;
		case eMoveTypeNone:
			COUT("eMoveTypeNone");
			break;
	}
	return m;
}

bool PointInteract::selectIfIsIn( const OfxRectD& rect )
{
	Point2 p = getPoint();
	if( p.x >= rect.x1 && p.x <= rect.x2 &&
	    p.y >= rect.y1 && p.y <= rect.y2 )
	{
		_offset = Point2(0,0);
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
