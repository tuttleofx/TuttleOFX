#include "AVWriterProcess.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <AvTranscoder/codec/VideoCodec.hpp>

namespace tuttle
{
namespace plugin
{
namespace av
{
namespace writer
{

template <class View>
AVWriterProcess<View>::AVWriterProcess(AVWriterPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationFromTopToBottom)
    , _plugin(instance)
    , _params(_plugin.getProcessParams())
    , _videoStream(
          static_cast<avtranscoder::VideoGenerator&>(*_plugin._transcoder->getStreamTranscoder(0).getCurrentDecoder()))
    , _videoFrame(*_plugin._videoDesc)
{
    this->setNoMultiThreading();
}

template <class View>
void AVWriterProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void AVWriterProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    BOOST_ASSERT(procWindowRoW == this->_dstPixelRod);

    using namespace terry;

    rgb8_image_t img(this->_srcView.dimensions());
    rgb8_view_t vw(view(img));

    // Convert pixels in PIX_FMT_RGB24
    copy_and_convert_pixels(this->_srcView, vw);

    uint8_t* imageData = (uint8_t*)boost::gil::interleaved_view_get_raw_data(vw);

    // set video stream next frame
    _videoFrame.getData()[0] = imageData;
    _videoStream.setNextFrame(_videoFrame);

    // process
    _plugin._transcoder->processFrame();
}
}
}
}
}
