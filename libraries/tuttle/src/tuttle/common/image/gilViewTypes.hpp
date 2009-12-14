/*
 * File:   gilViewTypes.hpp
 * Author: Administrateur
 *
 * Created on 23 novembre 2009, 11:11
 */

#ifndef _GILVIEWTYPES_HPP
#define _GILVIEWTYPES_HPP

#include <boost/integer.hpp>  // for boost::uint_t
#include <boost/gil/gil_all.hpp>
#include <boost/type_traits.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>

#define I10_MIN 0               // 0
#define I10_MAX 1023            // 2^10 - 1

namespace boost {
namespace gil  {

/******************************************************************************
 * Bit stream
 *****************************************************************************/
typedef uint16_t uint10_t;
typedef uint16_t uint12_t;

/// 10 bits rgba bit stream
typedef bit_aligned_pixel_reference< uint64_t,
                                     mpl::vector4_c<uint10_t, 10, 10, 10, 10>,
                                     rgba_layout_t,
                                     true
                                     >  rgba10_stream_ref_t;
typedef bit_aligned_pixel_iterator<rgba10_stream_ref_t> rgba10_stream_ptr_t;
typedef std::iterator_traits<rgba10_stream_ptr_t>::value_type rgba10_stream_pixel_t;
typedef view_type_from_pixel<rgba10_stream_pixel_t>::type rgba10_stream_view_t;

/// 10 bits abgr bit stream
typedef bit_aligned_pixel_reference< uint64_t,
                                     mpl::vector4_c<uint10_t, 10, 10, 10, 10>,
                                     abgr_layout_t,
                                     true
                                     >  abgr10_stream_ref_t;
typedef bit_aligned_pixel_iterator<abgr10_stream_ref_t> abgr10_stream_ptr_t;
typedef std::iterator_traits<abgr10_stream_ptr_t>::value_type abgr10_stream_pixel_t;
typedef view_type_from_pixel<abgr10_stream_pixel_t>::type abgr10_stream_view_t;

/// 12 bits rgba bit stream
typedef bit_aligned_pixel_reference< uint64_t,
                                     mpl::vector4_c<uint12_t, 12, 12, 12, 12>,
                                     rgba_layout_t,
                                     true
                                     >  rgba12_stream_ref_t;
typedef bit_aligned_pixel_iterator<rgba12_stream_ref_t> rgba12_stream_ptr_t;
typedef std::iterator_traits<rgba12_stream_ptr_t>::value_type rgba12_stream_pixel_t;
typedef view_type_from_pixel<rgba12_stream_pixel_t>::type rgba12_stream_view_t;

/// 12 bits abgr bit stream
typedef bit_aligned_pixel_reference< uint64_t,
                                     mpl::vector4_c<uint12_t, 12, 12, 12, 12>,
                                     abgr_layout_t,
                                     true
                                     >  abgr12_stream_ref_t;
typedef bit_aligned_pixel_iterator<abgr12_stream_ref_t> abgr12_stream_ptr_t;
typedef std::iterator_traits<abgr12_stream_ptr_t>::value_type abgr12_stream_pixel_t;
typedef view_type_from_pixel<abgr12_stream_pixel_t>::type abgr12_stream_view_t;

/// 12 bits rgba packed to short
typedef const packed_channel_reference<uint64_t, 04, 12,true> rgba12_packed_channel0_t;
typedef const packed_channel_reference<uint64_t, 20, 12,true> rgba12_packed_channel1_t;
typedef const packed_channel_reference<uint64_t, 36, 12,true> rgba12_packed_channel2_t;
typedef const packed_channel_reference<uint64_t, 52, 12,true> rgba12_packed_channel3_t;
typedef mpl::vector4<rgba12_packed_channel0_t,rgba12_packed_channel1_t,
					 rgba12_packed_channel2_t,rgba12_packed_channel3_t> rgba12_packed_channels_t;
typedef packed_pixel<uint64_t, rgba12_packed_channels_t, rgba_layout_t> rgba12_packed_pixel_t;
typedef view_type_from_pixel<rgba12_packed_pixel_t>::type rgba12_packed_view_t;

/// 12 bits abgr packed to short
typedef const packed_channel_reference<uint64_t, 04, 12,true> abgr12_packed_channel0_t;
typedef const packed_channel_reference<uint64_t, 20, 12,true> abgr12_packed_channel1_t;
typedef const packed_channel_reference<uint64_t, 36, 12,true> abgr12_packed_channel2_t;
typedef const packed_channel_reference<uint64_t, 52, 12,true> abgr12_packed_channel3_t;
typedef mpl::vector4<abgr12_packed_channel0_t,abgr12_packed_channel1_t,
					 abgr12_packed_channel2_t,abgr12_packed_channel3_t> abgr12_packed_channels_t;
typedef packed_pixel<uint64_t, abgr12_packed_channels_t, abgr_layout_t> abgr12_packed_pixel_t;
typedef view_type_from_pixel<abgr12_packed_pixel_t>::type abgr12_packed_view_t;

/// 10 bits rgba packed to short
typedef const packed_channel_reference<uint64_t, 22, 10,true> rgba10_packed_channel0_t;
typedef const packed_channel_reference<uint64_t, 12, 10,true> rgba10_packed_channel1_t;
typedef const packed_channel_reference<uint64_t, 02, 10,true> rgba10_packed_channel2_t;
typedef const packed_channel_reference<uint64_t, 54, 10,true> rgba10_packed_channel3_t;
typedef mpl::vector4<rgba10_packed_channel0_t,rgba10_packed_channel1_t,
					 rgba10_packed_channel2_t,rgba10_packed_channel3_t> rgba10_packed_channels_t;
typedef packed_pixel<uint64_t, rgba10_packed_channels_t, rgba_layout_t> rgba10_packed_pixel_t;
typedef view_type_from_pixel<rgba10_packed_pixel_t>::type rgba10_packed_view_t;

/// 10 bits abgr packed to short
typedef const packed_channel_reference<uint64_t, 4, 10,true> abgr10_packed_channel0_t;
typedef const packed_channel_reference<uint64_t, 20,10,true> abgr10_packed_channel1_t;
typedef const packed_channel_reference<uint64_t, 36,10,true> abgr10_packed_channel2_t;
typedef const packed_channel_reference<uint64_t, 52,10,true> abgr10_packed_channel3_t;
typedef mpl::vector4<abgr10_packed_channel0_t,abgr10_packed_channel1_t,
					 abgr10_packed_channel2_t,abgr10_packed_channel3_t> abgr10_packed_channels_t;
typedef packed_pixel<uint64_t, abgr10_packed_channels_t, abgr_layout_t> abgr10_packed_pixel_t;
typedef view_type_from_pixel<abgr10_packed_pixel_t>::type abgr10_packed_view_t;


/******************************************************************************
 * Packed on bytes view types
 *****************************************************************************/
typedef packed_channel_value<10> bits10;

GIL_DEFINE_BASE_TYPEDEFS( 10, gray )
GIL_DEFINE_BASE_TYPEDEFS( 10, bgr )
GIL_DEFINE_BASE_TYPEDEFS( 10, argb )
GIL_DEFINE_BASE_TYPEDEFS( 10, bgra )
GIL_DEFINE_BASE_TYPEDEFS( 10, abgr )

GIL_DEFINE_ALL_TYPEDEFS( 10, rgb )
GIL_DEFINE_ALL_TYPEDEFS( 10, rgba )
GIL_DEFINE_ALL_TYPEDEFS( 10, cmyk )

template <int N>
struct devicen_t;
template <int N>
struct devicen_layout_t;
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL( 10, dev2n, devicen_t<2>, devicen_layout_t<2>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL( 10, dev3n, devicen_t<3>, devicen_layout_t<3>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL( 10, dev4n, devicen_t<4>, devicen_layout_t<4>)
GIL_DEFINE_ALL_TYPEDEFS_INTERNAL( 10, dev5n, devicen_t<5>, devicen_layout_t<5>)

}
}  // namespace boost::gil

#endif  /* _GILVIEWTYPES_HPP */

