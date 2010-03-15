#ifndef _TUTTLE_PLUGIN_RECTANGLEINTERACT_HPP_
#define	_TUTTLE_PLUGIN_RECTANGLEINTERACT_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "PointInteract.hpp"
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

class RectangleInteract : public PointInteract
{
public:
	typedef boost::gil::point2<double> Point2;
public:
	RectangleInteract( const InteractInfos& infos, OFX::Double2DParam*, OFX::Double2DParam*, const bool normalized = true );
	~RectangleInteract();
	
private:
	OFX::Double2DParam* _paramA; ///< same as BL
	OFX::Double2DParam* _paramB; ///< same as TR

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

	Point2 getPoint() const { return (ofxToGil( _paramB->getValue() ) + ofxToGil( _paramA->getValue() )) * 0.5; }
	void setPoint( const Scalar& x, const Scalar& y )
	{
		OfxPointD pA = _paramA->getValue();
		OfxPointD pB = _paramB->getValue();
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

}
}
}

#endif

