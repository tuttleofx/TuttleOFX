#include "SobelPlugin.hpp"

#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>
#include <terry/color/gradient.hpp>
#include <terry/color/norm.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>
#include <terry/filter/convolve.hpp>
#include <terry/algorithm/pixel_by_channel.hpp>
#include <terry/typedefs.hpp>

#include <boost/gil/utilities.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/math/special_functions/pow.hpp>

#include <cmath>

namespace tuttle
{
namespace plugin
{
namespace sobel
{

template <class SView, class DView>
SobelProcess<SView, DView>::SobelProcess(SobelPlugin& effect)
    : ImageGilFilterProcessor<SView, DView>(effect, eImageOrientationFromBottomToTop)
    , _plugin(effect)
{
    using namespace terry::numeric;
    pixel_zeros_t<DPixel>()(_pixelZero);
}

template <class SView, class DView>
void SobelProcess<SView, DView>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<SView, DView>::setup(args);

    _params = _plugin.getProcessParams(args.renderScale);
}

template <class SView, class DView>
void SobelProcess<SView, DView>::preProcess()
{
    this->progressBegin(5 /* steps */ * this->_renderWindowSize.x * this->_renderWindowSize.y);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class SView, class DView>
void SobelProcess<SView, DView>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost;
    using namespace terry;
    using namespace terry::numeric;
    using namespace terry::filter;
    using namespace terry::algorithm;

    //	TUTTLE_LOG_INFO( "Sobel X: " << _params._xKernelGaussianDerivative.size() << "x" << _params._xKernelGaussian.size()
    //);
    //	TUTTLE_LOG_INFO( "Sobel Y: " << _params._yKernelGaussianDerivative.size() << "x" << _params._yKernelGaussian.size()
    //);

    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    typedef typename channel_mapping_type<DView>::type DChannel;
    typedef typename terry::floating_channel_type_t<DChannel>::type DChannelFloat;
    typedef pixel<DChannelFloat, gray_layout_t> DPixelGray;

    DView dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    const Point proc_tl(procWindowRoW.x1 - this->_srcPixelRod.x1, procWindowRoW.y1 - this->_srcPixelRod.y1);

    if(_params._xKernelGaussianDerivative.size() == 0 || (!_params._unidimensional && _params._xKernelGaussian.size() == 0))
    {
        fill_pixels(kth_channel_view<0>(dst), _pixelZero);
    }
    else
    {
        if(_params._unidimensional)
        {
            correlate_rows_auto<DPixelGray>(color_converted_view<DPixelGray>(this->_srcView),
                                            _params._xKernelGaussianDerivative, kth_channel_view<0>(dst), proc_tl,
                                            _params._boundary_option);
        }
        else
        {
            switch(_params._pass)
            {
                case eParamPassFull:
                {
                    correlate_rows_cols_auto<DPixelGray, OfxAllocator>(
                        color_converted_view<DPixelGray>(this->_srcView), _params._xKernelGaussianDerivative,
                        _params._xKernelGaussian, kth_channel_view<0>(dst), proc_tl, _params._boundary_option);
                    break;
                }
                case eParamPass1:
                {
                    correlate_rows_auto<DPixelGray>(color_converted_view<DPixelGray>(this->_srcView),
                                                    _params._xKernelGaussianDerivative, kth_channel_view<0>(dst), proc_tl,
                                                    _params._boundary_option);
                    break;
                }
                case eParamPass2:
                {
                    correlate_cols_auto<DPixelGray>(kth_channel_view<0>(this->_srcView), _params._xKernelGaussian,
                                                    kth_channel_view<0>(dst), proc_tl, _params._boundary_option);
                    break;
                }
            }
        }
    }
    if(this->progressForward(dst.size()))
        return;

    if(_params._yKernelGaussianDerivative.size() == 0 || (!_params._unidimensional && _params._yKernelGaussian.size() == 0))
    {
        fill_pixels(kth_channel_view<1>(dst), _pixelZero);
    }
    else
    {
        if(_params._unidimensional)
        {
            correlate_cols_auto<DPixelGray>(color_converted_view<DPixelGray>(this->_srcView),
                                            _params._yKernelGaussianDerivative, kth_channel_view<1>(dst), proc_tl,
                                            _params._boundary_option);
        }
        else
        {
            switch(_params._pass)
            {
                case eParamPassFull:
                {
                    correlate_rows_cols_auto<DPixelGray, OfxAllocator>(
                        color_converted_view<DPixelGray>(this->_srcView), _params._yKernelGaussian,
                        _params._yKernelGaussianDerivative, kth_channel_view<1>(dst), proc_tl, _params._boundary_option);
                    break;
                }
                case eParamPass1:
                {
                    correlate_rows_auto<DPixelGray>(color_converted_view<DPixelGray>(this->_srcView),
                                                    _params._yKernelGaussian, kth_channel_view<1>(dst), proc_tl,
                                                    _params._boundary_option);
                    break;
                }
                case eParamPass2:
                {
                    typedef boost::mpl::bool_<(num_channels<SView>::value >= 2)> Enable;
                    computeYPass2<DPixelGray>(dst, proc_tl, Enable());
                    break;
                }
            }
        }
    }
    if(this->progressForward(dst.size()))
        return;

    if(!_params._computeGradientNorm)
    {
        fill_pixels(kth_channel_view<2>(dst), _pixelZero);
    }
    else if(_params._gradientNormManhattan)
    {
        transform_pixels_progress(kth_channel_view<0>(dst), // srcX
                                  kth_channel_view<1>(dst), // srcY
                                  kth_channel_view<2>(dst), // dst: gradient direction
                                  transform_pixel_by_channel_t<terry::color::channel_normManhattan_t>(),
                                  this->getOfxProgress());
    }
    else
    {
        transform_pixels_progress(kth_channel_view<0>(dst), // srcX
                                  kth_channel_view<1>(dst), // srcY
                                  kth_channel_view<2>(dst), // dst: gradient direction
                                  transform_pixel_by_channel_t<terry::color::channel_norm_t>(), this->getOfxProgress());
    }
    if(this->progressForward(dst.size()))
        return;

    computeGradientDirection(dst, boost::mpl::bool_<(boost::gil::num_channels<DView>::value >= 4)>());
}

template <class SView, class DView>
void SobelProcess<SView, DView>::computeGradientDirection(DView& dst, boost::mpl::true_)
{
    using namespace boost;
    using namespace terry;
    using namespace terry::algorithm;

    if(!_params._computeGradientDirection)
    {
        fill_pixels(kth_channel_view<3>(dst), channel_traits<typename channel_type<DView>::type>::max_value());
        if(this->progressForward(dst.size()))
            return;
    }
    else
    {
        if(_params._gradientDirectionAbs)
        {
            transform_pixels_progress(kth_channel_view<0>(dst), // srcX
                                      kth_channel_view<1>(dst), // srcY
                                      kth_channel_view<3>(dst), // dst: gradient direction
                                      transform_pixel_by_channel_t<terry::color::channel_gradientDirectionAbs_t>(),
                                      this->getOfxProgress());
        }
        else
        {
            transform_pixels_progress(kth_channel_view<0>(dst), // srcX
                                      kth_channel_view<1>(dst), // srcY
                                      kth_channel_view<3>(dst), // dst: gradient direction
                                      transform_pixel_by_channel_t<terry::color::channel_gradientDirection_t>(),
                                      this->getOfxProgress());
        }
    }
}
}
}
}
