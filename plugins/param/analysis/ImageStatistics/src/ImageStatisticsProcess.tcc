#include "ImageStatisticsPlugin.hpp"

#include <tuttle/plugin/global.hpp>
#include <terry/globals.hpp>
#include <tuttle/plugin/param/gilColor.hpp>
#include <terry/typedefs.hpp>
#include <terry/channel_view.hpp>

#include <terry/numeric/operations.hpp>
#include <terry/numeric/operations_assign.hpp>
#include <terry/numeric/assign.hpp>
#include <terry/numeric/assign_minmax.hpp>
#include <terry/numeric/minmax.hpp>
#include <terry/numeric/init.hpp>
#include <terry/numeric/pow.hpp>
#include <terry/numeric/sqrt.hpp>
#include <boost/gil/extension/color/hsl.hpp>

#include <boost/units/pow.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/erase.hpp>
#include <boost/mpl/find.hpp>

/*
namespace boost {
namespace gil {

namespace details {

template <typename Colorspace, typename ChannelType>
// models pixel concept
struct without_alpha_channel_impl_t
{
        typedef pixel<ChannelType, layout<Colorspace> > Pixel;
        typedef Pixel PixelR;

        BOOST_STATIC_ASSERT( ( !contains_color<Pixel, alpha_t>::value ) );

        PixelR operator ()( const Pixel& src ) const
        {
                return src;
        }

};

template <typename ChannelType>
// models pixel concept
struct without_alpha_channel_impl_t<rgba_t, ChannelType>
{
        typedef rgba_t Colorspace;
        typedef pixel<ChannelType, layout<Colorspace> > Pixel;
        typedef pixel<ChannelType, layout<rgb_t> > PixelR;

        BOOST_STATIC_ASSERT( ( contains_color<Pixel, alpha_t>::value ) );

        PixelR operator ()( const Pixel& src ) const
        {
                PixelR dst;

                get_color( dst, red_t() )   = get_color( src, red_t() );
                get_color( dst, green_t() ) = get_color( src, green_t() );
                get_color( dst, blue_t() )  = get_color( src, blue_t() );
                return dst;
        }

};

}

template <typename Pixel>
// models pixel concept
struct without_alpha_channel_t
{
        typedef typename channel_type<Pixel>::type ChannelType;
        typedef typename color_space_type<Pixel>::type Colorspace;

        typedef details::without_alpha_channel_impl_t<Colorspace, ChannelType> Do;
        typedef typename Do::PixelR PixelR;

        PixelR operator ()( const Pixel& src ) const
        {
                return Do() ( src );
        }

};

GIL_DEFINE_ALL_TYPEDEFS( 64, hsl )
GIL_DEFINE_ALL_TYPEDEFS( 64s, hsl )
GIL_DEFINE_ALL_TYPEDEFS( 64f, hsl )

}
}
*/

