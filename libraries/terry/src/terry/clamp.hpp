#ifndef _TERRY_CLAMP_HPP_
#define _TERRY_CLAMP_HPP_

#include <terry/color_processed_view.hpp>

namespace terry {

template <typename Channel>
struct channel_clamp
{
	typedef typename channel_base_type<Channel>::type ChannelBaseType;
	
	const ChannelBaseType _min;
	const ChannelBaseType _max;

	channel_clamp( const Channel min, const Channel max )
	: _min( min )
	, _max( max )
	{ }

	GIL_FORCEINLINE
		void operator( )(const Channel& src, Channel& dst ) const
	{
		dst = std::min( _max, std::max( _min, ChannelBaseType(src) ) );
	}
};

template<typename Pixel>
struct clamp_converter
{
	typedef typename boost::gil::channel_type<Pixel>::type Channel;

	const Channel _min;
	const Channel _max;

	clamp_converter( )
	: _min( boost::gil::channel_traits<Channel>::min_value( ) )
	, _max( boost::gil::channel_traits<Channel>::max_value( ) )
	{ }

	clamp_converter( const Channel min, const Channel max )
	: _min( min )
	, _max( max )
	{ }

	GIL_FORCEINLINE
		void operator( )(const Pixel& src, Pixel & dst ) const
	{
		using namespace boost::gil;
		static_for_each( src, dst, channel_clamp<Channel>( _min, _max ) );
	}
};


namespace detail {

template<class View, class has_scoped_channel>
struct clamp_view_type_resolver
{
	typedef typename detail::color_processed_view_type<clamp_converter<typename View::value_type>,View>::type Result;
	
	inline Result clamp_view( const View& src )
	{
		typedef typename View::value_type Pixel;
		return color_processed_view( src, clamp_converter<Pixel>() );
	}
};

template<class View>
struct clamp_view_type_resolver<View, boost::mpl::false_>
{
	typedef View Result;
	
	inline Result clamp_view( const View& src )
	{
		return src;
	}
};

/**
 * @brief Just an optimization to disable the process if the view don't use
 *        a scoped_channel_value. Without scoped_vhannel_value, we directly use
 *        the primitive type with the full range of this type. So there is
 *        nothing to clamp in this case. The values can't be outside the type
 *        range.
 */
template<class View>
struct clamp_view_type
{
	typedef typename channel_type<View>::type Channel;
	
	typedef clamp_view_type_resolver<View, typename is_scoped_channel<Channel>::type > Resolver;
	
	inline typename Resolver::Result clamp_view( const View& src )
	{
		return Resolver::clamp_view( src );
	}
};

}

template <typename View>
inline typename detail::color_processed_view_type<clamp_converter<typename View::value_type>,View>::type
clamp_view( const View& src )
{
	typedef typename View::value_type Pixel;
	return color_processed_view( src, clamp_converter<Pixel>() );
}

}

#endif

