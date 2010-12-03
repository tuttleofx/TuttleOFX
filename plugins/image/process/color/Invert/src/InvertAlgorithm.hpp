#ifndef _TUTTLE_PLUGIN_INVERT_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_INVERT_ALGORITHM_HPP_

#include <boost/gil/extension/channel.hpp>

namespace tuttle {
namespace plugin {
namespace invert {

template< typename Channel>
struct channel_invert_t
{
	void operator()( const Channel& src, Channel& dst ) const
	{
		dst = boost::gil::channel_invert( src );
	}
};

struct pixel_invert_colors_t
{
	template<class Pixel>
	Pixel operator()( const Pixel& src ) const
	{
		using namespace boost;
		typedef typename gil::channel_type<Pixel>::type Channel;
		Pixel res;
		gil::static_for_each( src, res, channel_invert_t<Channel>() );
		gil::assign_channel_if_exists_t< Pixel, gil::alpha_t >()( src, res );
		return res;
	}
};


}
}
}

#endif
