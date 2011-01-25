/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_PIXEL_NUMERIC_OPERATIONS_HPP
#define GIL_PIXEL_NUMERIC_OPERATIONS_HPP

/*!
/// \file               
/// \brief Structures for pixel-wise numeric operations
/// \author Lubomir Bourdev and Hailin Jin \n
///         Adobe Systems Incorporated
/// \date   2005-2007 \n Last updated on February 6, 2007
/// Currently defined structures:
///     pixel_plus_t (+), pixel_minus_t (-)
///     pixel_multiplies_scalar_t (*), pixel_divides_scalar_t (/)
///     pixel_halves_t (/=2), pixel_zeros_t (=0)
///     pixel_assigns_t (=)
*/

#include <functional>
#include <boost/gil/gil_config.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/color_base_algorithm.hpp>
#include "channel_numeric_operations.hpp"

namespace boost { namespace gil {

/// \ingroup PixelNumericOperations
/// \brief construct for adding two pixels
template <typename PixelRef1, // models pixel concept
          typename PixelRef2, // models pixel concept
          typename PixelR>    // models pixel value concept
struct pixel_plus_t {
	GIL_FORCEINLINE
    PixelR operator() (const PixelRef1& p1,
                       const PixelRef2& p2) const {
        PixelR result;
        static_transform(p1,p2,result,
                           channel_plus_t<typename channel_type<PixelRef1>::type,
                                          typename channel_type<PixelRef2>::type,
                                          typename channel_type<PixelR>::type>());
        return result;
    }
};

/// \ingroup PixelNumericOperations
/// \brief construct for subtracting two pixels
template <typename PixelRef1, // models pixel concept
          typename PixelRef2, // models pixel concept
          typename PixelR>    // models pixel value concept
struct pixel_minus_t {
	GIL_FORCEINLINE
    PixelR operator() (const PixelRef1& p1,
                       const PixelRef2& p2) const {
        PixelR result;
        static_transform(p1,p2,result,
                           channel_minus_t<typename channel_type<PixelRef1>::type,
                                           typename channel_type<PixelRef2>::type,
                                           typename channel_type<PixelR>::type>());
        return result;
    }
};

/// \ingroup PixelNumericOperations
/// \brief construct for adding two pixels
template <typename PixelRef1, // models pixel concept
          typename PixelRef2, // models pixel concept
          typename PixelR>    // models pixel value concept
struct pixel_multiplies_t {
	GIL_FORCEINLINE
    PixelR operator() (const PixelRef1& p1,
                       const PixelRef2& p2) const {
        PixelR result;
        static_transform(p1,p2,result,
                           channel_multiplies_t<typename channel_type<PixelRef1>::type,
                                          typename channel_type<PixelRef2>::type,
                                          typename channel_type<PixelR>::type>());
        return result;
    }
};

/// \ingroup PixelNumericOperations
/// \brief construct for subtracting two pixels
template <typename PixelRef1, // models pixel concept
          typename PixelRef2, // models pixel concept
          typename PixelR>    // models pixel value concept
struct pixel_divides_t {
	GIL_FORCEINLINE
    PixelR operator() (const PixelRef1& p1,
                       const PixelRef2& p2) const {
        PixelR result;
        static_transform(p1,p2,result,
                           channel_divides_t<typename channel_type<PixelRef1>::type,
                                           typename channel_type<PixelRef2>::type,
                                           typename channel_type<PixelR>::type>());
        return result;
    }
};

/// \ingroup PixelNumericOperations
/// \brief construct for multiplying scalar to a pixel
template <typename PixelRef, // models pixel concept
          typename Scalar,   // models a scalar type
          typename PixelR=PixelRef>   // models pixel value concept
struct pixel_multiplies_scalar_t {
	GIL_FORCEINLINE
    PixelR operator () (const PixelRef& p,
                        const Scalar& s) const {
        PixelR result;
        static_transform(p,result,
                           std::bind2nd(channel_multiplies_scalar_t<typename channel_type<PixelRef>::type,
                                                                    Scalar,
                                                                    typename channel_type<PixelR>::type>(),s));
        return result;
    }
};

/// \ingroup PixelNumericOperations
/// \brief construct for dividing a pixel by a scalar
template <typename PixelRef, // models pixel concept
          typename Scalar,   // models a scalar type
          typename PixelR=PixelRef>   // models pixel value concept
struct pixel_divides_scalar_t {
	GIL_FORCEINLINE
    PixelR operator () (const PixelRef& p,
                        const Scalar& s) const {
        PixelR result;
        static_transform(p,result,
                           std::bind2nd(channel_divides_scalar_t<typename channel_type<PixelRef>::type,
                                                                 Scalar,
                                                                 typename channel_type<PixelR>::type>(),s));
        return result;
    }
};

/// \ingroup PixelNumericOperations
/// \brief construct to compute pow on a pixel
template <typename PixelRef, int n, typename PixelR=PixelRef> // models pixel concept
struct pixel_pow_t {
	GIL_FORCEINLINE
    PixelR operator () (const PixelRef& p) const {
        PixelR result;
        static_transform(p,result, channel_pow_t<typename channel_type<PixelRef>::type,
						                         n,
												 typename channel_type<PixelR>::type>());
        return result;
    }
};

/// \ingroup PixelNumericOperations
/// \brief construct for dividing a pixel by 2
template <typename PixelRef> // models pixel concept
struct pixel_halves_t {
	GIL_FORCEINLINE
    PixelRef& operator () (PixelRef& p) const {
        static_for_each(p,channel_halves_t<typename channel_type<PixelRef>::type>());
        return p;
    }
};

template <typename Pixel>
GIL_FORCEINLINE
void pixel_halves(Pixel& p)
{
    pixel_halves_t<Pixel>()(p);
}

/// \ingroup PixelNumericOperations
/// \brief construct for setting a pixel to zero (for whatever zero means)
template <typename PixelRef> // models pixel concept
struct pixel_zeros_t {
	GIL_FORCEINLINE
    PixelRef& operator () (PixelRef& p) const {
        static_for_each(p,channel_zeros_t<typename channel_type<PixelRef>::type>());
        return p;
    }
};

template <typename Pixel>
GIL_FORCEINLINE
Pixel& pixel_zeros(Pixel& p)
{
    return pixel_zeros_t<Pixel>()(p);
}

template <typename Pixel>
GIL_FORCEINLINE
Pixel pixel_zeros()
{
	Pixel p;
	return pixel_zeros_t<Pixel>()(p);
}


/// \ingroup PixelNumericOperations
///definition and a generic implementation for casting and assigning a pixel to another
///user should specialize it for better performance
template <typename PixelRef,  // models pixel concept
          typename PixelRefR> // models pixel concept
struct pixel_assigns_t {
	GIL_FORCEINLINE
    PixelRefR& operator () (const PixelRef& src,
                           PixelRefR& dst) const {
        static_for_each(src,dst,channel_assigns_t<typename channel_type<PixelRef>::type,
                                                  typename channel_type<PixelRefR>::type>());
        return dst;
    }
};

template <typename Pixel,  // models pixel concept
          typename PixelR> // models pixel concept
GIL_FORCEINLINE
void pixel_assigns(const Pixel& src, Pixel& dst)
{
	pixel_assigns_t<Pixel,PixelR>()(src, dst);
}


/// \ingroup PixelNumericOperations
///definition and a generic implementation for casting and assigning a pixel to another
///user should specialize it for better performance
template <typename Scalar,  // models pixel concept
          typename PixelR> // models pixel concept
struct pixel_assigns_scalar_t
{
	GIL_FORCEINLINE
    PixelR& operator()( const Scalar s,
                        PixelR& dst ) const
	{
        static_for_each( dst,
                         std::bind1st( channel_assigns_t<Scalar, typename channel_type<PixelR>::type>(), s ) );
        return dst;
    }
};

template <typename Scalar,  // models pixel concept
          typename PixelR> // models pixel concept
GIL_FORCEINLINE
void pixel_assigns_scalar(const Scalar s, PixelR& dst)
{
	pixel_assigns_scalar_t<Scalar,PixelR>()(s, dst);
}


/// \ingroup PixelNumericOperations
/// \brief construct for setting a pixel to the min channel value (see channel_traits::min_value)
template <typename PixelR> // models pixel concept
struct pixel_assigns_min_t
{
	typedef typename boost::gil::channel_type<PixelR>::type Channel;
	GIL_FORCEINLINE
    PixelR& operator()(PixelR& dst) const
	{
		pixel_assigns_scalar_t<Channel,PixelR>()( channel_traits<Channel>::min_value(), dst);
        return dst;
    }
};

template <typename Pixel>
GIL_FORCEINLINE
void pixel_assigns_min(Pixel& p)
{
    pixel_assigns_min_t<Pixel>()(p);
}

/// \ingroup PixelNumericOperations
/// \brief construct for setting a pixel to the max channel value (see channel_traits::max_value)
template <typename PixelR> // models pixel concept
struct pixel_assigns_max_t
{
	typedef typename boost::gil::channel_type<PixelR>::type Channel;
	GIL_FORCEINLINE
    PixelR& operator() (PixelR& dst) const
	{
		pixel_assigns_scalar_t<Channel,PixelR>()( channel_traits<Channel>::max_value() , dst);
        return dst;
    }
};

template <typename Pixel>
GIL_FORCEINLINE
void pixel_assigns_max(Pixel& p)
{
    pixel_assigns_max_t<Pixel>()(p);
}


} }  // namespace boost::gil

#endif
