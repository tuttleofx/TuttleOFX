#ifndef _TUTTLE_PLUGIN_SOBEL_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_SOBEL_ALGORITHM_HPP_

#include <boost/gil/extension/channel.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/numeric/channel_numeric_operations_assign.hpp>
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
	GIL_FORCEINLINE
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
	GIL_FORCEINLINE
	void operator()( const Channel& x, const Channel& y, Channel& res ) const
	{
		channel_gradientDirection_t<Channel>()(x, y, res);
		if( res < 0 )
			res += bm::constants::pi<typename bgil::channel_base_type<Channel>::type>();
	}
};

/**
 * @brief Compute the norm from the (x, y) coordinates of the input vector.
 */
template<typename Channel>
struct channel_norm_t
{
	GIL_FORCEINLINE
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
	GIL_FORCEINLINE
	void operator()( const Channel& a, const Channel& b, Channel& res ) const
	{
		res = std::abs(a) + std::abs(b);
	}
};


}
}
}

#endif
