#ifndef _TUTTLE_PLUGIN_INTERACT_HPP_
#define	_TUTTLE_PLUGIN_INTERACT_HPP_

#include <ofxsInteract.h>
#include <ofxsParam.h>
#include <tuttle/common/opengl/gl.h>

namespace tuttle {
namespace plugin {
namespace interact {

enum EMoveType
{
	eMoveTypeNone,
	eMoveTypeXY,
	eMoveTypeX,
	eMoveTypeY,
};

inline bool double2DSetValue( OFX::Double2DParam& p, const OfxPointD& value, const EMoveType& moveType )
{
	OfxPointD origin( p.getValue() );
	switch( moveType )
	{
		case eMoveTypeXY:
		{
			p.setValue( value );
			return true;
		}
		case eMoveTypeX:
		{
			p.setValue( value.x, origin.y );
			return true;
		}
		case eMoveTypeY:
		{
			p.setValue( origin.x, value.y );
			return true;
		}
		default:
			return false;
	}
}
inline bool double2DSetValue( OFX::Double2DParam* p, const OfxPointD& value, const EMoveType& moveType )
{
	return double2DSetValue( *p, value, moveType );
}

template<class Point>
inline EMoveType clicPoint( const Point& point, const Point& mouse, const double marge )
{
	Point dist;
	dist.x = std::abs(point.x - mouse.x);
	dist.y = std::abs(point.y - mouse.y);
	
	double marge2 = marge*2;
	if( dist.x < marge2 && dist.y < marge2 )
		return eMoveTypeXY;
	if( dist.y < marge )
		return eMoveTypeX;
	if( dist.x < marge )
		return eMoveTypeY;
	return eMoveTypeNone;
}

/**
 * @brief check if the mouse clic intersect a point
 * @return the type of intersection (None, XY, X, Y)
 * @param point the ofx parameter (in normalized space)
 * @param mouse the mouse clic
 */
inline EMoveType clicDouble2D( const OFX::Double2DParam& point, const OfxPointD& mouse, const double marge )
{
	OfxPointD p = point.getValue();
	return clicPoint<>( p, mouse, marge );
}
inline EMoveType clicDouble2D( const OFX::Double2DParam* point, const OfxPointD& mouse, const double marge )
{
	return clicDouble2D( *point, mouse, marge );
}

}
}
}

#endif

