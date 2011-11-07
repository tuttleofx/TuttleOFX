#ifndef _TUTTLE_OFXTOGIL_RECT_HPP_
#define	_TUTTLE_OFXTOGIL_RECT_HPP_

#include <ofxCore.h>

#include <tuttle/common/system/compatibility.hpp>
#include <terry/math/Rect.hpp>

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

inline terry::Rect<std::ssize_t> ofxToGil( const OfxRectI& r )
{
        return terry::Rect<std::ssize_t>( r.x1, r.y1, r.x2, r.y2 );
}

inline OfxRectI gilToOfx( const terry::Rect<std::ssize_t>& r )
{
	const OfxRectI rect = { r.x1, r.y1, r.x2, r.y2 };
	return rect;
}

}
}


#endif

