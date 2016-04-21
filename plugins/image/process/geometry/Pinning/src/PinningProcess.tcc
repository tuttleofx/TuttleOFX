#include <terry/geometry/pinning.hpp>

#include <tuttle/plugin/ofxToGil/rect.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>
#include <terry/sampler/all.hpp>
#include <terry/sampler/resample_progress.hpp>

namespace tuttle
{
namespace plugin
{
namespace pinning
{

template <class View>
PinningProcess<View>::PinningProcess(PinningPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void PinningProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);

    _params = _plugin.getProcessParams(args.time, args.renderScale);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void PinningProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::sampler;

    const terry::Rect<std::ssize_t> procWindowOutput = ofxToGil(this->translateRoWToOutputClipCoordinates(procWindowRoW));

    switch(_params._samplerProcessParams._filter)
    {
        case eParamFilterNearest:
        {
            resample< ::terry::sampler::nearest_neighbor_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterBilinear:
        {
            resample< ::terry::sampler::bilinear_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterBC:
        {
            bc_sampler BCsampler(_params._samplerProcessParams._paramB, _params._samplerProcessParams._paramC);
            resample(this->_srcView, this->_dstView, procWindowOutput, BCsampler);
            return;
        }
        case eParamFilterBicubic:
        {
            resample<bicubic_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterCatrom:
        {
            resample<catrom_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterMitchell:
        {
            resample<mitchell_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterParzen:
        {
            resample<parzen_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterKeys:
        {
            resample<keys_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterSimon:
        {
            resample<simon_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterRifman:
        {
            resample<rifman_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterLanczos:
        {
            lanczos_sampler lanczosSampler(_params._samplerProcessParams._filterSize,
                                           _params._samplerProcessParams._filterSharpen);
            resample(this->_srcView, this->_dstView, procWindowOutput, lanczosSampler);
            return;
        }
        case eParamFilterLanczos3:
        {
            resample<lanczos3_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterLanczos4:
        {
            resample<lanczos4_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterLanczos6:
        {
            resample<lanczos6_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterLanczos12:
        {
            resample<lanczos12_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
        case eParamFilterGaussian:
        {
            gaussian_sampler gaussianSampler(_params._samplerProcessParams._filterSize,
                                             _params._samplerProcessParams._filterSigma);
            resample<gaussian_sampler>(this->_srcView, this->_dstView, procWindowOutput);
            return;
        }
    }
    BOOST_THROW_EXCEPTION(exception::Bug() << exception::user("Interpolation method not recognize."));
}

template <class View>
template <class Sampler>
void PinningProcess<View>::resample(View& srcView, View& dstView, const terry::Rect<std::ssize_t>& procWindow,
                                    const Sampler& sampler)
{
    using namespace boost::gil;
    switch(_params._method)
    {
        case eParamMethodAffine:
        case eParamMethodPerspective:
            terry::sampler::resample_pixels_progress<Sampler>(srcView, dstView, _params._perspective, procWindow,
                                                              _params._samplerProcessParams._outOfImageProcess,
                                                              this->getOfxProgress(), sampler);
            return;
        case eParamMethodBilinear:
            terry::sampler::resample_pixels_progress<Sampler>(srcView, dstView, _params._bilinear, procWindow,
                                                              _params._samplerProcessParams._outOfImageProcess,
                                                              this->getOfxProgress(), sampler);
            return;
    }
}
}
}
}
