#ifndef _TUTTLE_PLUGIN_INTERACT_PARAMRECTANGLEINCLIP_HPP_
#define	_TUTTLE_PLUGIN_INTERACT_PARAMRECTANGLEINCLIP_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "PointInteract.hpp"
#include "overlay.hpp"
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

template<ECoordonateSystem coord>
class ParamRectangleInClip : public PointInteract
{
public:
	ParamRectangleInClip( const InteractInfos& infos, OFX::Double2DParam* paramA, OFX::Double2DParam* paramB, OFX::Clip* relativeClip );
	~ParamRectangleInClip();
	
private:
	OFX::Double2DParam* _paramA; ///< same as TL (min)
	OFX::Double2DParam* _paramB; ///< same as BR (max)
	OFX::Clip* _relativeClip;

	/**
	 *         T
	 *  TL___________ TR
	 *    |         |
	 * L  |    C    |  R
	 *    |         |
	 *    |_________| 
	 *  BL           BR
	 *         B
	 */
	enum ESelectType
	{
		eSelectTypeNone,
		eSelectTypeT,
		eSelectTypeL,
		eSelectTypeR,
		eSelectTypeB,
		eSelectTypeTL,
		eSelectTypeTR,
		eSelectTypeBL,
		eSelectTypeBR,
		eSelectTypeC
	};
	ESelectType _selectType;
	
public:
	bool draw( const OFX::DrawArgs& args ) const;

	ESelectType selectType( const Point2& ) const;

	EMoveType selectIfIntesect( const Point2& );
	bool selectIfIsIn( const OfxRectD& );

	Point2 getPoint() const
	{
		if( ! _relativeClip->isConnected() )
			return Point2(0, 0); // throw to stop overlay ?
		return ofxToGil( interact::pointCanonicalXYToNormalizedXXc( ( _paramB->getValue() + _paramA->getValue() ) * 0.5, _relativeClip->getCanonicalRodSize( this->getTime() ) ) );
	}
	void setPoint( const Scalar& x, const Scalar& y )
	{
		OfxPointD imgSize = _relativeClip->getCanonicalRodSize( this->getTime() );
		OfxPointD pA = interact::pointCanonicalXYToNormalizedXXc( _paramA->getValue(), imgSize );
		OfxPointD pB = interact::pointCanonicalXYToNormalizedXXc( _paramB->getValue(), imgSize );
		switch( _selectType )
		{
			case eSelectTypeT:
			{
				_paramB->setValue( pB.x, y );
				break;
			}
			case eSelectTypeL:
			{
				_paramA->setValue( x, pA.y );
				break;
			}
			case eSelectTypeR:
			{
				_paramB->setValue( x, pB.y );
				break;
			}
			case eSelectTypeB:
			{
				_paramA->setValue( pA.x, y );
				break;
			}
			case eSelectTypeTL:
			{
				_paramA->setValue( x, pA.y );
				_paramB->setValue( pB.x, y );
				break;
			}
			case eSelectTypeTR:
			{
				_paramB->setValue( x, y );
				break;
			}
			case eSelectTypeBL:
			{
				_paramA->setValue( x, y );
				break;
			}
			case eSelectTypeBR:
			{
				_paramA->setValue( pA.x, y );
				_paramB->setValue( x, pB.y );
				break;
			}
			case eSelectTypeC:
			{
				Point2 shift( getPoint() );
				shift.x = x - shift.x;
				shift.y = y - shift.y;
				_paramA->setValue( pA.x + shift.x, pA.y + shift.y );
				_paramB->setValue( pB.x + shift.x, pB.y + shift.y );
				break;
			}
			case eSelectTypeNone:
			{
				assert( false );
				break;
			}
		}
	}
};


template<ECoordonateSystem coord>
ParamRectangleInClip<coord>::ParamRectangleInClip( const InteractInfos& infos, OFX::Double2DParam* paramA, OFX::Double2DParam* paramB, OFX::Clip* relativeClip )
: PointInteract( infos )
, _paramA( paramA )
, _paramB( paramB )
{
}

template<ECoordonateSystem coord>
ParamRectangleInClip<coord>::~ParamRectangleInClip( ) { }

template<ECoordonateSystem coord>
bool ParamRectangleInClip<coord>::draw( const OFX::DrawArgs& args ) const
{
	PointInteract::draw( args );
	OfxPointD imgSize = _relativeClip->getCanonicalRodSize( this->getTime() );
	Point2 pA( ofxToGil( interact::pointNormalizedXXcToCanonicalXY( _paramA->getValue(), imgSize ) ) );
	Point2 pB( ofxToGil( interact::pointNormalizedXXcToCanonicalXY( _paramB->getValue(), imgSize ) ) );
	overlay::displayRect( pA, pB );
}

template<ECoordonateSystem coord>
typename ParamRectangleInClip<coord>::ESelectType ParamRectangleInClip<coord>::selectType( const Point2& p ) const
{
	Point2 a = ofxToGil( _paramA->getValue() );
	Point2 b = ofxToGil( _paramB->getValue() );
	Point2 min, max;
	min.x = std::min( a.x, b.x );
	min.y = std::min( a.y, b.y );
	max.x = std::max( a.x, b.x );
	max.y = std::max( a.y, b.y );

	if( std::abs( a.x - p.x ) < this->getMarge() )
	{
		if( std::abs( b.y - p.y ) < this->getMarge() )
			return eSelectTypeTL;
		else if( std::abs( a.y - p.y ) < this->getMarge() )
			return eSelectTypeBL;
		else if( p.y > min.y && p.y < max.y )
			return eSelectTypeL;
	}
	else if( std::abs( b.x - p.x ) < this->getMarge() )
	{
		if( std::abs( a.y - p.y ) < this->getMarge() )
			return eSelectTypeBR;
		else if( std::abs( b.y - p.y ) < this->getMarge() )
			return eSelectTypeTR;
		else if( p.y > min.y && p.y < max.y )
			return eSelectTypeR;
	}
	else if( std::abs( b.y - p.y ) < this->getMarge() && p.x > min.x && p.x < max.x )
		return eSelectTypeT;
	else if( std::abs( a.y - p.y ) < this->getMarge() && p.x > min.x && p.x < max.x )
		return eSelectTypeB;
	return eSelectTypeNone;
}

template<ECoordonateSystem coord>
EMoveType ParamRectangleInClip<coord>::selectIfIntesect( const Point2& mouse )
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

template<ECoordonateSystem coord>
bool ParamRectangleInClip<coord>::selectIfIsIn( const OfxRectD& rect )
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

#endif

