#ifndef _TERRY_COLOR_LAYOUT_YUV_HPP_
#define	_TERRY_COLOR_LAYOUT_YUV_HPP_

#include "rgb.hpp"

namespace terry {
namespace color {
namespace layout {

////////////////////////////////////////////////////////////////////////////////
// YUV //

/// \addtogroup ColorNameModel
/// \{
namespace yuv
{
/// \brief Luminance
struct y_t {};
/// \brief U
struct u_t {};
/// \brief V
struct v_t {};
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3< yuv::y_t
                    , yuv::u_t
                    , yuv::v_t
                    > yuv_t;

}
}
}


#endif
