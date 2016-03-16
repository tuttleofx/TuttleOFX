#ifndef _TERRY_COLOR_COLORSPACE_RGB_HPP_
#define _TERRY_COLOR_COLORSPACE_RGB_HPP_

#include "xyz.hpp"

#include <boost/gil/rgba.hpp>

namespace terry
{
namespace color
{

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
    virtual bool operator==(const IColorParams& other) const
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
};

template <typename SChannelType, typename DChannelType>
void color_transformation_step(const RGBParams& params, const boost::gil::pixel<SChannelType, RGB::layout>& src,
                               boost::gil::pixel<DChannelType, XYZ::layout>& dst)
{
    std::cout << "color_transformation_step RGB to XYZ" << std::endl;
    numeric::pixel_zeros(dst);
}
template <typename SChannelType, typename DChannelType>
void color_transformation_step(const RGBParams& params, const boost::gil::pixel<SChannelType, XYZ::layout>& src,
                               boost::gil::pixel<DChannelType, RGB::layout>& dst)
{
    std::cout << "color_transformation_step XYZ to RGB" << std::endl;
    numeric::pixel_zeros(dst);
}

// BOOST_MPL_ASSERT( ( ::boost::mpl::equal<
//					::boost::mpl::vector<XYZ, RGB>,
//					color_dependencies<RGB>::from_root
//					> ) );
//
// BOOST_MPL_ASSERT( ( ::boost::mpl::equal<
//					::boost::mpl::vector<RGB, XYZ>,
//					color_dependencies<RGB>::to_root
//					> ) );
}
}

#endif
