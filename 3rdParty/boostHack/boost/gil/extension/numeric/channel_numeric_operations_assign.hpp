#ifndef GIL_CHANNEL_NUMERIC_OPERATIONS_ASSIGN_HPP
#define GIL_CHANNEL_NUMERIC_OPERATIONS_ASSIGN_HPP

/*!
/// \brief Structures for channel-wise numeric operations
/// Currently defined structures:
///    channel_plus_assign_t (+=),
///    channel_minus_assign_t (-=),
///    channel_multiplies_assign_t (*=),
///    channel_divides_assign_t (/=),
///    channel_plus_scalar_assign_t (+=s),
///    channel_minus_scalar_assign_t (-=s),
///    channel_multiplies_scalar_assign_t (*=s),
///    channel_divides_scalar_assign_t (/=s),
*/

#include <functional>
#include <boost/gil/gil_config.hpp>
#include <boost/gil/channel.hpp>

namespace boost { namespace gil {

/// \ingroup ChannelNumericOperations
/// \brief ch2 += ch1
/// structure for adding one channel to another
/// this is a generic implementation; user should specialize it for better performance
template <typename ChannelSrc,typename ChannelDst>
struct channel_plus_assign_t : public std::binary_function<ChannelSrc,ChannelDst,ChannelDst> {
    typename channel_traits<ChannelDst>::reference
	operator()( typename channel_traits<ChannelSrc>::const_reference ch1,
                typename channel_traits<ChannelDst>::reference ch2 ) const {
        return ch2 += ChannelDst( ch1 );
    }
};

/// \ingroup ChannelNumericOperations
/// \brief ch2 -= ch1
/// structure for subtracting one channel from another
/// this is a generic implementation; user should specialize it for better performance
template <typename ChannelSrc,typename ChannelDst>
struct channel_minus_assign_t : public std::binary_function<ChannelSrc,ChannelDst,ChannelDst> {
    typename channel_traits<ChannelDst>::reference
	operator()( typename channel_traits<ChannelSrc>::const_reference ch1,
                typename channel_traits<ChannelDst>::reference ch2 ) const {
        return ch2 -= ChannelDst( ch1 );
    }
};

/// \ingroup ChannelNumericOperations
/// \brief ch2 *= ch1
/// structure for multiplying one channel to another
/// this is a generic implementation; user should specialize it for better performance
template <typename ChannelSrc,typename ChannelDst>
struct channel_multiplies_assign_t : public std::binary_function<ChannelSrc,ChannelDst,ChannelDst> {
    typename channel_traits<ChannelDst>::reference
	operator()( typename channel_traits<ChannelSrc>::const_reference ch1,
                typename channel_traits<ChannelDst>::reference ch2 ) const {
        return ch2 *= ch1;
    }
};

/// \ingroup ChannelNumericOperations
/// \brief ch2 /= ch1
/// structure for dividing channels
/// this is a generic implementation; user should specialize it for better performance
template <typename ChannelSrc,typename ChannelDst>
struct channel_divides_assign_t : public std::binary_function<ChannelSrc,ChannelDst,ChannelDst> {
    typename channel_traits<ChannelDst>::reference
	operator()( typename channel_traits<ChannelSrc>::const_reference ch1,
                typename channel_traits<ChannelDst>::reference ch2 ) const {
        return ch2 /= ch1;
    }
};


/// \ingroup ChannelNumericOperations
/// \brief ch += s
/// structure for adding a scalar to a channel
/// this is a generic implementation; user should specialize it for better performance
template <typename Scalar, typename ChannelDst>
struct channel_plus_scalar_assign_t : public std::binary_function<Scalar,ChannelDst,ChannelDst> {
    typename channel_traits<ChannelDst>::reference
	operator()( const Scalar& s,
	            typename channel_traits<ChannelDst>::reference ch ) const {
        return ch += ChannelDst(s);
    }
};

/// \ingroup ChannelNumericOperations
/// \brief ch -= s
/// structure for subtracting a scalar from a channel
/// this is a generic implementation; user should specialize it for better performance
template <typename Scalar, typename ChannelDst>
struct channel_minus_scalar_assign_t : public std::binary_function<Scalar,ChannelDst,ChannelDst> {
    typename channel_traits<ChannelDst>::reference
	operator()( const Scalar& s,
	            typename channel_traits<ChannelDst>::reference ch ) const {
        return ch -= ChannelDst(s);
    }
};

/// \ingroup ChannelNumericOperations
/// \brief ch *= s
/// structure for multiplying a scalar to one channel
/// this is a generic implementation; user should specialize it for better performance
template <typename Scalar, typename ChannelDst>
struct channel_multiplies_scalar_assign_t : public std::binary_function<Scalar,ChannelDst,ChannelDst> {
    typename channel_traits<ChannelDst>::reference
	operator()( const Scalar& s,
	            typename channel_traits<ChannelDst>::reference ch ) const {
        return ch *= s;
    }
};

/// \ingroup ChannelNumericOperations
/// \brief ch /= s
/// structure for dividing a channel by a scalar
/// this is a generic implementation; user should specialize it for better performance
template <typename Scalar, typename ChannelDst>
struct channel_divides_scalar_assign_t : public std::binary_function<Scalar,ChannelDst,ChannelDst> {
    typename channel_traits<ChannelDst>::reference
	operator()( const Scalar& s,
	            typename channel_traits<ChannelDst>::reference ch ) const {
        return ch /= s;
    }
};

} }  // namespace boost::gil

#endif