namespace tuttle
{
namespace plugin
{
namespace imageStatistics
{

template <typename T>
T standard_deviation(const T v_sum, const T v_sum_p2, const std::size_t nb)
{
    using namespace boost::units;
    return std::sqrt(v_sum_p2 / nb - pow<2>(v_sum / nb));
}

template <typename Pixel>
Pixel pixel_standard_deviation(const Pixel& v_sum, const Pixel& v_sum_p2, const std::size_t nb)
{
    using namespace boost::gil;
    Pixel res;
    for(int i = 0; i < num_channels<Pixel>::type::value; ++i)
    {
        res[i] = standard_deviation(v_sum[i], v_sum_p2[i], nb);
    }
    return res;
}

/**
 * @brief In probability theory and statistics, kurtosis is a measure of the
 * "peakedness" of the probability distribution of a real-valued random variable.
 *
 * Higher kurtosis means more of the variance is the result of infrequent extreme deviations,
 * as opposed to frequent modestly sized deviations.
 */
template <typename T>
T kurtosis(const T v_mean, const T v_standard_deviation, const T v_sum, const T v_sum_p2, const T v_sum_p3, const T v_sum_p4,
           const std::size_t nb)
{
    using namespace boost::units;
    return ((v_sum_p4 - 4.0 * v_mean * v_sum_p3 + 6.0 * pow<2>(v_mean) * v_sum_p2) / nb - 3.0 * pow<4>(v_mean)) /
               (pow<4>(v_standard_deviation)) -
           3.0;
}

template <typename Pixel>
Pixel pixel_kurtosis(const Pixel& v_mean, const Pixel& v_standard_deviation, const Pixel& v_sum, const Pixel& v_sum_p2,
                     const Pixel& v_sum_p3, const Pixel& v_sum_p4, const std::size_t nb)
{
    using namespace boost::gil;
    Pixel res;
    for(int i = 0; i < num_channels<Pixel>::type::value; ++i)
    {
        res[i] = kurtosis(v_mean[i], v_standard_deviation[i], v_sum[i], v_sum_p2[i], v_sum_p3[i], v_sum_p4[i], nb);
    }
    return res;
}

/**
 * @brief In probability theory and statistics, skewness is a measure of the
 * asymmetry of the probability distribution of a real-valued random variable.
 *
 * A zero value indicates that the values are relatively evenly distributed
 * on both sides of the mean, typically but not necessarily implying
 * a symmetric distribution.
 */
template <typename T>
T skewness(const T v_mean, const T v_standard_deviation, const T v_sum, const T v_sum_p2, const T v_sum_p3,
           const std::size_t nb)
{
    using namespace boost::units;
    return ((v_sum_p3 - 3.0 * v_mean * v_sum_p2) / nb + 2.0 * pow<3>(v_mean)) / pow<3>(v_standard_deviation);
}

template <typename Pixel>
Pixel pixel_skewness(const Pixel& v_mean, const Pixel& v_standard_deviation, const Pixel& v_sum, const Pixel& v_sum_p2,
                     const Pixel& v_sum_p3, const std::size_t nb)
{
    using namespace boost::gil;
    Pixel res;
    for(int i = 0; i < num_channels<Pixel>::type::value; ++i)
    {
        res[i] = skewness(v_mean[i], v_standard_deviation[i], v_sum[i], v_sum_p2[i], v_sum_p3[i], nb);
    }
    return res;
}

template <class Pixel>
struct OutputParams
{
    OutputParams()
    {
        using namespace terry::numeric;
        pixel_zeros_t<Pixel>()(_average);
        pixel_zeros_t<Pixel>()(_variance);
        pixel_zeros_t<Pixel>()(_channelMin);
        pixel_zeros_t<Pixel>()(_channelMax);
        pixel_zeros_t<Pixel>()(_luminosityMin);
        pixel_zeros_t<Pixel>()(_luminosityMax);
        pixel_zeros_t<Pixel>()(_kurtosis);
        pixel_zeros_t<Pixel>()(_skewness);
    }

    Pixel _average;
    Pixel _variance;
    Pixel _channelMin;
    Pixel _channelMax;
    Pixel _luminosityMin;
    Pixel _luminosityMax;
    Pixel _kurtosis;
    Pixel _skewness;

    std::size_t _nbPixels;
};

template <class View, class MaskView, typename CType = boost::gil::bits64f>
struct ComputeOutputParams
{
    typedef typename View::value_type Pixel;
    typedef typename boost::gil::color_space_type<View>::type Colorspace;
    typedef boost::gil::pixel<typename boost::gil::channel_type<View>::type, boost::gil::layout<boost::gil::gray_t> >
        PixelGray; // grayscale pixel type (using the input channel_type)
    typedef boost::gil::pixel<CType, boost::gil::layout<Colorspace> >
        CPixel; // the pixel type use for computation (using input colorspace)

    typedef OutputParams<CPixel> Output;

