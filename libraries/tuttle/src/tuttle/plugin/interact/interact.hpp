#ifndef _TUTTLE_PLUGIN_INTERACT_HPP_
#define _TUTTLE_PLUGIN_INTERACT_HPP_

#include <ofxsInteract.h>
#include <ofxsParam.h>
#include <tuttle/plugin/opengl/gl.h>
#include <cmath>

namespace tuttle {
namespace plugin {
namespace interact {

enum EMotion
{
	eMotionNone,
	eMotionTranslate,
	eMotionRotate,
	eMotionScale
};

enum EAxis
{
	eAxisNone,
	eAxisXY,
	eAxisX,
	eAxisY
};

struct MotionType
{
	EMotion _mode;
	EAxis _axis;
};

template<class Point>
inline EAxis clicPoint( const Point& point, const Point& mouse, const double marge )
{
	Point dist;
	dist.x = std::abs( point.x - mouse.x );
	dist.y = std::abs( point.y - mouse.y );

	const double bigMarge  = marge * 3.0;
	const double tinyMarge = marge * 0.5;
	if( dist.x < marge && dist.y < marge )
	{
		return eAxisXY;
	}
	else if( dist.y < tinyMarge && dist.x < bigMarge )
	{
		return eAxisX;
	}
	else if( dist.x < tinyMarge && dist.y < bigMarge  )
	{
		return eAxisY;
	}
	return eAxisNone;
}

/**
 * @brief check if the mouse clic intersect a point
 * @return the type of intersection (None, XY, X, Y)
 * @param point the ofx parameter (in normalized space)
 * @param mouse the mouse clic
 */
inline EAxis clicDouble2D( const OFX::Double2DParam& point, const OfxPointD& mouse, const double marge )
{
	OfxPointD p = point.getValue();

	return clicPoint<>( p, mouse, marge );
}

inline EAxis clicDouble2D( const OFX::Double2DParam* point, const OfxPointD& mouse, const double marge )
{
	return clicDouble2D( *point, mouse, marge );
}

}
}
}

#endif

