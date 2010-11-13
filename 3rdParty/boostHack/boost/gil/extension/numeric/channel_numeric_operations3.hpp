#ifndef GIL_CHANNEL_NUMERIC_OPERATIONS3_HPP
#define GIL_CHANNEL_NUMERIC_OPERATIONS3_HPP

/*!
/// \brief Structures for channel-wise numeric operations
/// Currently defined structures:
///    channel_min_assign_t,
///    channel_max_assign_t,
*/

#include <functional>
#include <boost/gil/gil_config.hpp>
#include <boost/gil/channel.hpp>

namespace boost { namespace gil {

/// \ingroup ChannelNumericOperations
/// \brief ch2 = min( ch1, ch2 )
/// structure for adding one channel to another
/// this is a generic implementation; user should specialize it for better performance
template <typename ChannelSrc, typename ChannelDst>
struct channel_assign_min_t : public std::binary_function<ChannelSrc, ChannelDst, ChannelDst>
{
	typename channel_traits<ChannelDst>::reference operator()( typename channel_traits<ChannelSrc>::const_reference ch1,
	                                                           typename channel_traits<ChannelDst>::reference ch2 ) const
	{
		return ch2 = std::min( ChannelDst( ch1 ), ch2 );
	}

};

/// \ingroup ChannelNumericOperations
/// \brief ch2 = max( ch1, ch2 )
/// this is a generic implementation; user should specialize it for better performance
template <typename ChannelSrc, typename ChannelDst>
struct channel_assign_max_t : public std::binary_function<ChannelSrc, ChannelDst, ChannelDst>
{
	typename channel_traits<ChannelDst>::reference operator()( typename channel_traits<ChannelSrc>::const_reference ch1,
	                                                           typename channel_traits<ChannelDst>::reference ch2 ) const
	{
		return ch2 = std::max( ChannelDst( ch1 ), ch2 );
	}
};


} }  // namespace boost::gil

#endif
