#include "ViewerAlgorithm.hpp"

#include <terry/algorithm/pixel_by_channel.hpp>

namespace tuttle
{
namespace plugin
{
namespace viewer
{

template <class View>
ViewerProcess<View>::ViewerProcess(ViewerPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationFromBottomToTop)
    , _plugin(effect)
{
    this->setNoMultiThreading();
}

template <class View>
void ViewerProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ViewerProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
}
}
}
}
