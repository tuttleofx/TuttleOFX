
namespace tuttle
{
namespace plugin
{
namespace channelShuffle
{

template <class View>
ChannelShuffleProcess<View>::ChannelShuffleProcess(ChannelShufflePlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void ChannelShuffleProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ChannelShuffleProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
    {
        typename View::x_iterator src_it = this->_srcView.x_at(procWindowOutput.x1, y);
        typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
        for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++src_it, ++dst_it)
        {
            for(unsigned int c = 0; c < boost::gil::size<Pixel>::value; ++c)
            {
                (*dst_it)[c] = (*src_it)[_params.mapping[c]];
            }
        }
        if(this->progressForward(procWindowSize.x))
            return;
    }
}
}
}
}
