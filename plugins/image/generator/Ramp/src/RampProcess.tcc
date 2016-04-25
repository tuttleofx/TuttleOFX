#include "RampAlgorithm.hpp"
#include "RampPlugin.hpp"

namespace tuttle
{
namespace plugin
{
namespace ramp
{

template <class View>
RampProcess<View>::RampProcess(RampPlugin& effect)
    : ImageGilProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
RampProcessParams<View> RampProcess<View>::getProcessParams() const
{
    using namespace terry;
    RampProcessParams<View> params;

    params.direction = (_plugin._direction->getValue() == 0);
    params.color = _plugin._color->getValue();

    OfxRGBAColourD cStart = _plugin._colorStart->getValue();
    color_convert(rgba32f_pixel_t(cStart.r, cStart.g, cStart.b, cStart.a), params.colorStart);
    OfxRGBAColourD cEnd = _plugin._colorEnd->getValue();
    color_convert(rgba32f_pixel_t(cEnd.r, cEnd.g, cEnd.b, cEnd.a), params.colorEnd);

    return params;
}

template <class View>
void RampProcess<View>::setup(const OFX::RenderArguments& args)
{
    using namespace terry;
    ImageGilProcessor<View>::setup(args);
    RampProcessParams<View> params = getProcessParams();

    boost::function_requires<PixelLocatorConcept<Locator> >();
    gil_function_requires<StepIteratorConcept<typename Locator::x_iterator> >();

    OfxRectD rod = _plugin._clipDst->getCanonicalRod(args.time, args.renderScale);
    Point dims(rod.x2 - rod.x1, rod.y2 - rod.y1);

    // create a ramp view
    RampVirtualView checker(dims, Locator(Point(0, 0), Point(1, 1), RampFunctorT(dims, params.colorStart, params.colorEnd,
                                                                                 params.direction, params.color)));
    // create a subview depending on the image ratio
    _srcView = subimage_view<>(checker, 0.0, 0.0, boost::numeric_cast<int>(dims.x), boost::numeric_cast<int>(dims.y));
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void RampProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
    {
        typename RampVirtualView::x_iterator src_it = this->_srcView.x_at(procWindowOutput.x1, y);
        typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
        for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++src_it, ++dst_it)
        {
            (*dst_it) = (*src_it);
        }
        if(this->progressForward(procWindowSize.x))
            return;
    }
}
}
}
}
