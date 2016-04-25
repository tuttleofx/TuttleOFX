#include "NormalizeAlgorithm.hpp"
#include "NormalizeDefinitions.hpp"
#include "NormalizePlugin.hpp"
#include "NormalizeProcess.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>
#include <terry/numeric/operations.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/init.hpp>
#include <terry/numeric/scale.hpp>

#include <boost/gil/color_base_algorithm.hpp>

namespace tuttle
{
namespace plugin
{
namespace normalize
{

template <class View>
NormalizeProcess<View>::NormalizeProcess(NormalizePlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void NormalizeProcess<View>::setup(const OFX::RenderArguments& args)
{
    using namespace terry;
    using namespace terry::numeric;
    typedef rgba32f_pixel_t PixelParam;
    typedef pixel<Channel, gray_layout_t> PixelGray;

    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);

    View src = this->_srcView;
    Pixel smin;
    Pixel smax;
    pixel_zeros_t<Pixel>()(smin);
    pixel_zeros_t<Pixel>()(smax);

    rgba32f_pixel_t r;

    switch(_params._mode)
    {
        case eParamModeAnalyse:
        {
            analyseInputMinMax<View>(src, _params._analyseMode, smin, smax, *this);
            break;
        }
        case eParamModeCustom:
        {
            smin = _params._srcMinColor;
            smax = _params._srcMaxColor;
            break;
        }
    }
    for(std::ssize_t n = 0; n < boost::gil::num_channels<Pixel>::type::value; ++n)
    {
        _sMin[n] = smin[n];
        _dMin[n] = _params._dstMinColor[n];
        if((smax[n] - smin[n]) == 0)
            _ratio[n] = 0;
        else
        {
            typedef typename channel_type<PixelGray>::type tChannel;
            _ratio[n] = channel_traits<tChannel>::max_value() * (_params._dstMaxColor[n] - _params._dstMinColor[n]) /
                        (smax[n] - smin[n]);
            r[n] = 1.f * channel_traits<tChannel>::max_value() * (_params._dstMaxColor[n] - _params._dstMinColor[n]) /
                   (smax[n] - smin[n]);
        }
    }
}

template <class View>
template <class WorkView, typename LocalChannel, typename Pixel32f>
void NormalizeProcess<View>::processChannel(View& src, View& dst, bool process, Pixel32f& ratio, Pixel32f& sMin,
                                            Pixel32f& dMin)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;
    if(process)
    {
        typedef channel_view_type<LocalChannel, View> LocalView;
        typedef typename LocalView::type::value_type LocalPixel;
        typename LocalView::type localSrcView(LocalView::make(src));
        typename LocalView::type localDstView(LocalView::make(dst));

        transform_pixels(localSrcView, localDstView,
                         pixel_scale_t<LocalPixel, gray32f_pixel_t>(gray32f_pixel_t(get_color(ratio, LocalChannel())),
                                                                    gray32f_pixel_t(get_color(sMin, LocalChannel())),
                                                                    gray32f_pixel_t(get_color(dMin, LocalChannel()))));
    }
    else
    {
        copy_pixels(channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst));
    }
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void NormalizeProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;
    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    View src = subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    if(_params._processR && _params._processG && _params._processB && _params._processA)
    {
        transform_pixels_progress(src, dst, pixel_scale_t<Pixel, rgba32f_pixel_t>(_ratio, _sMin, _dMin), *this);
    }
    else
    {
        // by channel:
        // red
        {
            processChannel<View, red_t>(src, dst, _params._processR, _ratio, _sMin, _dMin);
        }
        // green
        {
            processChannel<View, green_t>(src, dst, _params._processG, _ratio, _sMin, _dMin);
        }
        // blue
        {
            processChannel<View, blue_t>(src, dst, _params._processB, _ratio, _sMin, _dMin);
        }
        // alpha
        {
            processChannel<View, alpha_t>(src, dst, _params._processA, _ratio, _sMin, _dMin);
        }
    }
}

template <>
void NormalizeProcess<boost::gil::rgb32f_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;
    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    rgb32f_view_t src =
        subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    rgb32f_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    rgb32f_pixel_t ratio;
    get_color(ratio, red_t()) = get_color(_ratio, red_t());
    get_color(ratio, green_t()) = get_color(_ratio, green_t());
    get_color(ratio, blue_t()) = get_color(_ratio, blue_t());

    rgb32f_pixel_t sMin;
    get_color(sMin, red_t()) = get_color(_sMin, red_t());
    get_color(sMin, green_t()) = get_color(_sMin, green_t());
    get_color(sMin, blue_t()) = get_color(_sMin, blue_t());

    rgb32f_pixel_t dMin;
    get_color(dMin, red_t()) = get_color(_dMin, red_t());
    get_color(dMin, green_t()) = get_color(_dMin, green_t());
    get_color(dMin, blue_t()) = get_color(_dMin, blue_t());

    if(_params._processR && _params._processG && _params._processB)
    {
        transform_pixels_progress(src, dst, pixel_scale_t<Pixel, Pixel>(ratio, sMin, dMin), *this);
    }
    else
    {
        // by channel:
        // red
        {
            processChannel<rgb32f_view_t, red_t>(src, dst, _params._processR, ratio, sMin, dMin);
        }
        // green
        {
            processChannel<rgb32f_view_t, green_t>(src, dst, _params._processG, ratio, sMin, dMin);
        }
        // blue
        {
            processChannel<rgb32f_view_t, blue_t>(src, dst, _params._processB, ratio, sMin, dMin);
        }
    }
}