    static Output run(const View& image, const MaskView& maskView, const bool useMask, ImageStatisticsPlugin& plugin)
    {
        using namespace terry::numeric;
        OutputParams<CPixel> output;

        std::size_t nbProcPixels = 0;

        Pixel channelMin;
        Pixel channelMax;
        Pixel luminosityMin;
        PixelGray luminosityMinGray;
        Pixel luminosityMax;
        PixelGray luminosityMaxGray;

        CPixel sum;
        CPixel varianceSum;
        CPixel sum_p2;
        CPixel sum_p3;
        CPixel sum_p4;
        pixel_zeros_t<CPixel>()(sum);
        pixel_zeros_t<CPixel>()(varianceSum);
        pixel_zeros_t<CPixel>()(sum_p2);
        pixel_zeros_t<CPixel>()(sum_p3);
        pixel_zeros_t<CPixel>()(sum_p4);

        for(int y = 0; y < image.height(); ++y)
        {
            typename View::x_iterator src_it = image.x_at(0, y);
            typename MaskView::x_iterator mask_it = maskView.x_at(0, y);

            for(int x = 0; x < image.width(); ++x, ++src_it, ++mask_it)
            {
                if(useMask && get_color(*mask_it, gray_color_t()) == 0.0)
                    continue;

                PixelGray grayCurrentPixel; // current pixel in gray colorspace
                color_convert(*src_it, grayCurrentPixel);

                if(nbProcPixels == 0)
                {
                    // It's the first pixel we visit.
                    // So initialize statistics!
                    channelMin = *src_it;
                    channelMax = *src_it;
                    luminosityMin = *src_it;
                    luminosityMinGray = grayCurrentPixel;
                    luminosityMax = *src_it;
                    luminosityMaxGray = grayCurrentPixel;
                }
                // Count the number of pixels taken into account
                ++nbProcPixels;

                CPixel pix;
                pixel_assigns_t<Pixel, CPixel>()(*src_it, pix); // pix = src_it;

                CPixel pix_p2;
                CPixel pix_p3;
                CPixel pix_p4;

                pixel_assigns_t<CPixel, CPixel>()(pixel_pow_t<CPixel, 2>()(pix), pix_p2); // pix_p2 = pow<2>( pix );
                pixel_assigns_t<CPixel, CPixel>()(pixel_multiplies_t<CPixel, CPixel, CPixel>()(pix, pix_p2),
                                                  pix_p3); // pix_p3 = pix * pix_p2;
                pixel_assigns_t<CPixel, CPixel>()(pixel_multiplies_t<CPixel, CPixel, CPixel>()(pix_p2, pix_p2),
                                                  pix_p4); // pix_p4 = pix_p2 * pix_p2;

                pixel_plus_assign_t<CPixel, CPixel>()(pix, sum);       // sum += pix;
                pixel_plus_assign_t<CPixel, CPixel>()(pix_p2, sum_p2); // sum_p2 += pix_p2;
                pixel_plus_assign_t<CPixel, CPixel>()(pix_p3, sum_p3); // sum_p3 += pix_p3;
                pixel_plus_assign_t<CPixel, CPixel>()(pix_p4, sum_p4); // sum_p4 += pix_p4;

                // search min for each channel
                pixel_assign_min_t<Pixel, Pixel>()(*src_it, channelMin);
                // search max for each channel
                pixel_assign_max_t<Pixel, Pixel>()(*src_it, channelMax);

                // search min luminosity
                if(get_color(grayCurrentPixel, gray_color_t()) < get_color(luminosityMinGray, gray_color_t()))
                {
                    luminosityMin = *src_it;
                    luminosityMinGray = grayCurrentPixel;
                }
                // search max luminosity
                if(get_color(grayCurrentPixel, gray_color_t()) > get_color(luminosityMaxGray, gray_color_t()))
                {
                    luminosityMax = *src_it;
                    luminosityMaxGray = grayCurrentPixel;
                }
            }
        }

        output._nbPixels = nbProcPixels;

        output._channelMin = channelMin;
        output._channelMax = channelMax;
        output._luminosityMin = luminosityMin;
        output._luminosityMax = luminosityMax;

        CPixel stdDeriv = pixel_standard_deviation(sum, sum_p2, nbProcPixels);
        output._average = pixel_divides_scalar_t<CPixel, double>()(sum, nbProcPixels);

        for(int y = 0; y < image.height(); ++y)
        {
            typename View::x_iterator src_it = image.x_at(0, y);

            for(int x = 0; x < image.width(); ++x, ++src_it)
            {
                CPixel pix;
                pixel_assigns_t<Pixel, CPixel>()(*src_it, pix); // pix = src_it;

                CPixel pix_diff = pix;
                CPixel pix_diff2;

                pix_diff = pixel_minus_t<CPixel, CPixel, CPixel>()(pix, output._average); // pix_diff = (pix - mean)
                pix_diff2 =
                    pixel_multiplies_t<CPixel, CPixel, CPixel>()(pix_diff, pix_diff); // pix_diff2 = (x - mean)*(x - mean)

                pixel_plus_assign_t<CPixel, CPixel>()(pix_diff2, varianceSum); // varianceSum += pix_diff2;
            }
        }

        CPixel varianceSquare = pixel_divides_scalar_t<CPixel, double>()(varianceSum, nbProcPixels);
        output._variance = pixel_sqrt_t<CPixel, Pixel>()(varianceSquare);
        output._kurtosis = pixel_kurtosis(output._average, stdDeriv, sum, sum_p2, sum_p3, sum_p4, nbProcPixels);
        output._skewness = pixel_skewness(output._average, stdDeriv, sum, sum_p2, sum_p3, nbProcPixels);

        return output;
    }
};

template <typename OutputParamsRGBA, typename OutputParamsHSL>
void setOutputParams(const OutputParamsRGBA& outputParamsRGBA, const OutputParamsHSL& outputParamsHSL, const OfxTime time,
                     ImageStatisticsPlugin& plugin)
{
    BOOST_ASSERT(outputParamsRGBA._nbPixels == outputParamsHSL._nbPixels);

    plugin._paramOutputNbPixels->setValueAtTime(time, outputParamsRGBA._nbPixels);

    setRGBAParamValuesAtTime(plugin._paramOutputAverage, time, outputParamsRGBA._average);
    setRGBAParamValuesAtTime(plugin._paramOutputVariance, time, outputParamsRGBA._variance);
    setRGBAParamValuesAtTime(plugin._paramOutputChannelMin, time, outputParamsRGBA._channelMin);
    setRGBAParamValuesAtTime(plugin._paramOutputChannelMax, time, outputParamsRGBA._channelMax);
    setRGBAParamValuesAtTime(plugin._paramOutputLuminosityMin, time, outputParamsRGBA._luminosityMin);
    setRGBAParamValuesAtTime(plugin._paramOutputLuminosityMax, time, outputParamsRGBA._luminosityMax);
    setRGBAParamValuesAtTime(plugin._paramOutputKurtosis, time, outputParamsRGBA._kurtosis);
    setRGBAParamValuesAtTime(plugin._paramOutputSkewness, time, outputParamsRGBA._skewness);

    set012ParamValuesAtTime(plugin._paramOutputAverageHSL, time, outputParamsHSL._average);
    set012ParamValuesAtTime(plugin._paramOutputVarianceHSL, time, outputParamsHSL._variance);
    set012ParamValuesAtTime(plugin._paramOutputChannelMinHSL, time, outputParamsHSL._channelMin);
    set012ParamValuesAtTime(plugin._paramOutputChannelMaxHSL, time, outputParamsHSL._channelMax);
    set012ParamValuesAtTime(plugin._paramOutputLuminosityMinHSL, time, outputParamsHSL._luminosityMin);
    set012ParamValuesAtTime(plugin._paramOutputLuminosityMaxHSL, time, outputParamsHSL._luminosityMax);
    set012ParamValuesAtTime(plugin._paramOutputKurtosisHSL, time, outputParamsHSL._kurtosis);
    set012ParamValuesAtTime(plugin._paramOutputSkewnessHSL, time, outputParamsHSL._skewness);
}

template <class View>
ImageStatisticsProcess<View>::ImageStatisticsProcess(ImageStatisticsPlugin& instance)
    : ImageGilFilterProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
    this->setNoMultiThreading();

