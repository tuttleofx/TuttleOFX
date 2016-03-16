#ifndef _TUTTLE_PLUGIN_NORMALIZE_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_NORMALIZE_ALGORITHM_HPP_

#include <terry/channel.hpp>
#include <terry/channel_view.hpp>
#include <terry/algorithm/static_channel_recursion.hpp>
#include <terry/numeric/operations.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/minmax.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>

namespace tuttle
{
namespace plugin
{
namespace normalize
{

template <class View, typename LocalChannel>
void analyseChannel(View& src, typename View::value_type& min, typename View::value_type& max, IProgress& p)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;

    typedef channel_view_type<LocalChannel, View> LocalView;
    typename LocalView::type localView(LocalView::make(src));
    pixel_minmax_by_channel_t<typename LocalView::type::value_type> minmax(localView(0, 0));
    transform_pixels_progress(localView, minmax, p);
    static_fill(min, minmax.min[0]);
    static_fill(max, minmax.max[0]);
}

/**
 * @brief compute min and max from input view analyse.
 *
 * @param[in] src: input image to analyse
 * @param[in] analyseMode: choose the analyse method
 * @param[out] min: output min values
 * @param[out] max: output max values
 */
template <class View>
void analyseInputMinMax(const View& src, const EParamAnalyseMode analyseMode, typename View::value_type& min,
                        typename View::value_type& max, IProgress& p)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;

    typedef typename View::value_type Pixel;

