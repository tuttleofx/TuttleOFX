#ifndef _TUTTLE_PLUGIN_SOBEL_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_SOBEL_ALGORITHM_HPP_

#include <tuttle/plugin/image/gil/channel.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/channel_numeric_operations2.hpp>
#include <boost/math/special_functions/pow.hpp>
#include <boost/math/constants/constants.hpp>

namespace tuttle {
namespace plugin {
namespace sobel {

namespace bgil = boost::gil;
namespace bm = boost::math;

/**
 * @brief Compute the direction from the (x, y) coordinates of the input vector.
 */
template< typename Channel>
struct channel_gradientDirection_t
{
	void operator()( const Channel& x, const Channel& y, Channel& res ) const
	{
		res = std::atan2( y, x );
	}
};

/**
 * @brief Compute the direction from the (x, y) coordinates of the input vector, limited between 0 and PI.
 */
template< typename Channel>
struct channel_gradientDirectionAbs_t
{
	void operator()( const Channel& x, const Channel& y, Channel& res ) const
	{
		channel_gradientDirection_t<Channel>()(x, y, res);
		if( res < 0 )
			res += bm::constants::pi<typename bgil::base_channel_value<Channel>::type>();
	}
};

/**
 * @brief Compute the norm from the (x, y) coordinates of the input vector.
 */
template<typename Channel>
struct channel_norm_t
{
	void operator()( const Channel& a, const Channel& b, Channel& res ) const
	{
		res = std::sqrt( bm::pow<2>(a) + bm::pow<2>(b) );
	}
};

/**
 * @brief Compute the Manhattan norm from the (x, y) coordinates of the input vector.
 */
template<typename Channel>
struct channel_normManhattan_t
{
	void operator()( const Channel& a, const Channel& b, Channel& res ) const
	{
		res = std::abs(a) + std::abs(b);
	}
};



/**
 * @brief Compute the max value.
 */
template<typename Channel>
struct channel_max_assign_t
{
	void operator()( const Channel& v, Channel& res ) const
	{
		res = std::max( v, res );
	}
};

/**
 * @brief Compute the max for each channel.
 */
template<typename CPixel>
struct pixel_max_by_channel_t
{
	CPixel value;
	template<typename Pixel>
	void operator()( const Pixel& v )
	{
		bgil::static_for_each(
			v,
			value,
			channel_max_assign_t<typename boost::gil::channel_type<CPixel>::type>() );
	}
};

/**
 * @brief Normalize an image.
 */
template<typename CPixel>
struct pixel_normalize_t
{
	const CPixel& _max;
	typedef typename boost::gil::channel_type<CPixel>::type CChannel;
	typedef typename boost::gil::base_channel_value<CChannel>::type CChannelBaseType;
	pixel_normalize_t( const CPixel& max )
	: _max(max)
	{
		BOOST_STATIC_ASSERT( boost::is_floating_point<CChannelBaseType>::value );
	}

	template<typename Pixel>
	void operator()( const Pixel& pix ) const
	{
//		typedef typename boost::gil::channel_type<Pixel>::type Channel;
//		typedef typename boost::gil::base_channel_value<Channel>::type ChannelBaseType;
//		BOOST_STATIC_ASSERT( boost::is_floating_point<CChannelBaseType>::value );
//
//		bgil::static_for_each(
//			_max,
//			pix,
//			boost::gil::channel_divides_assign_t<CPixel, Pixel>() );
	}
};


}
}
}

#endif