    _clipMask = instance.fetchClip(kClipMask);
    _clipMaskConnected = _clipMask->isConnected();

    if(_clipMaskConnected && (_clipMask->getPixelComponents() != this->_clipSrc->getPixelComponents()))
        BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user("Can't mix different input Pixel Components."));
}

template <class View>
void ImageStatisticsProcess<View>::setup(const OFX::RenderArguments& args)
{
    using namespace boost::gil;
    using namespace terry;

    ImageGilFilterProcessor<View>::setup(args);

    // mask view
    if(_clipMaskConnected)
    {
        this->_mask.reset(_clipMask->fetchImage(args.time));
        if(!this->_mask.get())
            BOOST_THROW_EXCEPTION(exception::ImageNotReady());
        if(this->_mask->getRowDistanceBytes() <= 0)
            BOOST_THROW_EXCEPTION(exception::WrongRowBytes());
        this->_maskView = this->getView(this->_mask.get(), _clipMask->getPixelRod(args.time, args.renderScale));

        if(OFX::getImageEffectHostDescription()->hostName == "uk.co.thefoundry.nuke")
        {
            // bug in nuke, getRegionOfDefinition() on OFX::Image returns bounds
            _maskPixelRod = _clipMask->getPixelRod(args.time, args.renderScale);
        }
        else
        {
            _maskPixelRod = _mask->getRegionOfDefinition();
        }
    }

    _processParams = _plugin.getProcessParams(args.time, args.renderScale);

    OfxRectI srcRoI = translateRegion(_processParams._rect, this->_srcPixelRod);
    View image = subimage_view(this->_srcView, srcRoI.x1, srcRoI.y1, srcRoI.x2 - srcRoI.x1, srcRoI.y2 - srcRoI.y1);

    typedef View MaskView; /// @todo add a MaskView template parameter...

    OfxRectI maskRoI = translateRegion(_processParams._rect, _maskPixelRod);
    View maskView = subimage_view(this->_maskView, maskRoI.x1, maskRoI.y1, maskRoI.x2 - maskRoI.x1, maskRoI.y2 - maskRoI.y1);

    // choose the mask channel to use
    // alpha_t if the image contains an alpha channel like rgba
    // gray_t if grayscale image
    typedef typename MaskView::value_type MaskPixel;
    typedef typename boost::mpl::if_<contains_color<MaskPixel, alpha_t>, alpha_t, gray_t>::type MaskColorChannel;
    typedef channel_view_type<MaskColorChannel, MaskView> KthChannelView;
    typename KthChannelView::type channelMaskView = KthChannelView::make(maskView); // gray or alpha channel

    typedef ComputeOutputParams<View, typename KthChannelView::type, boost::gil::bits64f> ComputeRGBA;
    typename ComputeRGBA::Output outputRGBA = ComputeRGBA::run(image, channelMaskView, _clipMaskConnected, this->_plugin);

    typedef pixel<typename channel_type<View>::type, layout<hsl_t> > HSLPixel;
    typedef color_converted_view_type<View, HSLPixel> HSLConverter;
    typedef ComputeOutputParams<typename HSLConverter::type, typename KthChannelView::type, boost::gil::bits64f> ComputeHSL;
    typename ComputeHSL::Output outputHSL =
        ComputeHSL::run(color_converted_view<HSLPixel>(image), channelMaskView, _clipMaskConnected, this->_plugin);

    setOutputParams(outputRGBA, outputHSL, args.time, this->_plugin);

    switch(_processParams._chooseOutput)
    {
        case eParamChooseOutputSource:
            break;
        case eParamChooseOutputAverage:
            color_convert(outputRGBA._average, _outputPixel);
            break;
        case eParamChooseOutputVariance:
            color_convert(outputRGBA._variance, _outputPixel);
            break;
        case eParamChooseOutputChannelMin:
            color_convert(outputRGBA._channelMin, _outputPixel);
            break;
        case eParamChooseOutputChannelMax:
            color_convert(outputRGBA._channelMax, _outputPixel);
            break;
        case eParamChooseOutputLuminosityMin:
            color_convert(outputRGBA._luminosityMin, _outputPixel);
            break;
        case eParamChooseOutputLuminosityMax:
            color_convert(outputRGBA._luminosityMax, _outputPixel);
            break;
    }
}

/**
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void ImageStatisticsProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    if(_processParams._chooseOutput == eParamChooseOutputSource)
    {
        for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
        {
            typename View::x_iterator src_it = this->_srcView.x_at(procWindowOutput.x1, y);
            typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
            for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++src_it, ++dst_it)
            {
                (*dst_it) = (*src_it);
            }
            if(this->progressForward(procWindowSize.x))
                return;
        }
    }
    else
    {
        for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
        {
            typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
            for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++dst_it)
            {
                (*dst_it) = _outputPixel;
            }
            if(this->progressForward(procWindowSize.x))
                return;
        }
    }
}
}
}
}
