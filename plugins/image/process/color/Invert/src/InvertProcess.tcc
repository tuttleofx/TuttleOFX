#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>
#include <terry/algorithm/pixel_by_channel.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>
#include <terry/color/invert.hpp>
#include <terry/channel_view.hpp>

#include <boost/gil/algorithm.hpp>
#include <boost/gil/rgba.hpp>

namespace tuttle
{
namespace plugin
{
namespace invert
{

template <class View>
InvertProcess<View>::InvertProcess(InvertPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
}

template <class View>
void InvertProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);
}

template <class View>
void InvertProcess<View>::preProcess()
{
    const std::size_t nbChannels = boost::gil::num_channels<View>::value;
    const bool onePass = _params._red && _params._green && _params._blue;
    this->progressBegin(this->_dstView.size() * (onePass ? 1 : nbChannels));
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void InvertProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    TUTTLE_LOG_ERROR("undefined template");
}

template <>
void InvertProcess<terry::rgba32f_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::rgba32f_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::rgba32f_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertRGBAPixels<terry::rgba32f_view_t>(src, dst, _params._red, _params._green, _params._blue, _params._alpha);
}

template <>
void InvertProcess<terry::rgba32_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::rgba32_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::rgba32_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertRGBAPixels<terry::rgba32_view_t>(src, dst, _params._red, _params._green, _params._blue, _params._alpha);
}

template <>
void InvertProcess<terry::rgba16_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::rgba16_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::rgba16_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertRGBAPixels<terry::rgba16_view_t>(src, dst, _params._red, _params._green, _params._blue, _params._alpha);
}

template <>
void InvertProcess<terry::rgba8_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::rgba8_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::rgba8_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertRGBAPixels<terry::rgba8_view_t>(src, dst, _params._red, _params._green, _params._blue, _params._alpha);
}

template <>
void InvertProcess<terry::rgb32f_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::rgb32f_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::rgb32f_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertRGBPixels<terry::rgb32f_view_t>(src, dst, _params._red, _params._green, _params._blue);
}

template <>
void InvertProcess<terry::rgb32_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::rgb32_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::rgb32_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertRGBPixels<terry::rgb32_view_t>(src, dst, _params._red, _params._green, _params._blue);
}

template <>
void InvertProcess<terry::rgb16_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::rgb16_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::rgb16_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertRGBPixels<terry::rgb16_view_t>(src, dst, _params._red, _params._green, _params._blue);
}

template <>
void InvertProcess<terry::rgb8_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::rgb8_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::rgb8_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertRGBPixels<terry::rgb8_view_t>(src, dst, _params._red, _params._green, _params._blue);
}

template <>
void InvertProcess<terry::gray32f_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::gray32f_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::gray32f_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertGrayPixels<terry::gray32f_view_t>(src, dst, _params._gray);
}

template <>
void InvertProcess<terry::gray32_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::gray32_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::gray32_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertGrayPixels<terry::gray32_view_t>(src, dst, _params._gray);
}

template <>
void InvertProcess<terry::gray16_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::gray16_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::gray16_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertGrayPixels<terry::gray16_view_t>(src, dst, _params._gray);
}

template <>
void InvertProcess<terry::gray8_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    terry::gray8_view_t src =
        subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    terry::gray8_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    invertGrayPixels<terry::gray8_view_t>(src, dst, _params._gray);
}

