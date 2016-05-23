#include "CheckerboardProcess.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace checkerboard
{

template <class View>
CheckerboardProcess<View>::CheckerboardProcess(CheckerboardPlugin& instance)
    : ImageGilProcessor<View>(instance, eImageOrientationFromBottomToTop)
    , _plugin(instance)
{
}

template <class View>
CheckerboardParams<View> CheckerboardProcess<View>::getParams()
{
    using namespace boost::gil;
    CheckerboardParams<View> params;
    params._boxes = ofxToGil(_plugin._boxes->getValue());

    OfxRGBAColourD c1 = _plugin._color1->getValue();
    color_convert(rgba32f_pixel_t(c1.r, c1.g, c1.b, c1.a), params._color1);
    OfxRGBAColourD c2 = _plugin._color2->getValue();
    color_convert(rgba32f_pixel_t(c2.r, c2.g, c2.b, c2.a), params._color2);
    return params;
}

template <class View>
void CheckerboardProcess<View>::setup(const OFX::RenderArguments& args)
{
    using namespace boost::gil;
    ImageGilProcessor<View>::setup(args);

    boost::function_requires<PixelLocatorConcept<Locator> >();
    gil_function_requires<StepIteratorConcept<typename Locator::x_iterator> >();

    // params
    CheckerboardParams<View> params = getParams();

    OfxRectD rod = _plugin._clipDst->getCanonicalRod(args.time, args.renderScale);

    Point dims(rod.x2 - rod.x1, rod.y2 - rod.y1);
    Point tileSize(dims.x / params._boxes.x, dims.y / params._boxes.y);

    // create a squared color wheel
    CheckerboardVirtualView checker(
        Point(dims.x, dims.y),
        Locator(Point(0, 0), Point(1, 1), CheckerboardFunctorT(tileSize, params._color1, params._color2)));
    // create a subview depending on the image ratio
    _srcView = subimage_view<>(checker, 0, 0, boost::numeric_cast<int>(dims.x), boost::numeric_cast<int>(dims.y));
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template <class View>
void CheckerboardProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};

    for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
    {
        typename CheckerboardVirtualView::x_iterator src_it = this->_srcView.x_at(procWindowOutput.x1, y);
        typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
        for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++src_it, ++dst_it)
        {
            *dst_it = *src_it;
        }
        if(this->progressForward(procWindowSize.x))
            return;
    }
}
}
}
}
