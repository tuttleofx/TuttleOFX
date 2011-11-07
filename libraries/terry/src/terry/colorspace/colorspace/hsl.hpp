#ifndef _TERRY_COLOR_COLORSPACE_HSL_HPP_
#define	_TERRY_COLOR_COLORSPACE_HSL_HPP_

#include "rgb.hpp"

namespace terry {
namespace color {

/// \addtogroup ColorNameModel
/// \{
namespace hsl_colorspace
{
/// \brief Hue
struct hue_t {};    
/// \brief Saturation
struct saturation_t {};
/// \brief Lightness
struct lightness_t {}; 
}
/// \}

/// \ingroup ColorSpaceModel
typedef ::boost::mpl::vector3<
		hsl_colorspace::hue_t,
		hsl_colorspace::saturation_t,
		hsl_colorspace::lightness_t
	> hsl_colorspace_t;

/// \ingroup LayoutModel
typedef ::boost::gil::layout<hsl_colorspace_t> hsl_layout_t;


struct HSLParams : public IColorParams
{
	typedef HSLParams This;
	virtual bool operator==( const IColorParams& other ) const
	{
		const This* other_ptr = dynamic_cast<const This*>(&other);
		return other_ptr != NULL;
	}
};
/**
 * @brief HSL colorspace description
 * @todo
 */
struct HSL
{
	typedef RGB reference;
	typedef HSLParams params;
	
	typedef hsl_colorspace_t colorspace;
	typedef hsl_layout_t layout;
	typedef ::boost::gil::pixel<colorspace,layout> pixel;
};

template<class PixelSrc, class PixelDst>
void color_transform( const HSLParams& srcParams, const PixelSrc& src, const RGBParams& dstParams, PixelDst& dst )
{
	dst = terry::get_black<PixelDst>();
}
template<class PixelSrc, class PixelDst>
void color_transform( const RGBParams& srcParams, const PixelSrc& src, const HSLParams& dstParams, PixelDst& dst )
{
	dst = terry::get_black<PixelDst>();
}

BOOST_MPL_ASSERT( ( ::boost::mpl::equal<
					::boost::mpl::vector<XYZ, RGB, HSL>,
					color_dependencies<HSL>::from_root
					> ) );

BOOST_MPL_ASSERT( ( ::boost::mpl::equal<
					::boost::mpl::vector<HSL, RGB, XYZ>,
					color_dependencies<HSL>::to_root
					> ) );

BOOST_MPL_ASSERT(( ::boost::mpl::equal<
                                        ::boost::mpl::vector<XYZ, RGB>,
                                        ::boost::mpl::at_c<color_dependencies<HSL>::color_steps_from_root, 0>::type
                                        > ));
BOOST_MPL_ASSERT(( ::boost::mpl::equal<
                                        ::boost::mpl::vector<RGB, HSL>,
                                        ::boost::mpl::at_c<color_dependencies<HSL>::color_steps_from_root, 1>::type
                                        > ));


}

TERRY_DEFINE_GIL_INTERNALS_3(hsl)
TERRY_DEFINE_COLORSPACE_STANDARD_TYPEDEFS(hsl)

}

#endif

