#ifndef _TERRY_COLOR_COLORSPACE_HPP_
#define	_TERRY_COLOR_COLORSPACE_HPP_

//#include "layout.hpp"
//#include "gradation.hpp"
//#include "primaries.hpp"
//#include "temperature.hpp"

namespace terry {
namespace color {
namespace colorspace {

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

////////////////////////////////////////////////////////////////////////////////
// YPbPr //

////////////////////////////////////////////////////////////////////////////////
// HSV //

/// \addtogroup ColorNameModel
/// \{
namespace hsv
{
/// \brief Hue
struct hue_t {};
/// \brief Saturation
struct saturation_t {};
/// \brief Value
struct value_t {};
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3< hsl::hue_t
                    , hsv::saturation_t
                    , hsv::value_t
                    > hsv_t;

////////////////////////////////////////////////////////////////////////////////
// HSL //

/// \addtogroup ColorNameModel
/// \{
namespace hsl
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
typedef boost::mpl::vector3< hsl::hue_t
                    , hsl::saturation_t
                    , hsl::lightness_t
                    > hsl_t;

////////////////////////////////////////////////////////////////////////////////
// Lab //

/// \addtogroup ColorNameModel
/// \{
namespace lab
{
/// \brief Lightness
struct lightness_t {};
/// \brief chrominance dimension a: green -> magenta
struct a_t {};
/// \brief chrominance dimension a: blue -> yellow
struct b_t {};
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3< lab::lightness_t
                    , lab::a_t
                    , lab::b_t
                    > lab_t;

////////////////////////////////////////////////////////////////////////////////
// Luv //

/// \addtogroup ColorNameModel
/// \{
namespace luv
{
/// \brief Lightness
struct lightness_t {};
/// \brief chrominance dimension u
struct u_t {};
/// \brief chrominance dimension v
struct v_t {};
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3< luv::lightness_t
                    , luv::u_t
                    , luv::v_t
                    > luv_t;

////////////////////////////////////////////////////////////////////////////////
// XYZ //

/// \addtogroup ColorNameModel
/// \{
namespace XYZ
{
/// \brief X
struct X_t {};
/// \brief Y
struct Y_t {};
/// \brief Z
struct Z_t {};
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3< XYZ::X_t
                    , XYZ::Y_t
                    , XYZ::Z_t
                    > XYZ_t;

////////////////////////////////////////////////////////////////////////////////
// Yxy //

/// \addtogroup ColorNameModel
/// \{
namespace Yxy
{
/// \brief Y
struct Y_t {};
/// \brief x
struct x_t {};
/// \brief y
struct y_t {};
}
/// \}

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector3< Yxy::Y_t
                    , Yxy::x_t
                    , Yxy::y_t
                    > Yxy_t;

}

}
}

#endif