template <class View>
template <typename WorkView>
void InvertProcess<View>::invertRGBAPixels(WorkView& src, WorkView& dst, bool red, bool green, bool blue, bool alpha)
{
    using namespace boost::gil;
    using namespace terry;
    using namespace terry::algorithm;
    using namespace terry::color;

    if(red && green && blue)
    {
        // If all channel colors are inverted, which is the most used case
        // do it in one pass
        if(alpha)
        {
            transform_pixels_progress(src, dst, transform_pixel_by_channel_t<terry::color::channel_invert_t>(), *this);
        }
        else
        {
            transform_pixels_progress(src, dst, pixel_invert_colors_t(), *this);
        }
    }
    else
    {
        {
            typedef red_t LocalChannel;
            if(red)
            {
                transform_pixels_progress(channel_view<LocalChannel, WorkView>(src),
                                          channel_view<LocalChannel, WorkView>(dst),
                                          transform_pixel_by_channel_t<channel_invert_t>(), *this);
            }
            else
            {
                copy_pixels(channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst));
                this->progressForward(dst.size());
            }
        }
        {
            typedef green_t LocalChannel;
            if(green)
            {
                transform_pixels_progress(channel_view<LocalChannel, WorkView>(src),
                                          channel_view<LocalChannel, WorkView>(dst),
                                          transform_pixel_by_channel_t<channel_invert_t>(), *this);
            }
            else
            {
                copy_pixels(channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst));
                this->progressForward(dst.size());
            }
        }
        {
            typedef blue_t LocalChannel;
            if(blue)
            {
                transform_pixels_progress(channel_view<LocalChannel, WorkView>(src),
                                          channel_view<LocalChannel, WorkView>(dst),
                                          transform_pixel_by_channel_t<channel_invert_t>(), *this);
            }
            else
            {
                copy_pixels(channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst));
                this->progressForward(dst.size());
            }
        }
        {
            typedef alpha_t LocalChannel;
            if(alpha)
            {
                transform_pixels_progress(channel_view<LocalChannel, WorkView>(src),
                                          channel_view<LocalChannel, WorkView>(dst),
                                          transform_pixel_by_channel_t<channel_invert_t>(), *this);
            }
            else
            {
                copy_pixels(channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst));
                this->progressForward(dst.size());
            }
        }
    }
}

template <class View>
template <typename WorkView>
void InvertProcess<View>::invertRGBPixels(WorkView& src, WorkView& dst, bool red, bool green, bool blue)
{
    using namespace boost::gil;
    using namespace terry;
    using namespace terry::algorithm;
    using namespace terry::color;

    if(red && green && blue)
    {
        transform_pixels_progress(src, dst, transform_pixel_by_channel_t<terry::color::channel_invert_t>(), *this);
    }
    else
    {
        {
            typedef red_t LocalChannel;
            if(red)
            {
                transform_pixels_progress(channel_view<LocalChannel, WorkView>(src),
                                          channel_view<LocalChannel, WorkView>(dst),
                                          transform_pixel_by_channel_t<channel_invert_t>(), *this);
            }
            else
            {
                copy_pixels(channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst));
                this->progressForward(dst.size());
            }
        }
        {
            typedef green_t LocalChannel;
            if(green)
            {
                transform_pixels_progress(channel_view<LocalChannel, WorkView>(src),
                                          channel_view<LocalChannel, WorkView>(dst),
                                          transform_pixel_by_channel_t<channel_invert_t>(), *this);
            }
            else
            {
                copy_pixels(channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst));
                this->progressForward(dst.size());
            }
        }
        {
            typedef blue_t LocalChannel;
            if(blue)
            {
                transform_pixels_progress(channel_view<LocalChannel, WorkView>(src),
                                          channel_view<LocalChannel, WorkView>(dst),
                                          transform_pixel_by_channel_t<channel_invert_t>(), *this);
            }
            else
            {
                copy_pixels(channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst));
                this->progressForward(dst.size());
            }
        }
    }
}

template <class View>
template <typename WorkView>
void InvertProcess<View>::invertGrayPixels(WorkView& src, WorkView& dst, bool gray)
{
    using namespace boost::gil;
    using namespace terry;
    using namespace terry::algorithm;

    if(gray)
    {
        transform_pixels_progress(src, dst, transform_pixel_by_channel_t<terry::color::channel_invert_t>(), *this);
    }
    else
    {
        copy_pixels(src, dst);
        this->progressForward(dst.size());
    }
}
}
}
}
