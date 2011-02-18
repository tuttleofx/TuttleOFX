/*!
/// \brief Structures for pixel-wise numeric operations
/// Currently defined structures:
///     pixel_min_assign_t,
///     pixel_max_assign_t,
*/
#ifndef GIL_PIXEL_NUMERIC_OPERATIONS3_HPP
#define GIL_PIXEL_NUMERIC_OPERATIONS3_HPP

#include "channel_numeric_operations_minmax.hpp"
#include <boost/gil/extension/channel.hpp>

#include <boost/gil/gil_config.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/color_base_algorithm.hpp>

#include <functional>

namespace boost { namespace gil {

/// \ingroup PixelNumericOperations
/// \brief p2 = min( p1, p2 )
template <typename PixelSrc, // models pixel concept
          typename PixelDst>
// models pixel value concept
struct pixel_assign_min_t
{
	GIL_FORCEINLINE
	PixelDst& operator()( const PixelSrc& p1,
	                      PixelDst& p2 ) const
	{
		static_for_each( p1, p2,
		                 channel_assign_min_t<typename channel_type<PixelSrc>::type,
		                                      typename channel_type<PixelDst>::type>() );
		return p2;
	}

};

/// \ingroup PixelNumericOperations
/// \brief p2 = max( p1, p2 )
template <typename PixelSrc, // models pixel concept
          typename PixelDst>
// models pixel value concept
struct pixel_assign_max_t
{
	GIL_FORCEINLINE
	PixelDst& operator()( const PixelSrc& p1,
	                      PixelDst& p2 ) const
	{
		static_for_each( p1, p2,
		                 channel_assign_max_t<typename channel_type<PixelSrc>::type,
		                                      typename channel_type<PixelDst>::type>() );
		return p2;
	}

};


template<typename CPixel>
struct pixel_minmax_by_channel_t
{
	typedef typename channel_type<CPixel>::type Channel;

	CPixel min;
	CPixel max;

	pixel_minmax_by_channel_t( const CPixel& v )
	: min( v )
	, max( v )
	{
	}

	template<typename Pixel>
	GIL_FORCEINLINE
	void operator()( const Pixel& v )
	{
		pixel_assign_min_t<Pixel,CPixel>()( v, min );
		pixel_assign_max_t<Pixel,CPixel>()( v, max );
	}
};

/**
 * @brief Normalize an image.
 */
template<typename Pixel, typename CPixel = Pixel>
struct pixel_scale_t
{
	typedef typename boost::gil::channel_type<Pixel>::type Channel;
	typedef typename boost::gil::channel_base_type<Channel>::type ChannelBaseType;

	const CPixel _ratio;
	CPixel _shift;

	pixel_scale_t( const CPixel& ratio, const CPixel& sCenter, const CPixel& dCenter )
	: _ratio(ratio)
	{
		BOOST_STATIC_ASSERT( boost::is_floating_point<ChannelBaseType>::value );
		// res = ( ( src - sCenter ) * _ratio ) + dCenter;
		// res = (src * _ratio) + (dCenter - (sCenter*ratio))
		// shift = (dCenter - (sCenter*ratio))
		pixel_assigns_t<CPixel,CPixel>()(
			pixel_minus_t<CPixel,CPixel,CPixel>()(
				dCenter,
				pixel_multiplies_t<CPixel,CPixel,CPixel>()(
					sCenter,
					ratio
					)
				),
			_shift );
	}

	pixel_scale_t( const CPixel& ratio, const CPixel& shift )
	: _ratio(ratio)
	, _shift(shift)
	{
		BOOST_STATIC_ASSERT( boost::is_floating_point<ChannelBaseType>::value );
	}

	GIL_FORCEINLINE
	Pixel operator()( const Pixel& src ) const
	{
		Pixel res;
		// res = (src * _ratio) + _shift
		pixel_assigns_t<CPixel,Pixel>()(
			pixel_plus_t<CPixel,CPixel,CPixel>()(
				pixel_multiplies_t<Pixel,CPixel,CPixel>()(
					src,
					_ratio
					),
				_shift ),
			res );

		return res;
	}
};

}
}

#endif
