#ifndef NUMERIC_ADDS_HPP
#define NUMERIC_ADDS_HPP

#include <functional>
#include <boost/gil/gil_config.hpp>
#include <boost/gil/channel.hpp>

namespace boost { namespace gil {

/// \ingroup ChannelNumericOperations
/// \brief structure for getting the absolute value of a channel
template <typename Channel1,typename ChannelR>
struct channel_abs_t : public std::unary_function<Channel1,ChannelR> {
    ChannelR operator()(typename channel_traits<Channel1>::const_reference ch1) const {
        return ChannelR(std::abs(ch1));
    }
};


/// \ingroup ChannelNumericOperations
/// \brief structure for dividing channels
template <typename Channel1,typename Channel2,typename ChannelR>
struct channel_div_clamp_t : public std::binary_function<Channel1,Channel2,ChannelR> {
    ChannelR operator()(typename channel_traits<Channel1>::const_reference ch1,
                        typename channel_traits<Channel2>::const_reference ch2) const {
        if ( ch1 <= Channel1(0) || ch2 <= Channel2(0) )
            return ChannelR(0);
        else
            return ChannelR(ch1) / ChannelR(ch2);
    }
};

}
}

#endif //NUMERIC_ADDS_HPP