template <>
void NormalizeProcess<boost::gil::rgb16_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;

    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    rgb16_view_t src = subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    rgb16_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    rgb32f_pixel_t ratio;
    get_color(ratio, red_t()) = get_color(_ratio, red_t());
    get_color(ratio, green_t()) = get_color(_ratio, green_t());
    get_color(ratio, blue_t()) = get_color(_ratio, blue_t());

    rgb32f_pixel_t sMin;
    get_color(sMin, red_t()) = get_color(_sMin, red_t());
    get_color(sMin, green_t()) = get_color(_sMin, green_t());
    get_color(sMin, blue_t()) = get_color(_sMin, blue_t());

    rgb32f_pixel_t dMin;
    get_color(dMin, red_t()) = get_color(_dMin, red_t());
    get_color(dMin, green_t()) = get_color(_dMin, green_t());
    get_color(dMin, blue_t()) = get_color(_dMin, blue_t());

    if(_params._processR && _params._processG && _params._processB)
    {
        transform_pixels_progress(src, dst, pixel_scale_t<Pixel, rgb32f_pixel_t>(ratio, sMin, dMin), *this);
    }
    else
    {
        // by channel:
        // red
        {
            processChannel<rgb16_view_t, red_t>(src, dst, _params._processR, ratio, sMin, dMin);
        }
        // green
        {
            processChannel<rgb16_view_t, green_t>(src, dst, _params._processG, ratio, sMin, dMin);
        }
        // blue
        {
            processChannel<rgb16_view_t, blue_t>(src, dst, _params._processB, ratio, sMin, dMin);
        }
    }
}

template <>
void NormalizeProcess<boost::gil::rgb8_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;
    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    rgb8_view_t src = subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    rgb8_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    rgb32f_pixel_t ratio;
    get_color(ratio, red_t()) = get_color(_ratio, red_t());
    get_color(ratio, green_t()) = get_color(_ratio, green_t());
    get_color(ratio, blue_t()) = get_color(_ratio, blue_t());

    rgb32f_pixel_t sMin;
    get_color(sMin, red_t()) = get_color(_sMin, red_t());
    get_color(sMin, green_t()) = get_color(_sMin, green_t());
    get_color(sMin, blue_t()) = get_color(_sMin, blue_t());

    rgb32f_pixel_t dMin;
    get_color(dMin, red_t()) = get_color(_dMin, red_t());
    get_color(dMin, green_t()) = get_color(_dMin, green_t());
    get_color(dMin, blue_t()) = get_color(_dMin, blue_t());

    if(_params._processR && _params._processG && _params._processB)
    {
        transform_pixels_progress(src, dst, pixel_scale_t<Pixel, rgb32f_pixel_t>(ratio, sMin, dMin), *this);
    }
    else
    {
        // by channel:
        // red
        {
            processChannel<rgb8_view_t, red_t>(src, dst, _params._processR, ratio, sMin, dMin);
        }
        // green
        {
            processChannel<rgb8_view_t, green_t>(src, dst, _params._processG, ratio, sMin, dMin);
        }
        // blue
        {
            processChannel<rgb8_view_t, blue_t>(src, dst, _params._processB, ratio, sMin, dMin);
        }
    }
}

template <>
void NormalizeProcess<boost::gil::gray32f_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;
    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    gray32f_view_t src =
        subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    gray32f_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    gray32f_pixel_t ratio;
    get_color(ratio, gray_color_t()) = get_color(_ratio, red_t());

    gray32f_pixel_t sMin;
    get_color(sMin, gray_color_t()) = get_color(_sMin, red_t());

    gray32f_pixel_t dMin;
    get_color(dMin, gray_color_t()) = get_color(_dMin, red_t());

    if(_params._processA)
    {
        transform_pixels_progress(src, dst, pixel_scale_t<Pixel, Pixel>(ratio, sMin, dMin), *this);
    }
}

template <>
void NormalizeProcess<boost::gil::gray16_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;

    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    gray16_view_t src =
        subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    gray16_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    gray32f_pixel_t ratio;
    get_color(ratio, gray_color_t()) = get_color(_ratio, red_t());

    gray32f_pixel_t sMin;
    get_color(sMin, gray_color_t()) = get_color(_sMin, red_t());

    gray32f_pixel_t dMin;
    get_color(dMin, gray_color_t()) = get_color(_dMin, red_t());

    if(_params._processA)
    {
        transform_pixels_progress(src, dst, pixel_scale_t<Pixel, gray32f_pixel_t>(ratio, sMin, dMin), *this);
    }
}

template <>
void NormalizeProcess<boost::gil::gray8_view_t>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;
    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    gray8_view_t src = subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    gray8_view_t dst =
        subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    gray32f_pixel_t ratio;
    get_color(ratio, gray_color_t()) = get_color(_ratio, red_t());

    gray32f_pixel_t sMin;
    get_color(sMin, gray_color_t()) = get_color(_sMin, red_t());

    gray32f_pixel_t dMin;
    get_color(dMin, gray_color_t()) = get_color(_dMin, red_t());

    if(_params._processA)
    {
        transform_pixels_progress(src, dst, pixel_scale_t<Pixel, gray32f_pixel_t>(ratio, sMin, dMin), *this);
    }
}
}
}
}
