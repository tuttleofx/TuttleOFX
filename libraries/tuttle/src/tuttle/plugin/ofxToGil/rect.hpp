#ifndef _TUTTLE_OFXTOGIL_RECT_HPP_
#define	_TUTTLE_OFXTOGIL_RECT_HPP_

#include <ofxCore.h>

#include <terry/math/rect.hpp>

namespace tuttle {
namespace plugin {

inline terry::Rect<double> ofxToGil( const OfxRectD& r )
{
	return terry::Rect<double>( r.x1, r.y1, r.x2, r.y2 );
}

inline OfxRectD gilToOfx( const terry::Rect<double>& r )
{
	const OfxRectD rect = { r.x1, r.y1, r.x2, r.y2 };
	return rect;
}

inline terry::Rect<int> ofxToGil( const OfxRectI& r )
{
	return terry::Rect<int>( r.x1, r.y1, r.x2, r.y2 );
}

inline OfxRectI gilToOfx( const terry::Rect<int>& r )
{
	const OfxRectI rect = { r.x1, r.y1, r.x2, r.y2 };
	return rect;
}

}
}


#endif

