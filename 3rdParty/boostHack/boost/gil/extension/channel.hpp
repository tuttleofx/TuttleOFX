#ifndef _GIL_EXTENSION_CHANNEL_HPP_
#define _GIL_EXTENSION_CHANNEL_HPP_

namespace boost {
namespace gil {

template <typename ChannelValue>
struct channel_base_type
{
	typedef ChannelValue type;
};

template <typename ChannelValue, typename MinV, typename MaxV>
struct channel_base_type<scoped_channel_value<ChannelValue, MinV, MaxV> >
{
	typedef ChannelValue type;
};



template< class Pixel, class Channel, class HasChannel = typename boost::gil::contains_color<Pixel,Channel>::type >
struct assign_channel_if_exists_t
{
	void operator()( const Pixel& src, const Pixel& dst ) const;
};

// no alpha, nothing to do...
template<class Pixel, class Channel>
struct assign_channel_if_exists_t<Pixel, Channel, boost::mpl::false_>
{
	void operator()( const Pixel& src, Pixel& dst ) const
	{
	}
};

template<class Pixel, class Channel>
struct assign_channel_if_exists_t<Pixel, Channel, boost::mpl::true_>
{
	void operator()( const Pixel& src, Pixel& dst ) const
	{
		using namespace boost;
		get_color( dst, Channel() ) = get_color( src, Channel() );
	}
};


}
}

#endif

