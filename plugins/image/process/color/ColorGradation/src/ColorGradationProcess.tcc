#include "ColorGradationDefinitions.hpp"
#include "ColorGradationProcess.hpp"
#include "ColorGradationPlugin.hpp"

#include <tuttle/plugin/exceptions.hpp>
#include <terry/typedefs.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>

#include <terry/globals.hpp>
#include <terry/copy.hpp>
#include <terry/colorspace/gradation.hpp>

#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorGradation
{

using namespace boost::gil;

template <class View>
ColorGradationProcess<View>::ColorGradationProcess(ColorGradationPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void ColorGradationProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);

    _params = _plugin.getProcessParams(args.renderScale);
}

template <class View>
template <class TIN, class TOUT>
GIL_FORCEINLINE void ColorGradationProcess<View>::processSwitchAlpha(const bool processAlpha, const View& src,
                                                                     const View& dst, TIN gradationIn, TOUT gradationOut)
{
    using namespace boost::gil;
    if(processAlpha)
    {
        terry::algorithm::transform_pixels_progress(
            src, dst, terry::color::transform_pixel_color_gradation_t<TIN, TOUT>(gradationIn, gradationOut), *this);
    }
    else
    {
        /// @todo do not apply process on alpha directly inside transform, with a "channel_for_each_if_channel"
        terry::algorithm::transform_pixels_progress(
            src, dst, terry::color::transform_pixel_color_gradation_t<TIN, TOUT>(gradationIn, gradationOut), *this);

        // temporary solution copy alpha channel
        terry::copy_channel_if_exist<alpha_t>(src, dst);
    }
}

template <class View>
template <class TIN>
GIL_FORCEINLINE void ColorGradationProcess<View>::processSwitchOut(const EParamGradation out, const bool processAlpha,
                                                                   const View& src, const View& dst, TIN gradationIn)
{
    using namespace boost::gil;
    terry::color::gradation::Gamma gamma(_params._GammaValueOut);
    terry::color::gradation::Cineon cineon(_params._BlackPointOut, _params._WhitePointOut, _params._GammaSensitoOut);
    switch(out)
    {
        case eParamGradation_linear:
            processSwitchAlpha<TIN, terry::color::gradation::Linear>(processAlpha, src, dst, gradationIn);
            break;
        case eParamGradation_sRGB:
            processSwitchAlpha<TIN, terry::color::gradation::sRGB>(processAlpha, src, dst, gradationIn);
            break;
        case eParamGradation_Rec709:
            processSwitchAlpha<TIN, terry::color::gradation::Rec709>(processAlpha, src, dst, gradationIn);
            break;
        case eParamGradation_cineon:
            processSwitchAlpha<TIN, terry::color::gradation::Cineon>(processAlpha, src, dst, gradationIn, cineon);
            break;
        case eParamGradation_gamma:
            processSwitchAlpha<TIN, terry::color::gradation::Gamma>(processAlpha, src, dst, gradationIn, gamma);
            break;
        case eParamGradation_panalog:
            processSwitchAlpha<TIN, terry::color::gradation::Panalog>(processAlpha, src, dst, gradationIn);
            break;
        case eParamGradation_REDLog:
            processSwitchAlpha<TIN, terry::color::gradation::REDLog>(processAlpha, src, dst, gradationIn);
            break;
        case eParamGradation_ViperLog:
            processSwitchAlpha<TIN, terry::color::gradation::ViperLog>(processAlpha, src, dst, gradationIn);
            break;
        case eParamGradation_REDSpace:
            processSwitchAlpha<TIN, terry::color::gradation::REDSpace>(processAlpha, src, dst, gradationIn);
            break;
        case eParamGradation_AlexaV3LogC:
            processSwitchAlpha<TIN, terry::color::gradation::AlexaV3LogC>(processAlpha, src, dst, gradationIn);
            break;
    }
}

template <class View>
void ColorGradationProcess<View>::processSwitchInOut(const EParamGradation in, const EParamGradation out,
                                                     const bool processAlpha, const View& src, const View& dst)
{
    using namespace boost::gil;
    terry::color::gradation::Gamma gamma(_params._GammaValueIn);
    terry::color::gradation::Cineon cineon(_params._BlackPointIn, _params._WhitePointIn, _params._GammaSensitoIn);
    switch(in)
    {
        case eParamGradation_linear:
            processSwitchOut<terry::color::gradation::Linear>(out, processAlpha, src, dst);
            break;
        case eParamGradation_sRGB:
            processSwitchOut<terry::color::gradation::sRGB>(out, processAlpha, src, dst);
            break;
        case eParamGradation_Rec709:
            processSwitchOut<terry::color::gradation::Rec709>(out, processAlpha, src, dst);
            break;
        case eParamGradation_cineon:
            processSwitchOut<terry::color::gradation::Cineon>(out, processAlpha, src, dst, cineon);
            break;
        case eParamGradation_gamma:
            processSwitchOut<terry::color::gradation::Gamma>(out, processAlpha, src, dst, gamma);
            break;
        case eParamGradation_panalog:
            processSwitchOut<terry::color::gradation::Panalog>(out, processAlpha, src, dst);
            break;
        case eParamGradation_REDLog:
            processSwitchOut<terry::color::gradation::REDLog>(out, processAlpha, src, dst);
            break;
        case eParamGradation_ViperLog:
            processSwitchOut<terry::color::gradation::ViperLog>(out, processAlpha, src, dst);
            break;
        case eParamGradation_REDSpace:
            processSwitchOut<terry::color::gradation::REDSpace>(out, processAlpha, src, dst);
            break;
        case eParamGradation_AlexaV3LogC:
            processSwitchOut<terry::color::gradation::AlexaV3LogC>(out, processAlpha, src, dst);
            break;
    }
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ColorGradationProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    View src = subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    processSwitchInOut(_params._in, _params._out, _params._processAlpha, src, dst);
}
}
}
}
