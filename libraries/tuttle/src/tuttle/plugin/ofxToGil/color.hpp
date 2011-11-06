#ifndef _TUTTLE_OFXTOGIL_COLOR_HPP_
#define	_TUTTLE_OFXTOGIL_COLOR_HPP_

#include <ofxPixels.h>

#include <terry/typedefs.hpp>
#include <boost/gil/rgba.hpp>

namespace tuttle {
namespace plugin {

inline boost::gil::rgba64f_pixel_t ofxToGil( const OfxRGBAColourD& c )
{
	return boost::gil::rgba64f_pixel_t( c.r, c.g, c.b, c.a );
}

}
}


#endif

