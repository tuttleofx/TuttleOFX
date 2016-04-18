#ifndef _TERRY_COLOR_COLORSPACE_LMS_HPP_
#define _TERRY_COLOR_COLORSPACE_LMS_HPP_

#include "xyz.hpp"

namespace terry
{
namespace color
{

/// \addtogroup ColorNameModel
/// \{
namespace lms_colorspace
{
/// \brief
struct L_t
{
};
/// \brief
struct M_t
{
};
/// \brief
struct S_t
{
};
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3<lms_colorspace::L_t, lms_colorspace::M_t, lms_colorspace::S_t> lms_colorspace_t;

/// \ingroup LayoutModel
typedef layout<lms_colorspace_t> lms_layout_t;

struct LMSParams : public IColorParams
{
    typedef LMSParams This;
    virtual bool operator==(const IColorParams& other) const
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
};

template <typename SChannelType, typename DChannelType>
void color_transformation_step(const LMSParams& params, const boost::gil::pixel<SChannelType, LMS::layout>& src,
                               boost::gil::pixel<DChannelType, XYZ::layout>& dst)
{
    std::cout << "color_transformation_step LMS to XYZ" << std::endl;
    numeric::pixel_zeros(dst);
}
template <typename SChannelType, typename DChannelType>
void color_transformation_step(const LMSParams& params, const boost::gil::pixel<SChannelType, XYZ::layout>& src,
                               boost::gil::pixel<DChannelType, LMS::layout>& dst)
{
    std::cout << "color_transformation_step XYZ to LMS" << std::endl;
    numeric::pixel_zeros(dst);
}
}
TERRY_DEFINE_GIL_INTERNALS_3(lms)
TERRY_DEFINE_COLORSPACE_STANDARD_TYPEDEFS(lms)
}

#endif
