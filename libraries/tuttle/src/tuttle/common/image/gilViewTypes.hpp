/* 
 * File:   gilViewTypes.hpp
 * Author: Administrateur
 *
 * Created on 23 novembre 2009, 11:11
 */

#ifndef _GILVIEWTYPES_HPP
#define	_GILVIEWTYPES_HPP

#include <boost/integer.hpp>  // for boost::uint_t
#include <boost/gil/gil_all.hpp>
#include <boost/type_traits.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>

#define I10_MIN 0               // 0
#define I10_MAX 1023            // 2^10 - 1

namespace boost { namespace gil  {


/******************************************************************************
 * Bit stream
 *****************************************************************************/
typedef uint16_t uint10_t;
typedef bit_aligned_pixel_reference< uint64_t
								  , mpl::vector4_c<uint10_t, 10, 10, 10, 10>
								  , rgba_layout_t
								  , true
								  >  rgba10101010_ref_t;

// A mutable iterator over RGBA10 pixels
typedef bit_aligned_pixel_iterator< rgba10101010_ref_t >
	rgba10101010_ptr_t;

typedef std::iterator_traits< rgba10101010_ptr_t >::value_type
	rgba10101010_pixel_t;

typedef view_type_from_pixel< rgba10101010_pixel_t >::type
	rgba10101010_view_t;

typedef bit_aligned_pixel_reference< uint64_t
								  , mpl::vector4_c<uint10_t, 10, 10, 10, 10>
								  , rgba_layout_t
								  , false
								  >  rgbac10101010_ref_t;

// A mutable iterator over RGBA10 pixels
typedef bit_aligned_pixel_iterator< rgbac10101010_ref_t >
	rgbac10101010_ptr_t;

typedef std::iterator_traits< rgbac10101010_ptr_t >::value_type
	rgbac10101010_pixel_t;

typedef view_type_from_pixel< rgbac10101010_pixel_t >::type
	rgbac10101010_view_t;

/******************************************************************************
 * Packed on bytes view types
 *****************************************************************************/
typedef packed_channel_value<10> bits10;

GIL_DEFINE_BASE_TYPEDEFS(10, gray)
GIL_DEFINE_BASE_TYPEDEFS(10, bgr)
GIL_DEFINE_BASE_TYPEDEFS(10, argb)
GIL_DEFINE_BASE_TYPEDEFS(10, bgra)
GIL_DEFINE_BASE_TYPEDEFS(10, abgr)

GIL_DEFINE_ALL_TYPEDEFS(10, rgb)
GIL_DEFINE_ALL_TYPEDEFS(10, rgba)
GIL_DEFINE_ALL_TYPEDEFS(10, cmyk)

template <int N> struct devicen_t;
template <int N> struct devicen_layout_t;
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(10, dev2n, devicen_t<2>, devicen_layout_t<2>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(10, dev3n, devicen_t<3>, devicen_layout_t<3>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(10, dev4n, devicen_t<4>, devicen_layout_t<4>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL(10, dev5n, devicen_t<5>, devicen_layout_t<5>)

}
}  // namespace boost::gil


#endif	/* _GILVIEWTYPES_HPP */

