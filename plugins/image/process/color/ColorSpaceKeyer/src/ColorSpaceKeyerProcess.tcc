#include "ColorSpaceKeyerAlgorithm.hpp"

#include <terry/algorithm/transform_pixels_progress.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

template <class View>
ColorSpaceKeyerProcess<View>::ColorSpaceKeyerProcess(ColorSpaceKeyerPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
    // this->setNoMultiThreading();
}

template <class View>
void ColorSpaceKeyerProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ColorSpaceKeyerProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    // this->_renderArgs.time
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);

    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    View src = subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    // Create and initialize functor
    Compute_alpha_pixel funct(false, _plugin._renderAttributes.geodesicFormColor,
                              _plugin._renderAttributes.geodesicFormSpill); // Output is alpha
    // this function is chose because of functor reference and not copy
    terry::algorithm::transform_pixels_progress(src, dst, funct, *this);
}
}
}
}
