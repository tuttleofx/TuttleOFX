#ifndef _BOOST_GIL_COLOR_DISTRIBUTION_HPP_
#define _BOOST_GIL_COLOR_DISTRIBUTION_HPP_

#include <boost/gil/extension/typedefs.hpp>

namespace boost { namespace gil {

namespace colorDistribution{
struct linear{};
struct sRGB{};
struct cineon{};
struct rec709{};
struct rec601{};
}

/// @brief change the color distribution
template< typename Channel,
          class IN,
          class OUT >
struct channel_color_distribution_t : public std::binary_function<Channel, Channel, Channel>
{
	BOOST_STATIC_ASSERT( sizeof( Channel ) >= 0 ) ; // Error: there is no specialization for this color distribution conversion.
};

#define GIL_DEFINE_SAME_COLOR_DISTRIBUTION( COLORDISTRIBUTION ) \
    template < typename Channel > \
    struct channel_color_distribution_t< Channel, COLORDISTRIBUTION, COLORDISTRIBUTION > \
		: public std::binary_function< Channel, Channel, Channel > \
	{ \
		typename channel_traits<Channel>::reference operator()( typename channel_traits<Channel>::const_reference ch1, \
		                                                            typename channel_traits<Channel>::reference ch2 ) const \
		{ \
			BOOST_THROW_EXCEPTION( std::logic_error( "This conversion is an identity conversion, you don't need to do the process." ) ); \
			return ch2 = ch1; \
		} \
	};

/**
 * @brief Lin to sRGB
 * Formula taken from Computational technology by Kang.
 */
template< typename Channel >
struct channel_color_distribution_t<Channel, colorDistribution::linear, colorDistribution::sRGB> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;

	typename channel_traits<Channel>::reference operator()( typename channel_traits<Channel>::const_reference ch1,
	                                                        typename channel_traits<Channel>::reference ch2 ) const
	{
		static const double inv_2_4 = 1.0 / 2.4;
		const T v1                  = channel_convert<T>( ch1 );
		T v2;

		if( v1 > 0.00304 )
		{
			v2 = 1.055 * exp( log( v1 ) * inv_2_4 ) - 0.055;
		}
		else
		{
			v2 = 12.92 * v1;
		}
		return ch2 = channel_convert<Channel>( v2 );
	}

};

/**
 * @brief sRGB to Lin
 * Formula taken from Computational technology by Kang.
 */
template< typename Channel >
struct channel_color_distribution_t<Channel, colorDistribution::sRGB, colorDistribution::linear> : public std::binary_function<Channel, Channel, Channel>
{
	typedef typename floating_channel_type_t<Channel>::type T;

	typename channel_traits<Channel>::reference operator()( typename channel_traits<Channel>::const_reference ch1,
	                                                        typename channel_traits<Channel>::reference ch2 ) const
	{
		const T v1 = channel_convert<T>( ch1 );
		T v2;

		if( v1 > 0.03928 )
		{
			v2 = exp( log( ( v1 + 0.055 ) / 1.055 ) * 2.4 );
		}
		else
		{
			v2 = v1 / 12.92;
		}
		return ch2 = channel_convert<Channel>( v2 );
	}

};

/**
 * @brief sRGB to custom, using intermediate linear conversion
 */
template< typename Channel,
          class OUT >
struct channel_color_distribution_t<Channel, colorDistribution::sRGB, OUT> : public std::binary_function<Channel, Channel, Channel>
{
	typedef colorDistribution::sRGB IN;
	typename channel_traits<Channel>::reference operator()( typename channel_traits<Channel>::const_reference ch1,
	                                                        typename channel_traits<Channel>::reference ch2 ) const
	{
		channel_color_distribution_t<Channel, IN, colorDistribution::linear>( ch1, ch2 );
		channel_color_distribution_t<Channel, colorDistribution::linear, OUT>( ch2, ch2 );
		return ch2;
	}
};

GIL_DEFINE_SAME_COLOR_DISTRIBUTION( colorDistribution::linear );
GIL_DEFINE_SAME_COLOR_DISTRIBUTION( colorDistribution::sRGB );
//GIL_DEFINE_SAME_COLOR_DISTRIBUTION( colorDistribution::cineon );
//GIL_DEFINE_SAME_COLOR_DISTRIBUTION( colorDistribution::rec709 );
//GIL_DEFINE_SAME_COLOR_DISTRIBUTION( colorDistribution::rec601 );

template< typename Pixel,
          class IN,
          class OUT >
struct pixel_color_distribution_t
{
	Pixel& operator()( const Pixel& p1,
	                   Pixel& p2 ) const
	{
		static_for_each( p1, p2,
		                 channel_color_distribution_t< typename channel_type<Pixel>::type,
		                                               IN, OUT >()
		                 );
		return p2;
	}
};

template< class IN,
          class OUT >
struct transform_pixel_color_distribution_t
{
	template< typename Pixel>
	Pixel operator()( const Pixel& p1 ) const
	{
		Pixel p2;
		pixel_color_distribution_t<Pixel, IN, OUT>( )( p1, p2 );
		return p2;
	}
};

}
}

#endif
