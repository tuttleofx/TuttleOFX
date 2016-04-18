#ifndef _TUTTLE_PLUGIN_CHANNELSHUFFLE_PROCESS_HPP_
#define _TUTTLE_PLUGIN_CHANNELSHUFFLE_PROCESS_HPP_

#include <tuttle/plugin/ImageGilFilterProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace channelShuffle
{

/**
 * @brief ChannelShuffle process
 *
 */
template <class View>
class ChannelShuffleProcess : public ImageGilFilterProcessor<View>
{
public:
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::channel_type<View>::type Channel;
    typedef float Scalar;

protected:
    ChannelShufflePlugin& _plugin;       ///< Rendering plugin
    ChannelShuffleProcessParams _params; ///< parameters

public:
    ChannelShuffleProcess(ChannelShufflePlugin& effect);

    void setup(const OFX::RenderArguments& args);

    void multiThreadProcessImages(const OfxRectI& procWindowRoW);
};
}
}
}

#include "ChannelShuffleProcess.tcc"

#endif
