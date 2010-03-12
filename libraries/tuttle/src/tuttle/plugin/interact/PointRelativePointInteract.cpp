#include "PointRelativePointInteract.hpp"

#include "interact.hpp"
#include "overlay.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

PointRelativePointInteract::PointRelativePointInteract( const InteractInfos& infos, OFX::Double2DParam* param, OFX::Double2DParam* relativeParam, bool normalized )
: _infos(infos)
, _param(param)
, _relativeParam(relativeParam)
, _offset(0,0)
, _normalized(normalized)
{
}

PointRelativePointInteract::~PointRelativePointInteract( ) { }

bool PointRelativePointInteract::draw( const OFX::DrawArgs& args )
{
	typedef boost::gil::point2<double> Point2;
	double margeCanonical = _infos.normalizedXXToCanonicalXX( _infos._marge );
	
	Point2 p( _infos.pointNormalizedXXcToCanonicalXY( ofxToGil( _relativeParam->getValue() ) + ofxToGil( _param->getValue() ) ) );

//	COUT( "PointRelativePointInteract::draw" );
//	COUT_VAR( _infos._imgSize );
//	COUT_VAR( p );

	glEnable( GL_LINE_STIPPLE );
	glColor3d(1.0,1.0,1.0);
	glLineStipple(1, 0xAAAA);
	overlay::displayPointRect( p, margeCanonical );
	glLineStipple(1, 0xFFFF);
	overlay::displayCross( p, 2.0 * margeCanonical );
	
	glDisable(GL_LINE_STIPPLE);
}

EMoveType PointRelativePointInteract::selectIfIntesect( const Point2& mouse )
{
	Point2 p( ofxToGil( _relativeParam->getValue() ) + ofxToGil( _param->getValue() ) );
	_offset = p - mouse;
	EMoveType m = clicPoint( p, mouse, _infos._marge );
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

bool PointRelativePointInteract::selectIfIsIn( const OfxRectD& rect )
{
	Point2 p( ofxToGil( _param->getValue() ) );
	if( p.x >= rect.x1 && p.x <= rect.x2 &&
	    p.y >= rect.y1 && p.y <= rect.y2 )
	{
		_offset = Point2(0,0);
		return true;
	}
	return false;
}

bool PointRelativePointInteract::moveXYSelected( const Point2& point )
{
	Point2 p( point - ofxToGil( _relativeParam->getValue() ) );
	_param->setValue( p.x + _offset.x, p.y + _offset.y );
	return true;
}

bool PointRelativePointInteract::moveXSelected( const Point2& point )
{
	Point2 p( point - ofxToGil( _relativeParam->getValue() ) );
	_param->setValue( p.x + _offset.x, _param->getValue().y );
	return true;
}

bool PointRelativePointInteract::moveYSelected( const Point2& point )
{
	Point2 p( point - ofxToGil( _relativeParam->getValue() ) );
	_param->setValue( _param->getValue().x, p.y + _offset.y );
	return true;
}

}
}
}
