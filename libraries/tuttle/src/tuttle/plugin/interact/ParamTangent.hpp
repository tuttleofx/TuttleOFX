#ifndef _TUTTLE_PLUGIN_PARAMTANGENT_HPP_
#define _TUTTLE_PLUGIN_PARAMTANGENT_HPP_

#include "Frame.hpp"
#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "PointInteract.hpp"
#include "interact.hpp"
#include "ParamPoint.hpp"
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/plugin/coordinateSystem.hpp>


#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

template<class TFrame, ECoordinateSystem coord>
class ParamTangent : public ParamPoint<TFrame, coord>
{

public:
	typedef ParamPoint<TFrame, coord> PPoint;
	typedef PPoint Parent;

	ParamTangent( const InteractInfos& infos,
				 OFX::Double2DParam* pCenter,
				 OFX::Double2DParam* pTanA,
				 OFX::Double2DParam* pTanB,
				 const TFrame& frame )
	: Parent( infos, pCenter, frame )
	, _paramTanA( infos, pTanA, frame )
	, _paramTanB( infos, pTanB, frame )
	, _selectType( eSelectTypeNone )
	{
	}

	virtual ~ParamTangent( ) { }

protected:
	PPoint _paramTanA;
	PPoint _paramTanB;
	Point2 _previousTanVec;

	enum ESelectType
	{

		eSelectTypeNone,
		eSelectTypeTanA,
		eSelectTypeTanB,
		eSelectTypeCenter
	};
	ESelectType _selectType;


public:
	bool draw( const OFX::DrawArgs& args ) const;

	MotionType intersect( const OFX::PenArgs& args );

	virtual void translate( const Point2& previous, const Point2& vec )
	{
		const Point2 p = this->getPosition();
		switch( _selectType )
		{
			case eSelectTypeCenter:
			{
				const Point2 inv = previous - p;
				const Point2 realVec = inv + vec;
				Parent::setPosition( previous + vec );
				_paramTanA.setPosition( _paramTanA.getPoint() + realVec );
				_paramTanB.setPosition( _paramTanB.getPoint() + realVec );
				break;
			}
			case eSelectTypeTanA:
			{
				const Point2 newpA = previous + _previousTanVec + vec;
				_paramTanA.setPosition( newpA );
				// symetric around center
				_paramTanB.setPosition( 2*p - newpA );
				break;
			}
			case eSelectTypeTanB:
			{
				const Point2 newpB = previous + _previousTanVec + vec;
				_paramTanB.setPosition( newpB );
				// symetric around center
				_paramTanA.setPosition( 2*p - newpB );
				break;
			}
			case eSelectTypeNone:
				break;
		}
	}
};

template<class TFrame, ECoordinateSystem coord>
bool ParamTangent<TFrame, coord>::draw( const OFX::DrawArgs& args ) const
{
	Parent::draw( args );
	_paramTanA.draw( args );
	_paramTanB.draw( args );

	Point2 pA = _paramTanA.getPoint( );
	Point2 pB = _paramTanB.getPoint( );

	glBegin( GL_LINE );
	glVertex2f( pA.x, pA.y );
	glVertex2f( pB.x, pB.y );
	glEnd( );

	return true;
}

template<class TFrame, ECoordinateSystem coord>
MotionType ParamTangent<TFrame, coord>::intersect( const OFX::PenArgs& args )
{
	MotionType m;
	m._mode = eMotionTranslate;
	m._axis = eAxisXY;

	if( _paramTanA.intersect( args )._axis == eAxisXY )
	{
		_selectType = eSelectTypeTanA;
		_previousTanVec = _paramTanA.getPosition() - this->getPosition();
		return m;
	}
	if( _paramTanB.intersect( args )._axis == eAxisXY )
	{
		_selectType = eSelectTypeTanB;
		_previousTanVec = _paramTanB.getPosition() - this->getPosition();
		return m;
	}
	if( Parent::intersect( args )._axis == eAxisXY )
	{
		_selectType = eSelectTypeCenter;
		_previousTanVec = Point2(0,0);
		return m;
	}
	m._mode = eMotionNone;
	m._axis = eAxisNone;
	return m;
}

}
}
}

#endif

