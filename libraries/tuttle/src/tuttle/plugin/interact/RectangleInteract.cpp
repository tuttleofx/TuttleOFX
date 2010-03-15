#include "RectangleInteract.hpp"

#include "interact.hpp"
#include "overlay.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

RectangleInteract::RectangleInteract( const InteractInfos& infos, OFX::Double2DParam* paramA, OFX::Double2DParam* paramB, bool normalized )
: _infos(infos)
, _paramA(paramA)
, _paramB(paramB)
, _offset(0,0)
, _normalized(normalized)
{
}

RectangleInteract::~RectangleInteract( ) { }

bool RectangleInteract::draw( const OFX::DrawArgs& args )
{
	Point2 pA( _infos.pointNormalizedXXcToCanonicalXY( ofxToGil( _paramA->getValue() ) ) );
	Point2 pB( _infos.pointNormalizedXXcToCanonicalXY( ofxToGil( _paramB->getValue() ) ) );
	overlay::displayRect( pA, pB );
}

EMoveType RectangleInteract::selectIfIntesect( const Point2& mouse )
{
	Point2 center( (ofxToGil( _paramB->getValue() ) + ofxToGil( _paramA->getValue() )) * 0.5 );
	_offset = center - mouse;
	EMoveType m = clicPoint( center, mouse, _infos._marge );
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

bool RectangleInteract::selectIfIsIn( const OfxRectD& rect )
{
	Point2 pA( ofxToGil( _paramA->getValue() ) );
	Point2 pB( ofxToGil( _paramB->getValue() ) );
	if( rect.x1 <= pA.x  && rect.x2 >= pB.x &&
	    rect.y1 <= pA.y  && rect.y2 >= pB.y )
	{
		_offset = Point2(0,0);
		return true;
	}
	return false;
}

bool RectangleInteract::moveXYSelected( const Point2& point )
{
	Point2 pA( ofxToGil( _paramA->getValue() ) + _offset );
	Point2 pB( ofxToGil( _paramB->getValue() ) + _offset );
	_paramA->setValue( pA.x, pA.y );
	_paramA->setValue( pB.x, pB.y );
	return true;
}

bool RectangleInteract::moveXSelected( const Point2& point )
{
	Point2 pA( ofxToGil( _paramA->getValue() ) + _offset );
	Point2 pB( ofxToGil( _paramB->getValue() ) + _offset );
	_paramA->setValue( pA.x, _paramA->getValue().y );
	_paramB->setValue( pB.x, _paramB->getValue().y );
	return true;
}

bool RectangleInteract::moveYSelected( const Point2& point )
{
	Point2 pA( ofxToGil( _paramA->getValue() ) + _offset );
	Point2 pB( ofxToGil( _paramB->getValue() ) + _offset );
	_paramA->setValue( _paramA->getValue().x, pA.y );
	_paramB->setValue( _paramB->getValue().x, pB.y );
	return true;
}

}
}
}
