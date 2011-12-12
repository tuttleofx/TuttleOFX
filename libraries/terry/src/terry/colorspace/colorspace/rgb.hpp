#ifndef _TERRY_COLOR_COLORSPACE_RGB_HPP_
#define	_TERRY_COLOR_COLORSPACE_RGB_HPP_

#include "xyz.hpp"

#include <boost/gil/rgba.hpp>

namespace terry {
namespace color {

namespace rgb_colorspace
{
	using ::boost::gil::red_t;
	using ::boost::gil::green_t;
	using ::boost::gil::blue_t;
}
typedef ::boost::gil::rgb_t rgb_colorspace_t;
using ::boost::gil::rgb_layout_t;

struct RGBParams : public IColorParams
{
	typedef RGBParams This;
	virtual bool operator==( const IColorParams& other ) const
	{
		const This* other_ptr = dynamic_cast<const This*>(&other);
		return other_ptr != NULL;
	}
};

/**
 * @brief RGB colorspace description
 * @todo
 */
struct RGB
{
	typedef XYZ reference;
	typedef RGBParams params;
	
	typedef rgb_colorspace_t colorspace;
	typedef rgb_layout_t layout;
//	typedef ::boost::gil::pixel<colorspace,layout> pixel;
};

template<class PixelSrc, class PixelDst>
void color_transform( const RGBParams& srcParams, const PixelSrc& src, const XYZParams& dstParams, PixelDst& dst )
{
	dst = terry::get_black<PixelDst>();
}
template<class PixelSrc, class PixelDst>
void color_transform( const XYZParams& srcParams, const PixelSrc& src, const RGBParams& dstParams, PixelDst& dst )
{
	dst = terry::get_black<PixelDst>();
}

}
}

#endif

