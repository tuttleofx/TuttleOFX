#ifndef _TUTTLE_PLUGIN_INTERACT_PARAMRECTANGLEFROMTWOCORNERS_HPP_
#define _TUTTLE_PLUGIN_INTERACT_PARAMRECTANGLEFROMTWOCORNERS_HPP_

#include "Frame.hpp"
#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "PointInteract.hpp"
#include "overlay.hpp"
#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

template<class TFrame, ECoordinateSystem coord>
class ParamRectangleFromTwoCorners : public PointInteract
{
public:
	ParamRectangleFromTwoCorners( const InteractInfos& infos, OFX::Double2DParam* paramA, OFX::Double2DParam* paramB, const TFrame& relativeClip );
	~ParamRectangleFromTwoCorners();

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

	MotionType intersect( const OFX::PenArgs& args );
	bool      isIn( const OfxRectD& );

	Point2 getPoint() const
	{
		if( !_relativeFrame.isEnabled() )
			return Point2( 0, 0 ); // throw to stop overlay ?
		OfxRectD rod = _relativeFrame.getFrame( this->getTime() );
		Point2 rodSize( rod.x2 - rod.x1, rod.y2 - rod.y1 );
		return pointNormalizedXXcToCanonicalXY( ofxToGil( ( _paramB->getValue() + _paramA->getValue() ) * 0.5 ), rodSize ) + Point2( rod.x1, rod.y1 );
	}

	void setPoint( const Scalar x, const Scalar y )
	{
		OfxRectD imgRod = _relativeFrame.getFrame( this->getTime() );
		Point2 imgRodSize( imgRod.x2 - imgRod.x1, imgRod.y2 - imgRod.y1 );
		Point2 mouse( x - imgRod.x1, y - imgRod.y1 );

		mouse = pointCanonicalXYToNormalizedXXc( mouse, imgRodSize );
		Point2 currentPos = pointCanonicalXYToNormalizedXXc( getPoint(), imgRodSize );
		OfxPointD pA      = _paramA->getValue();
		OfxPointD pB      = _paramB->getValue();
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

template<class TFrame, ECoordinateSystem coord>
ParamRectangleFromTwoCorners<TFrame, coord>::ParamRectangleFromTwoCorners( const InteractInfos& infos, OFX::Double2DParam* paramA, OFX::Double2DParam* paramB, const TFrame& relativeFrame )
	: PointInteract( infos )
	, _paramA( paramA )
	, _paramB( paramB )
	, _relativeFrame( relativeFrame )
{}

template<class TFrame, ECoordinateSystem coord>
ParamRectangleFromTwoCorners<TFrame, coord>::~ParamRectangleFromTwoCorners() {}

template<class TFrame, ECoordinateSystem coord>
bool ParamRectangleFromTwoCorners<TFrame, coord>::draw( const OFX::DrawArgs& args ) const
{
	PointInteract::draw( args );
	OfxRectD rod = _relativeFrame.getFrame( this->getTime() );
	Point2 rodSize( rod.x2 - rod.x1, rod.y2 - rod.y1 );
	Point2 pA( pointNormalizedXXcToCanonicalXY( ofxToGil( _paramA->getValue() ), rodSize ) + Point2( rod.x1, rod.y1 ) );
	Point2 pB( pointNormalizedXXcToCanonicalXY( ofxToGil( _paramB->getValue() ), rodSize ) + Point2( rod.x1, rod.y1 ) );
	overlay::displayRect( pA, pB );
	return true;
}

template<class TFrame, ECoordinateSystem coord>
typename ParamRectangleFromTwoCorners<TFrame, coord>::ESelectType ParamRectangleFromTwoCorners<TFrame, coord>::selectType( const OFX::PenArgs& args ) const
{
	const Point2 p        = ofxToGil( args.penPosition );
	double scale          = args.pixelScale.x;
	double margeCanonical = getMarge() * scale;
	OfxRectD rod          = _relativeFrame.getFrame( this->getTime() );
	Point2 rodSize( rod.x2 - rod.x1, rod.y2 - rod.y1 );
	Point2 a = pointNormalizedXXcToCanonicalXY( ofxToGil( _paramA->getValue() ), rodSize ) + Point2( rod.x1, rod.y1 );
	Point2 b = pointNormalizedXXcToCanonicalXY( ofxToGil( _paramB->getValue() ), rodSize ) + Point2( rod.x1, rod.y1 );
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

template<class TFrame, ECoordinateSystem coord>
MotionType ParamRectangleFromTwoCorners<TFrame, coord>::intersect( const OFX::PenArgs& args )
{
	MotionType m;
	m._mode = eMotionTranslate;
	_selectType     = selectType( args );
	if( _selectType != eSelectTypeNone )
	{
		m._axis = eAxisXY;
		return m;
	}
	m = PointInteract::intersect( args );
	if( m._axis != eAxisNone )
	{
		_selectType = eSelectTypeC;
	}
	return m;
}

template<class TFrame, ECoordinateSystem coord>
bool ParamRectangleFromTwoCorners<TFrame, coord>::isIn( const OfxRectD& rect )
{
	_selectType = eSelectTypeNone;
	const OfxRectD rod = _relativeFrame.getFrame( this->getTime() );
	Point2 rodSize( rod.x2 - rod.x1, rod.y2 - rod.y1 );
	Point2 a = pointNormalizedXXcToCanonicalXY( ofxToGil( _paramA->getValue() ), rodSize ) + Point2( rod.x1, rod.y1 );
	Point2 b = pointNormalizedXXcToCanonicalXY( ofxToGil( _paramB->getValue() ), rodSize ) + Point2( rod.x1, rod.y1 );
	Point2 min, max;
	min.x = std::min( a.x, b.x );
	min.y = std::min( a.y, b.y );
	max.x = std::max( a.x, b.x );
	max.y = std::max( a.y, b.y );
	if( rect.x1 <= min.x  && rect.x2 >= max.x &&
	    rect.y1 <= min.y  && rect.y2 >= max.y )
	{
		return true;
	}
	return false;
}

}
}
}

#endif

