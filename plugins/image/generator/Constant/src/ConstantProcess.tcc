#include "ConstantProcess.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>
#include <terry/numeric/assign.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace constant
{

template <class View>
ConstantProcess<View>::ConstantProcess(ConstantPlugin& instance)
    : ImageGilProcessor<View>(instance, eImageOrientationIndependant)
    , _plugin(instance)
{
}

template <class View>
ConstantParams<View> ConstantProcess<View>::getParams()
{
    using namespace boost::gil;
    ConstantParams<View> params;

    OfxRGBAColourD c = _plugin._color->getValue();
    color_convert(rgba32f_pixel_t(c.r, c.g, c.b, c.a), params._color);
    return params;
}

template <class View>
void ConstantProcess<View>::setup(const OFX::RenderArguments& args)
{
    ImageGilProcessor<View>::setup(args);

    // params
    _params = getParams();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void ConstantProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    const OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    View dst = subimage_view(this->_dstView, procWindowOutput.x1, procWindowOutput.y1, procWindowSize.x, procWindowSize.y);

    terry::algorithm::transform_pixels_progress(dst, terry::numeric::pixel_assigns_color_t<Pixel>(_params._color),
                                                this->getOfxProgress());
}
}
}
}
