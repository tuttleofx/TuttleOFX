#ifndef _TERRY_COLOR_LAYOUT_RGB_HPP_
#define	_TERRY_COLOR_LAYOUT_RGB_HPP_

#include "rgb.hpp"
#include <boost/gil/rgb.hpp>
#include <boost/gil/rgba.hpp>

namespace terry {
namespace color {
namespace layout {

////////////////////////////////////////////////////////////////////////////////
// RGB //
	
/// \addtogroup ColorNameModel
/// \{
namespace rgb
{
/// \brief Red
using boost::gil::red_t;
/// \brief Green
using boost::gil::green_t;
/// \brief Blue
using boost::gil::blue_t;
}

/// \}
/// \addtogroup ColorNameModel
using boost::gil::rgb_t;

/// \ingroup LayoutModel
using boost::gil::rgb_layout_t;
using boost::gil::bgr_layout_t;


////////////////////////////////////////////////////////////////////////////////
// RGBA //
/// \addtogroup ColorNameModel
using boost::gil::rgba_t;

/// \ingroup LayoutModel
using boost::gil::rgba_layout_t;
using boost::gil::bgra_layout_t;
using boost::gil::argb_layout_t;
using boost::gil::abgr_layout_t;

/*
////////////////////////////////////////////////////////////////////////////////
// RGB //

/// \addtogroup ColorNameModel
/// \{
namespace rgb
{
/// \brief Hue
struct red_t {};
/// \brief Saturation
struct green_t {};
/// \brief Value
struct blue_t {};
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3< rgb::red_t
                    , rgb::green_t
                    , rgb::blue_t
                    > rgb_t;

////////////////////////////////////////////////////////////////////////////////
// RGBA //

/// \brief Alpha
struct alpha_t {};

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector4< rgb::red_t
                    , rgb::green_t
                    , rgb::blue_t
                    , alpha_t
                    > rgba_t;
*/

}
}
}


#endif
