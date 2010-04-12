#ifndef _TUTTLE_PLUGIN_INTERACT_PARAMRECTANGLEFROMCENTERSIZE_HPP_
#define	_TUTTLE_PLUGIN_INTERACT_PARAMRECTANGLEFROMCENTERSIZE_HPP_

#include "ParamPoint.hpp"
#include "overlay.hpp"
#include <tuttle/common/image/gilGlobals.hpp>
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

template<class TFrame, ECoordonateSystem coord>
class ParamRectangleFromCenterSize : public ParamPoint<TFrame, coord>
{
public:
	ParamRectangleFromCenterSize( const InteractInfos& infos, OFX::Double2DParam* paramCenter, OFX::Double2DParam* paramSize, const TFrame& frame );
	~ParamRectangleFromCenterSize();
	
private:
	OFX::Double2DParam* _paramCenter; ///< same as TL (min)
	OFX::Double2DParam* _paramSize; ///< same as BR (max)
	TFrame _frame;

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
		if( ! _frame.isEnabled() )
			return Point2(0, 0); // throw to stop overlay ?
		OfxRectD rod = _frame.getFrame( this->getTime() );
		Point2 rodSize( rod.x2-rod.x1, rod.y2-rod.y1 );
		return pointNormalizedXXcToCanonicalXY( ofxToGil( _paramCenter->getValue() ), rodSize ) + Point2( rod.x1, rod.y1 );
	}
	void setPoint( const Scalar x, const Scalar y )
	{
		Point2 pCenter = ofxToGil( _paramCenter->getValue() );
		Point2 pSize   = ofxToGil( _paramSize->getValue() );

		OfxRectD frameRect = _frame.getFrame( this->getTime() );
		Point2 frameSize( frameRect.x2-frameRect.x1, frameRect.y2-frameRect.y1 );
		Point2 mouse( x-frameRect.x1, y-frameRect.y1 ); // relative to frame
		mouse = pointCanonicalXYToNormalizedXXc( mouse, frameSize ); // normalized in frame
		mouse = mouse + pCenter; // relative to pointCenter
		Point2 currentPos = pointCanonicalXYToNormalizedXXc( getPoint(), frameSize );
		switch( _selectType )
		{
			case eSelectTypeT:
			{
				_paramSize->setValue( pSize.x, mouse.y );
				break;
			}
			case eSelectTypeL:
			{
				_paramCenter->setValue( mouse.x, pCenter.y );
				break;
			}
			case eSelectTypeR:
			{
				_paramSize->setValue( mouse.x, pSize.y );
				break;
			}
			case eSelectTypeB:
			{
				_paramCenter->setValue( pCenter.x, mouse.y );
				break;
			}
			case eSelectTypeTL:
			{
				_paramCenter->setValue( mouse.x, pCenter.y );
				_paramSize->setValue( pSize.x, mouse.y );
				break;
			}
			case eSelectTypeTR:
			{
				_paramSize->setValue( mouse.x, mouse.y );
				break;
			}
			case eSelectTypeBL:
			{
				_paramCenter->setValue( mouse.x, mouse.y );
				break;
			}
			case eSelectTypeBR:
			{
				_paramCenter->setValue( pCenter.x, mouse.y );
				_paramSize->setValue( mouse.x, pSize.y );
				break;
			}
			case eSelectTypeC:
			{
				Point2 shift = mouse - currentPos;
				_paramCenter->setValue( pCenter.x + shift.x, pCenter.y + shift.y );
				_paramSize->setValue( pSize.x + shift.x, pSize.y + shift.y );
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
ParamRectangleFromCenterSize<TFrame, coord>::ParamRectangleFromCenterSize( const InteractInfos& infos, OFX::Double2DParam* paramCenter, OFX::Double2DParam* paramSize, const TFrame& frame )
: ParamPoint<TFrame, coord>( infos, paramCenter, frame )
, _paramCenter( paramCenter )
, _paramSize( paramSize )
, _frame( frame )
{
}

template<class TFrame, ECoordonateSystem coord>
ParamRectangleFromCenterSize<TFrame, coord>::~ParamRectangleFromCenterSize( ) { }

template<class TFrame, ECoordonateSystem coord>
bool ParamRectangleFromCenterSize<TFrame, coord>::draw( const OFX::DrawArgs& args ) const
{
	PointInteract::draw( args );
	OfxRectD rod = _frame.getFrame( this->getTime() );
	Point2 rodSize( rod.x2-rod.x1, rod.y2-rod.y1 );
	Point2 pA( pointNormalizedXXcToCanonicalXY( ofxToGil(_paramCenter->getValue()), rodSize ) + Point2( rod.x1, rod.y1 ) );
	Point2 pB( pointNormalizedXXcToCanonicalXY( ofxToGil(_paramSize->getValue()), rodSize ) + Point2( rod.x1, rod.y1 ) );
	overlay::displayRect( pA, pB );
}

template<class TFrame, ECoordonateSystem coord>
typename ParamRectangleFromCenterSize<TFrame, coord>::ESelectType ParamRectangleFromCenterSize<TFrame, coord>::selectType( const OFX::PenArgs& args ) const
{
	const Point2 p = ofxToGil( args.penPosition );
	double scale = args.pixelScale.x;
	double margeCanonical = this->getMarge() * scale;
	OfxRectD rod = _frame.getFrame( this->getTime() );
	Point2 rodSize( rod.x2-rod.x1, rod.y2-rod.y1 );
	Point2 a = pointNormalizedXXcToCanonicalXY( ofxToGil(_paramCenter->getValue()), rodSize ) + Point2( rod.x1, rod.y1 );
	Point2 b = pointNormalizedXXcToCanonicalXY( ofxToGil(_paramSize->getValue()), rodSize ) + Point2( rod.x1, rod.y1 );
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
EMoveType ParamRectangleFromCenterSize<TFrame, coord>::selectIfIntesect( const OFX::PenArgs& args )
{
	const Point2 mouse = ofxToGil( args.penPosition );
	Point2 center( (ofxToGil( _paramSize->getValue() ) + ofxToGil( _paramCenter->getValue() )) * 0.5 );
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
bool ParamRectangleFromCenterSize<TFrame, coord>::selectIfIsIn( const OfxRectD& rect )
{
	_selectType = eSelectTypeNone;
	Point2 pA( ofxToGil( _paramCenter->getValue() ) );
	Point2 pB( ofxToGil( _paramSize->getValue() ) );
	if( rect.x1 <= pA.x  && rect.x2 >= pB.x &&
	    rect.y1 <= pA.y  && rect.y2 >= pB.y )
	{
		this->_offset = Point2(0,0);
		return true;
	}
	return false;
}


}
}
}

#endif

