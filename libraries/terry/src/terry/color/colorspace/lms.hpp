#ifndef _TERRY_COLOR_COLORSPACE_LMS_HPP_
#define	_TERRY_COLOR_COLORSPACE_LMS_HPP_

#include <boost/gil/cmyk.hpp>

namespace terry {
namespace color {

/// \addtogroup ColorNameModel
/// \{
namespace lms_colorspace
{
/// \brief 
struct L_t {};    
/// \brief 
struct M_t {};
/// \brief 
struct S_t {}; 
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3<
		lms_colorspace::L_t,
		lms_colorspace::M_t,
		lms_colorspace::S_t
	> lms_colorspace_t;

/// \ingroup LayoutModel
typedef layout<lms_colorspace_t> lms_layout_t;

struct LMSParams : public IColorParams
{
	typedef LMSParams This;
	virtual bool operator==( const IColorParams& other ) const
	{
		const This* other_ptr = dynamic_cast<const This*>(&other);
		return other_ptr != NULL;
	}
};
/**
 * @brief LMS colorspace description
 * @todo
 */
struct LMS
{
	typedef XYZ reference;
	typedef LMSParams params;
	
	typedef lms_colorspace_t colorspace;
	typedef lms_layout_t layout;
	typedef ::boost::gil::pixel<colorspace,layout> pixel;
};

template<class PixelSrc, class PixelDst>
void color_transform( const LMSParams& srcParams, const PixelSrc& src, const XYZParams& dstParams, PixelDst& dst )
{
	dst = terry::get_black<PixelDst>();
}
template<class PixelSrc, class PixelDst>
void color_transform( const XYZParams& srcParams, const PixelSrc& src, const LMSParams& dstParams, PixelDst& dst )
{
	dst = terry::get_black<PixelDst>();
}



}
TERRY_DEFINE_GIL_INTERNALS_3(lms)
TERRY_DEFINE_COLORSPACE_STANDARD_TYPEDEFS(lms)
}

#endif

