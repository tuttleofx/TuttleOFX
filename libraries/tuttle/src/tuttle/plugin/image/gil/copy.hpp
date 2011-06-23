#ifndef _TUTTLE_PLUGIN_IMAGE_GIL_COPY_HPP_
#define	_TUTTLE_PLUGIN_IMAGE_GIL_COPY_HPP_

#include <boost/gil/color_base_algorithm.hpp>
#include <boost/gil/extension/toolbox/channel_view.hpp>

namespace boost {
namespace gil {

namespace detail_copy_channel {
template<class Channel, class View>
void copy_channel_if_exist( const View& src, const View& dst, const boost::mpl::true_ )
{
	copy_pixels( channel_view<Channel>(src), channel_view<Channel>(dst) );
}
template<class Channel, class View>
void copy_channel_if_exist( const View& src, const View& dst, const boost::mpl::false_ )
{
}
}

template<class Channel, class View>
void copy_channel_if_exist( const View& src, const View& dst )
{
	typedef typename contains_color<typename View::value_type, Channel>::type hasChannel;
	detail_copy_channel::copy_channel_if_exist<Channel, View>( src, dst, hasChannel() );
}

}
}

#endif

