#include "HistogramKeyerAlgorithm.hpp"

#include <terry/algorithm/transform_pixels_progress.hpp>

namespace tuttle
{
namespace plugin
{
namespace histogramKeyer
{

template <class View>
HistogramKeyerProcess<View>::HistogramKeyerProcess(HistogramKeyerPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void HistogramKeyerProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.time, args.renderScale);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void HistogramKeyerProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    // this->_renderArgs.time
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);

    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    View src = subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    // Create and initialize functor
    Compute_alpha_pixel funct;
    funct._params = _params;
    funct._isOutputBW = (_params._paramOutputSetting->getValue() == 1); // is output black and white (or alpha channel)
    // this function is chose because of functor reference and not copy
    terry::algorithm::transform_pixels_progress(src, dst, funct, *this);
}
}
}
}
