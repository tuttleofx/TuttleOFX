#include "RectangleInteract.hpp"

#include "interact.hpp"
#include "overlay.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

RectangleInteract::RectangleInteract( const InteractInfos& infos, OFX::Double2DParam* paramA, OFX::Double2DParam* paramB, const bool normalized )
: PointInteract( infos, normalized )
, _paramA( paramA )
, _paramB( paramB )
{
}

RectangleInteract::~RectangleInteract( ) { }

bool RectangleInteract::draw( const OFX::DrawArgs& args ) const
{
	PointInteract::draw( args );
	Point2 pA( _infos.pointNormalizedXXcToCanonicalXY( ofxToGil( _paramA->getValue() ) ) );
	Point2 pB( _infos.pointNormalizedXXcToCanonicalXY( ofxToGil( _paramB->getValue() ) ) );
	overlay::displayRect( pA, pB );
}

RectangleInteract::ESelectType RectangleInteract::selectType( const Point2& p ) const
{
	Point2 a = ofxToGil( _paramA->getValue() );
	Point2 b = ofxToGil( _paramB->getValue() );
	Point2 min, max;
	min.x = std::min( a.x, b.x );
	min.y = std::min( a.y, b.y );
	max.x = std::max( a.x, b.x );
	max.y = std::max( a.y, b.y );

	if( std::abs( a.x - p.x ) < _infos._marge )
	{
		if( std::abs( b.y - p.y ) < _infos._marge )
			return eSelectTypeTL;
		else if( std::abs( a.y - p.y ) < _infos._marge )
			return eSelectTypeBL;
		else if( p.y > min.y && p.y < max.y )
			return eSelectTypeL;
	}
	else if( std::abs( b.x - p.x ) < _infos._marge )
	{
		if( std::abs( a.y - p.y ) < _infos._marge )
			return eSelectTypeBR;
		else if( std::abs( b.y - p.y ) < _infos._marge )
			return eSelectTypeTR;
		else if( p.y > min.y && p.y < max.y )
			return eSelectTypeR;
	}
	else if( std::abs( b.y - p.y ) < _infos._marge && p.x > min.x && p.x < max.x )
		return eSelectTypeT;
	else if( std::abs( a.y - p.y ) < _infos._marge && p.x > min.x && p.x < max.x )
		return eSelectTypeB;
	return eSelectTypeNone;
}

EMoveType RectangleInteract::selectIfIntesect( const Point2& mouse )
{
	Point2 center( (ofxToGil( _paramB->getValue() ) + ofxToGil( _paramA->getValue() )) * 0.5 );
	this->_offset.x = 0;
	this->_offset.y = 0;
	_selectType = selectType( mouse );
	if( _selectType != eSelectTypeNone )
		return eMoveTypeXY;
	EMoveType m = PointInteract::selectIfIntesect( mouse );
	if( m != eMoveTypeNone )
		_selectType = eSelectTypeC;
	return m;
}

bool RectangleInteract::selectIfIsIn( const OfxRectD& rect )
{
	_selectType = eSelectTypeNone;
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


}
}
}
