#ifndef _TERRY_OPENEXR_HALF_HPP_
#define _TERRY_OPENEXR_HALF_HPP_

/**
 * @warning The file depends on the half.h file of OpenEXR project.
 */

#include <half.h> // OpenEXR Half float

#include <boost/integer.hpp> // for boost::uint_t
#include <boost/gil/gil_all.hpp>
#include <boost/type_traits.hpp>

namespace boost
{
namespace gil
{

template <int N>
struct devicen_t;
template <int N>
struct devicen_layout_t;

///////////////////////////////////////////////////////////////////////////////
///  @brief Define channel traits to support ilm half
///////////////////////////////////////////////////////////////////////////////

template <>
struct channel_traits<half> : public detail::channel_traits_impl<half, false>
{
    static half min_value() { return HALF_MIN; }
    static half max_value() { return HALF_MAX; }
};

///////////////////////////////////////////////////////////////////////////////
/// @typedef an half trait between 0 and 1
///////////////////////////////////////////////////////////////////////////////

struct half_zero
{
    static half apply() { return half(0.0f); }
};
struct half_one
{
    static half apply() { return half(1.0f); }
};
typedef scoped_channel_value<half, half_zero, half_one> bits16h;

inline bits16h operator+(bits16h _lhs, bits16h _rhs)
{
    return bits16h(_lhs += half(_rhs));
}

inline bits16h operator-(bits16h _lhs, bits16h _rhs)
{
    return bits16h(_lhs -= half(_rhs));
}

inline bits16h operator*(bits16h _lhs, bits16h _rhs)
{
    return bits16h(_lhs *= half(_rhs));
}

inline bits16h operator/(bits16h _lhs, bits16h _rhs)
{
    return bits16h(_lhs /= half(_rhs));
}

///////////////////////////////////////////////////////////////////////////////
/// channel conversion from half channel
///////////////////////////////////////////////////////////////////////////////
template <typename DstChannelV>
struct channel_converter_unsigned<bits16h, DstChannelV> : public std::unary_function<bits16h, DstChannelV>
{
    DstChannelV operator()(bits16h h) const
    {
        return DstChannelV(half(h) * float(channel_traits<DstChannelV>::max_value()) + 0.5f);
    }
};

///////////////////////////////////////////////////////////////////////////////
/// channel conversion to half channel
///////////////////////////////////////////////////////////////////////////////
template <typename SrcChannelV>
struct channel_converter_unsigned<SrcChannelV, bits16h> : public std::unary_function<SrcChannelV, bits16h>
{
    bits16h operator()(SrcChannelV x) const { return bits16h(float(x) / float(channel_traits<SrcChannelV>::max_value())); }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct channel_converter_unsigned<bits16h, bits16h> : public std::unary_function<bits16h, bits16h>
{
    bits16h operator()(bits16h h) const { return h; }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct channel_converter_unsigned<bits32f, bits16h> : public std::unary_function<bits32f, bits16h>
{
    bits16h operator()(bits32f f) const { return bits16h(half(f)); }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct channel_converter_unsigned<bits16h, bits32f> : public std::unary_function<bits16h, bits32f>
{
    bits32f operator()(bits16h h) const { return bits32f(float(half(h))); }
};

///////////////////////////////////////////////////////////////////////////////
template <>
struct channel_multiplier_unsigned<bits16h> : public std::binary_function<bits16h, bits16h, bits16h>
{
    bits16h operator()(bits16h a, bits16h b) const { return a * b; }
};

///////////////////////////////////////////////////////////////////////////////
///  defines gil types with new bits16h trait
///////////////////////////////////////////////////////////////////////////////

GIL_DEFINE_BASE_TYPEDEFS(16h, gray)
GIL_DEFINE_BASE_TYPEDEFS(16h, bgr)
GIL_DEFINE_BASE_TYPEDEFS(16h, argb)
GIL_DEFINE_BASE_TYPEDEFS(16h, abgr)
GIL_DEFINE_BASE_TYPEDEFS(16h, bgra)

GIL_DEFINE_ALL_TYPEDEFS(16h, rgb)
GIL_DEFINE_ALL_TYPEDEFS(16h, rgba)
GIL_DEFINE_ALL_TYPEDEFS(16h, cmyk)

template <int N>
struct devicen_t;
template <int N>
struct devicen_layout_t;
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16h, dev2n, devicen_t<2>, devicen_layout_t<2>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16h, dev3n, devicen_t<3>, devicen_layout_t<3>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16h, dev4n, devicen_t<4>, devicen_layout_t<4>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(16h, dev5n, devicen_t<5>, devicen_layout_t<5>)
}
}

#endif
