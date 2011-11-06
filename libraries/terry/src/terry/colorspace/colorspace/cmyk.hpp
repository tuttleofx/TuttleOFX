#ifndef _TERRY_COLOR_COLORSPACE_CMYK_HPP_
#define	_TERRY_COLOR_COLORSPACE_CMYK_HPP_

#include "rgb.hpp"

namespace terry {
namespace color {

/// \addtogroup ColorNameModel
/// \{
namespace cmyk_colorspace
{
/// \brief cyan
struct cyan_t {};    
/// \brief magenta
struct magenta_t {};
/// \brief yellow
struct yellow_t {}; 
/// \brief key -> black
struct key_t {}; 
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector4<
		cmyk_colorspace::cyan_t,
		cmyk_colorspace::magenta_t,
		cmyk_colorspace::yellow_t,
		cmyk_colorspace::key_t
	> cmyk_colorspace_t;

/// \ingroup LayoutModel
typedef layout<cmyk_colorspace_t> cmyk_layout_t;

struct CMYKParams : public IColorParams
{
	typedef CMYKParams This;
	virtual bool operator==( const IColorParams& other ) const
	{
		const This* other_ptr = dynamic_cast<const This*>(&other);
		return other_ptr != NULL;
	}
};

/**
 * @brief CMYK colorspace description
 * @todo
 */
struct CMYK
{
	typedef RGB reference;
	typedef CMYKParams params;
	
	typedef cmyk_colorspace_t colorspace;
	typedef cmyk_layout_t layout;
	typedef ::boost::gil::pixel<colorspace,layout> pixel;
};

template<class PixelSrc, class PixelDst>
void color_transform( const CMYKParams& srcParams, const PixelSrc& src, const RGBParams& dstParams, PixelDst& dst )
{
	dst = terry::get_black<PixelDst>();
}
template<class PixelSrc, class PixelDst>
void color_transform( const RGBParams& srcParams, const PixelSrc& src, const CMYKParams& dstParams, PixelDst& dst )
{
	dst = terry::get_black<PixelDst>();
}


}
TERRY_DEFINE_GIL_INTERNALS_4(cmyk)
TERRY_DEFINE_COLORSPACE_STANDARD_TYPEDEFS(cmyk)
}

#endif

