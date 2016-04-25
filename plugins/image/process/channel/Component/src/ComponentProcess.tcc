#include "ComponentAlgorithm.hpp"

#include <terry/algorithm/transform_pixels_progress.hpp>

namespace tuttle
{
namespace plugin
{
namespace component
{

template <class SView, class DView>
ComponentProcess<SView, DView>::ComponentProcess(ComponentPlugin& effect)
    : ImageGilFilterProcessor<SView, DView>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class SView, class DView>
void ComponentProcess<SView, DView>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<SView, DView>::setup(args);
    // get process params
    _params.grayMethod = static_cast<terry::color::components::EConvertToGray>(_plugin._paramToGrayMethod->getValue());
    _params.premultiplied = _plugin._paramPremultiplied->getValue();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class SView, class DView>
void ComponentProcess<SView, DView>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace terry;
    using namespace terry::color::components;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);

    const OfxRectI procWindowSrc = translateRegion(procWindowRoW, this->_srcPixelRod);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    SView src = subimage_view(this->_srcView, procWindowSrc.x1, procWindowSrc.y1, procWindowSize.x, procWindowSize.y);
    DView dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    convertComponentsView(src, dst, _params);
}
}
}
}
