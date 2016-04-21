#include <tuttle/plugin/ofxToGil/rect.hpp>
#include <terry/sampler/resample_progress.hpp>
#include <terry/geometry/affine.hpp>

namespace tuttle
{
namespace plugin
{
namespace resize
{

template <class View>
ResizeProcess<View>::ResizeProcess(ResizePlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationFromBottomToTop)
    , _plugin(effect)
{
    this->setNoMultiThreading();
}

template <class View>
void ResizeProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ResizeProcess<View>::multiThreadProcessImages(const OfxRectI& procWindow)
{
    using namespace terry;
    using namespace terry::sampler;

    const terry::Rect<std::ssize_t> procWin = ofxToGil(procWindow);

    const double src_width = std::max<double>(this->_srcView.width() - 1, 1);
    const double src_height = std::max<double>(this->_srcView.height() - 1, 1);
    const double dst_width = std::max<double>(this->_dstView.width() - 1, 1);
    const double dst_height = std::max<double>(this->_dstView.height() - 1, 1);

    // TUTTLE_LOG_INFO("\E[1;31mResize Position = " << -( _params._centerPoint.x - dst_width * 0.5) << "x" << -(
    // _params._centerPoint.y - dst_height * 0.5) << "\E[0;0m");

    const EParamFilterOutOfImage outOfImageProcess =
        static_cast<EParamFilterOutOfImage>(_params._samplerProcessParams._outOfImageProcess);

    matrix3x2<double> mat;

#if(TUTTLE_EXPERIMENTAL)
    if(_params._changeCenter)
    {
        mat = matrix3x2<double>::get_translate(-(_params._centerPoint.x - dst_width * 0.5),
                                               -(_params._centerPoint.y - dst_height * 0.5)) *
              matrix3x2<double>::get_translate(-dst_width * 0.5, -dst_height * 0.5) *
              matrix3x2<double>::get_scale((src_width + 1) / (dst_width + 1), (src_height + 1) / (dst_height + 1)) *
              matrix3x2<double>::get_translate(src_width * 0.5, src_height * 0.5);
    }
    else
#endif
    {
        mat = matrix3x2<double>::get_translate(-dst_width * 0.5, -dst_height * 0.5) *
              matrix3x2<double>::get_scale((src_width + 1) / (dst_width + 1), (src_height + 1) / (dst_height + 1)) *
              matrix3x2<double>::get_translate(src_width * 0.5, src_height * 0.5);
    }

    switch(_params._samplerProcessParams._filter)
    {
        case eParamFilterNearest:
            resample_pixels_progress< ::terry::sampler::nearest_neighbor_sampler>(
                this->_srcView, this->_dstView, mat, procWin, outOfImageProcess, this->getOfxProgress());
            break;
        case eParamFilterBilinear:
            resample_pixels_progress< ::terry::sampler::bilinear_sampler>(this->_srcView, this->_dstView, mat, procWin,
                                                                          outOfImageProcess, this->getOfxProgress());
            break;
        case eParamFilterBC:
        {
            bc_sampler BCsampler(_params._samplerProcessParams._paramB, _params._samplerProcessParams._paramC);
            resample_pixels_progress(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess, this->getOfxProgress(),
                                     BCsampler);
            break;
        }
        case eParamFilterBicubic:
            resample_pixels_progress<bicubic_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                      this->getOfxProgress());
            break;
        case eParamFilterCatrom:
            resample_pixels_progress<catrom_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                     this->getOfxProgress());
            break;
        case eParamFilterKeys:
            resample_pixels_progress<keys_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                   this->getOfxProgress());
            break;
        case eParamFilterSimon:
            resample_pixels_progress<simon_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                    this->getOfxProgress());
            break;
        case eParamFilterRifman:
            resample_pixels_progress<rifman_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                     this->getOfxProgress());
            break;
        case eParamFilterMitchell:
            resample_pixels_progress<mitchell_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                       this->getOfxProgress());
            break;
        case eParamFilterParzen:
            resample_pixels_progress<parzen_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                     this->getOfxProgress());
            break;
        case eParamFilterGaussian:
        {
            gaussian_sampler gaussianSampler(_params._samplerProcessParams._filterSize,
                                             _params._samplerProcessParams._filterSigma);
            resample_pixels_progress(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess, this->getOfxProgress(),
                                     gaussianSampler);
            break;
        }
        case eParamFilterLanczos:
        {
            lanczos_sampler lanczosSampler(_params._samplerProcessParams._filterSize,
                                           _params._samplerProcessParams._filterSharpen);
            resample_pixels_progress(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess, this->getOfxProgress(),
                                     lanczosSampler);
            break;
        }
        case eParamFilterLanczos3:
            resample_pixels_progress<lanczos3_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                       this->getOfxProgress());
            break;
        case eParamFilterLanczos4:
            resample_pixels_progress<lanczos4_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                       this->getOfxProgress());
            break;
        case eParamFilterLanczos6:
            resample_pixels_progress<lanczos6_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                       this->getOfxProgress());
            break;
        case eParamFilterLanczos12:
            resample_pixels_progress<lanczos12_sampler>(this->_srcView, this->_dstView, mat, procWin, outOfImageProcess,
                                                        this->getOfxProgress());
            break;
    }
}
}
}
}