    switch(analyseMode)
    {
        case eParamAnalyseModePerChannel:
        {
            pixel_minmax_by_channel_t<Pixel> minmax(src(0, 0));
            // compute the maximum value
            transform_pixels_progress(src, minmax, p);
            min = minmax.min;
            max = minmax.max;
            break;
        }
        case eParamAnalyseModeLuminosity:
        {
            typedef pixel<typename channel_type<View>::type, gray_layout_t> PixelGray;
            typedef typename color_converted_view_type<View, PixelGray>::type LocalView;
            LocalView localView(src);
            pixel_minmax_by_channel_t<typename LocalView::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeR:
        {
            typedef channel_view_type<red_t, View> LocalView;
            typename LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<typename LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeG:
        {
            typedef channel_view_type<green_t, View> LocalView;
            typename LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<typename LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeB:
        {
            typedef channel_view_type<blue_t, View> LocalView;
            typename LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<typename LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeA:
        {
            typedef channel_view_type<alpha_t, View> LocalView;
            typename LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<typename LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
    }
}

template <>
void analyseInputMinMax(const boost::gil::rgb32f_view_t& src, const EParamAnalyseMode analyseMode,
                        boost::gil::rgb32f_view_t::value_type& min, boost::gil::rgb32f_view_t::value_type& max,
                        IProgress& progress)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;

    typedef rgb32f_view_t::value_type Pixel;

    switch(analyseMode)
    {
        case eParamAnalyseModePerChannel:
        {
            pixel_minmax_by_channel_t<Pixel> minmax(src(0, 0));
            // compute the maximum value
            transform_pixels_progress(src, minmax, progress);
            min = minmax.min;
            max = minmax.max;
            break;
        }
        case eParamAnalyseModeLuminosity:
        {
            typedef pixel<channel_type<rgb32f_view_t>::type, gray_layout_t> PixelGray;
            typedef color_converted_view_type<rgb32f_view_t, PixelGray>::type LocalView;
            LocalView localView(src);
            pixel_minmax_by_channel_t<LocalView::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, progress);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeR:
        {
            typedef channel_view_type<red_t, rgb32f_view_t> LocalView;
            LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, progress);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeG:
        {
            typedef channel_view_type<green_t, rgb32f_view_t> LocalView;
            LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, progress);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeB:
        {
            typedef channel_view_type<blue_t, rgb32f_view_t> LocalView;
            LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, progress);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeA:
        {
            static_fill(min, 0);
            static_fill(max, 1);
            break;
        }
    }
}

template <>
void analyseInputMinMax(const boost::gil::rgb16_view_t& src, const EParamAnalyseMode analyseMode,
                        boost::gil::rgb16_view_t::value_type& min, boost::gil::rgb16_view_t::value_type& max,
                        IProgress& progress)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;

    typedef rgb16_view_t::value_type Pixel;

    switch(analyseMode)
    {
        case eParamAnalyseModePerChannel:
        {
            pixel_minmax_by_channel_t<Pixel> minmax(src(0, 0));
            // compute the maximum value
            transform_pixels_progress(src, minmax, progress);
            min = minmax.min;
            max = minmax.max;
            break;
        }
        case eParamAnalyseModeLuminosity:
        {
            typedef pixel<channel_type<rgb16_view_t>::type, gray_layout_t> PixelGray;
            typedef color_converted_view_type<rgb16_view_t, PixelGray>::type LocalView;
            LocalView localView(src);
            pixel_minmax_by_channel_t<LocalView::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, progress);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeR:
        {
            typedef channel_view_type<red_t, rgb16_view_t> LocalView;
            LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, progress);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeG:
        {
            typedef channel_view_type<green_t, rgb16_view_t> LocalView;
            LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, progress);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeB:
        {
            typedef channel_view_type<blue_t, rgb16_view_t> LocalView;
            LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, progress);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeA:
        {
            static_fill(min, 0);
            static_fill(max, 1);
            break;
        }
    }
}

template <>
void analyseInputMinMax(const boost::gil::rgb8_view_t& src, const EParamAnalyseMode analyseMode,
                        boost::gil::rgb8_view_t::value_type& min, boost::gil::rgb8_view_t::value_type& max, IProgress& p)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;

    typedef rgb8_view_t::value_type Pixel;

    switch(analyseMode)
    {
        case eParamAnalyseModePerChannel:
        {
            pixel_minmax_by_channel_t<Pixel> minmax(src(0, 0));
            // compute the maximum value
            transform_pixels_progress(src, minmax, p);
            min = minmax.min;
            max = minmax.max;
            break;
        }
        case eParamAnalyseModeLuminosity:
        {
            typedef pixel<channel_type<rgb8_view_t>::type, gray_layout_t> PixelGray;
            typedef color_converted_view_type<rgb8_view_t, PixelGray>::type LocalView;
            LocalView localView(src);
            pixel_minmax_by_channel_t<LocalView::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeR:
        {
            typedef channel_view_type<red_t, rgb8_view_t> LocalView;
            LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeG:
        {
            typedef channel_view_type<green_t, rgb8_view_t> LocalView;
            LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeB:
        {
            typedef channel_view_type<blue_t, rgb8_view_t> LocalView;
            LocalView::type localView(LocalView::make(src));
            pixel_minmax_by_channel_t<LocalView::type::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeA:
        {
            static_fill(min, 0);
            static_fill(max, 1);
            break;
        }
    }
}

template <>
void analyseInputMinMax(const boost::gil::gray32f_view_t& src, const EParamAnalyseMode analyseMode,
                        boost::gil::gray32f_view_t::value_type& min, boost::gil::gray32f_view_t::value_type& max,
                        IProgress& p)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;

    typedef gray32f_view_t::value_type Pixel;

    switch(analyseMode)
    {
        case eParamAnalyseModePerChannel:
        {
            pixel_minmax_by_channel_t<Pixel> minmax(src(0, 0));
            // compute the maximum value
            transform_pixels_progress(src, minmax, p);
            min = minmax.min;
            max = minmax.max;
            break;
        }
        case eParamAnalyseModeLuminosity:
        {
            typedef pixel<channel_type<gray32f_view_t>::type, gray_layout_t> PixelGray;
            typedef color_converted_view_type<gray32f_view_t, PixelGray>::type LocalView;
            LocalView localView(src);
            pixel_minmax_by_channel_t<LocalView::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeR:
        case eParamAnalyseModeG:
        case eParamAnalyseModeB:
        case eParamAnalyseModeA:
        {
            static_fill(min, 0);
            static_fill(max, 1);
            break;
        }
    }
}

template <>
void analyseInputMinMax(const boost::gil::gray16_view_t& src, const EParamAnalyseMode analyseMode,
                        boost::gil::gray16_view_t::value_type& min, boost::gil::gray16_view_t::value_type& max, IProgress& p)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;

    typedef gray16_view_t::value_type Pixel;

    switch(analyseMode)
    {
        case eParamAnalyseModePerChannel:
        {
            pixel_minmax_by_channel_t<Pixel> minmax(src(0, 0));
            // compute the maximum value
            transform_pixels_progress(src, minmax, p);
            min = minmax.min;
            max = minmax.max;
            break;
        }
        case eParamAnalyseModeLuminosity:
        {
            typedef pixel<channel_type<gray16_view_t>::type, gray_layout_t> PixelGray;
            typedef color_converted_view_type<gray16_view_t, PixelGray>::type LocalView;
            LocalView localView(src);
            pixel_minmax_by_channel_t<LocalView::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeR:
        case eParamAnalyseModeG:
        case eParamAnalyseModeB:
        case eParamAnalyseModeA:
        {
            static_fill(min, 0);
            static_fill(max, 1);
            break;
        }
    }
}

template <>
void analyseInputMinMax(const boost::gil::gray8_view_t& src, const EParamAnalyseMode analyseMode,
                        boost::gil::gray8_view_t::value_type& min, boost::gil::gray8_view_t::value_type& max, IProgress& p)
{
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::algorithm;

    typedef gray8_view_t::value_type Pixel;

    switch(analyseMode)
    {
        case eParamAnalyseModePerChannel:
        {
            pixel_minmax_by_channel_t<Pixel> minmax(src(0, 0));
            // compute the maximum value
            transform_pixels_progress(src, minmax, p);
            min = minmax.min;
            max = minmax.max;
            break;
        }
        case eParamAnalyseModeLuminosity:
        {
            typedef pixel<channel_type<gray8_view_t>::type, gray_layout_t> PixelGray;
            typedef color_converted_view_type<gray8_view_t, PixelGray>::type LocalView;
            LocalView localView(src);
            pixel_minmax_by_channel_t<LocalView::value_type> minmax(localView(0, 0));
            transform_pixels_progress(localView, minmax, p);
            static_fill(min, minmax.min[0]);
            static_fill(max, minmax.max[0]);
            break;
        }
        case eParamAnalyseModeR:
        case eParamAnalyseModeG:
        case eParamAnalyseModeB:
        case eParamAnalyseModeA:
        {
            static_fill(min, 0);
            static_fill(max, 1);
            break;
        }
    }
}
}
}
}

#endif
