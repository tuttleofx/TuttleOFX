#ifndef _TERRY_COLOR_LAYOUT_YPBPR_HPP_
#define	_TERRY_COLOR_LAYOUT_YPBPR_HPP_

#include "rgb.hpp"

namespace terry {
namespace color {
namespace layout {

////////////////////////////////////////////////////////////////////////////////
// YPbPr //

/// \addtogroup ColorNameModel
/// \{
namespace YPbPr
{
/// \brief Luminance
struct Y_t {};
/// \brief Pb
struct Pb_t {};
/// \brief Pr
struct Pr_t {};
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3< YPbPr::Y_t
                    , YPbPr::Pb_t
                    , YPbPr::Pr_t
                    > YPbPr_t;

}
}
}


#endif
