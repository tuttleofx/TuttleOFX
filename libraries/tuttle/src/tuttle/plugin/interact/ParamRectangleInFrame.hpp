#ifndef _TUTTLE_PLUGIN_INTERACT_PARAMRECTANGLEINCLIP_HPP_
#define	_TUTTLE_PLUGIN_INTERACT_PARAMRECTANGLEINCLIP_HPP_

#include "Frame.hpp"
#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "PointInteract.hpp"
#include "overlay.hpp"
#include <ofxsParam.h>

#include <boost/shared_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

template<class TFrame, ECoordonateSystem coord>
class ParamRectangleInFrame : public PointInteract
{
public:
	ParamRectangleInFrame( const InteractInfos& infos, OFX::Double2DParam* paramA, OFX::Double2DParam* paramB, const TFrame& relativeClip );
	~ParamRectangleInFrame();
	
private:
	OFX::Double2DParam* _paramA; ///< same as TL (min)
	OFX::Double2DParam* _paramB; ///< same as BR (max)
	TFrame _relativeFrame;

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

	ESelectType selectType( const OFX::PenArgs& args ) const;

	EMoveType selectIfIntesect( const OFX::PenArgs& args );
	bool selectIfIsIn( const OfxRectD& );

	Point2 getPoint() const
	{
		if( ! _relativeFrame.isEnabled() )
			return Point2(0, 0); // throw to stop overlay ?
		OfxRectD rod = _relativeFrame.getFrame( this->getTime() );
		Point2 rodSize( rod.x2-rod.x1, rod.y2-rod.y1 );
		return pointNormalizedXXcToCanonicalXY( ofxToGil( ( _paramB->getValue() + _paramA->getValue() ) * 0.5 ), rodSize ) + Point2( rod.x1, rod.y1 );
	}
	void setPoint( const Scalar x, const Scalar y )
	{
		OfxRectD imgRod = _relativeFrame.getFrame( this->getTime() );
		Point2 imgRodSize( imgRod.x2-imgRod.x1, imgRod.y2-imgRod.y1 );
		Point2 mouse( x-imgRod.x1, y-imgRod.y1 );
		mouse = pointCanonicalXYToNormalizedXXc( mouse, imgRodSize );
		Point2 currentPos = pointCanonicalXYToNormalizedXXc( getPoint(), imgRodSize );
		OfxPointD pA = _paramA->getValue();
		OfxPointD pB = _paramB->getValue();
		switch( _selectType )
		{
			case eSelectTypeT:
			{
				_paramB->setValue( pB.x, mouse.y );
				break;
			}
			case eSelectTypeL:
			{
				_paramA->setValue( mouse.x, pA.y );
				break;
			}
			case eSelectTypeR:
			{
				_paramB->setValue( mouse.x, pB.y );
				break;
			}
			case eSelectTypeB:
			{
				_paramA->setValue( pA.x, mouse.y );
				break;
			}
			case eSelectTypeTL:
			{
				_paramA->setValue( mouse.x, pA.y );
				_paramB->setValue( pB.x, mouse.y );
				break;
			}
			case eSelectTypeTR:
			{
				_paramB->setValue( mouse.x, mouse.y );
				break;
			}
			case eSelectTypeBL:
			{
				_paramA->setValue( mouse.x, mouse.y );
				break;
			}
			case eSelectTypeBR:
			{
				_paramA->setValue( pA.x, mouse.y );
				_paramB->setValue( mouse.x, pB.y );
				break;
			}
			case eSelectTypeC:
			{
				Point2 shift = mouse - currentPos;
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


template<class TFrame, ECoordonateSystem coord>
ParamRectangleInFrame<TFrame, coord>::ParamRectangleInFrame( const InteractInfos& infos, OFX::Double2DParam* paramA, OFX::Double2DParam* paramB, const TFrame& relativeFrame )
: PointInteract( infos )
, _paramA( paramA )
, _paramB( paramB )
, _relativeFrame( relativeFrame )
{
}

template<class TFrame, ECoordonateSystem coord>
ParamRectangleInFrame<TFrame, coord>::~ParamRectangleInFrame( ) { }

template<class TFrame, ECoordonateSystem coord>
bool ParamRectangleInFrame<TFrame, coord>::draw( const OFX::DrawArgs& args ) const
{
	PointInteract::draw( args );
	OfxRectD rod = _relativeFrame.getFrame( this->getTime() );
	Point2 rodSize( rod.x2-rod.x1, rod.y2-rod.y1 );
	Point2 pA( pointNormalizedXXcToCanonicalXY( ofxToGil(_paramA->getValue()), rodSize ) + Point2( rod.x1, rod.y1 ) );
	Point2 pB( pointNormalizedXXcToCanonicalXY( ofxToGil(_paramB->getValue()), rodSize ) + Point2( rod.x1, rod.y1 ) );
	overlay::displayRect( pA, pB );
}

template<class TFrame, ECoordonateSystem coord>
typename ParamRectangleInFrame<TFrame, coord>::ESelectType ParamRectangleInFrame<TFrame, coord>::selectType( const OFX::PenArgs& args ) const
{
	const Point2 p = ofxToGil( args.penPosition );
	double scale = args.pixelScale.x;
	double margeCanonical = getMarge() * scale;
	OfxRectD rod = _relativeFrame.getFrame( this->getTime() );
	Point2 rodSize( rod.x2-rod.x1, rod.y2-rod.y1 );
	Point2 a = pointNormalizedXXcToCanonicalXY( ofxToGil(_paramA->getValue()), rodSize ) + Point2( rod.x1, rod.y1 );
	Point2 b = pointNormalizedXXcToCanonicalXY( ofxToGil(_paramB->getValue()), rodSize ) + Point2( rod.x1, rod.y1 );
	Point2 min, max;
	min.x = std::min( a.x, b.x );
	min.y = std::min( a.y, b.y );
	max.x = std::max( a.x, b.x );
	max.y = std::max( a.y, b.y );

	if( std::abs( a.x - p.x ) < margeCanonical )
	{
		if( std::abs( b.y - p.y ) < margeCanonical )
			return eSelectTypeTL;
		else if( std::abs( a.y - p.y ) < margeCanonical )
			return eSelectTypeBL;
		else if( p.y > min.y && p.y < max.y )
			return eSelectTypeL;
	}
	else if( std::abs( b.x - p.x ) < margeCanonical )
	{
		if( std::abs( a.y - p.y ) < margeCanonical )
			return eSelectTypeBR;
		else if( std::abs( b.y - p.y ) < margeCanonical )
			return eSelectTypeTR;
		else if( p.y > min.y && p.y < max.y )
			return eSelectTypeR;
	}
	else if( std::abs( b.y - p.y ) < margeCanonical && p.x > min.x && p.x < max.x )
		return eSelectTypeT;
	else if( std::abs( a.y - p.y ) < margeCanonical && p.x > min.x && p.x < max.x )
		return eSelectTypeB;
	return eSelectTypeNone;
}

template<class TFrame, ECoordonateSystem coord>
EMoveType ParamRectangleInFrame<TFrame, coord>::selectIfIntesect( const OFX::PenArgs& args )
{
	const Point2 mouse = ofxToGil( args.penPosition );
	Point2 center( (ofxToGil( _paramB->getValue() ) + ofxToGil( _paramA->getValue() )) * 0.5 );
	this->_offset.x = 0;
	this->_offset.y = 0;
	_selectType = selectType( args );
	if( _selectType != eSelectTypeNone )
		return eMoveTypeXY;
	EMoveType m = PointInteract::selectIfIntesect( args );
	if( m != eMoveTypeNone )
		_selectType = eSelectTypeC;
	return m;
}

template<class TFrame, ECoordonateSystem coord>
bool ParamRectangleInFrame<TFrame, coord>::selectIfIsIn( const OfxRectD& rect )
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

