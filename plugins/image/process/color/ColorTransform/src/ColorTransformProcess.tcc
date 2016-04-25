#include "ColorTransformAlgorithm.hpp"

#include <terry/algorithm/transform_pixels_progress.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorTransform
{

template <class View>
ColorTransformProcess<View>::ColorTransformProcess(ColorTransformPlugin& effect)
    : ImageGilFilterProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void ColorTransformProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilFilterProcessor<View>::setup(args);
    _params = _plugin.getProcessParams(args.renderScale);

    _matrix = colorTransformMatrix<BoundedMatrix5x5>(_params);
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ColorTransformProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    using namespace terry;
    using namespace terry::algorithm;
    using namespace terry::color;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    View src = subimage_view(this->_srcView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);
    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    transform_pixels_progress(src, dst, MatrixProdPixel<BoundedMatrix5x5>(_matrix), *this);
}
}
}
}
