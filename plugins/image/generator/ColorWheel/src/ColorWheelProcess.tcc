#include "ColorWheelAlgorithm.hpp"

#include <tuttle/plugin/ofxToGil/point.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <algorithm>

namespace tuttle
{
namespace plugin
{
namespace colorWheel
{

template <class View>
ColorWheelProcess<View>::ColorWheelProcess(ColorWheelPlugin& effect)
    : ImageGilProcessor<View>(effect, eImageOrientationIndependant)
    , _plugin(effect)
{
}

template <class View>
void ColorWheelProcess<View>::setup(const OFX::RenderArguments& args)
{
    using namespace terry;
    ImageGilProcessor<View>::setup(args);
    _params = _plugin.getProcessParams();

    OfxRectD rod = _plugin._clipDst->getCanonicalRod(args.time, args.renderScale);

    int xshift = 0.0;
    int yshift = 0.0;

    if(_params.mode == eColorWheelModeRainbow)
    {
        boost::function_requires<PixelLocatorConcept<RainbowLocator> >();
        gil_function_requires<StepIteratorConcept<typename RainbowLocator::x_iterator> >();

        RainbowPoint dims(rod.x2 - rod.x1, rod.y2 - rod.y1);

        // create a squared color wheel
        RainbowVirtualView rainbow(dims, RainbowLocator(RainbowPoint(0, 0), RainbowPoint(1, 1), RainbowFunctorT(dims)));
        // create a subview depending on the image ratio
        _srcRainbowView = subimage_view<>(rainbow, 0, 0, boost::numeric_cast<int>(dims.x), boost::numeric_cast<int>(dims.y));
    }
    else
    {
        boost::function_requires<PixelLocatorConcept<ColorWheelLocator> >();
        gil_function_requires<StepIteratorConcept<typename ColorWheelLocator::x_iterator> >();

        ColorWheelPoint dims(rod.x2 - rod.x1, rod.y2 - rod.y1);
        float size = std::min(dims.x, dims.y);

        if(size == dims.x)
            yshift = -boost::numeric_cast<int>((dims.y - dims.x) * 0.5);
        else
            xshift = -boost::numeric_cast<int>((dims.x - dims.y) * 0.5);

        ColorWheelPoint tileSize(size, size);

        // create a squared color wheel
        ColorWheelVirtualView colorWheel(
            ColorWheelPoint(size, size),
            ColorWheelLocator(ColorWheelPoint(0, 0), ColorWheelPoint(1, 1),
                              ColorWheelFunctorT(tileSize, _params.mode == eColorWheelModeBlack)));
        // create a subview depending on the image ratio
        _srcColorWheelView =
            subimage_view<>(colorWheel, xshift, yshift, boost::numeric_cast<int>(dims.x), boost::numeric_cast<int>(dims.y));
    }
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template <class View>
void ColorWheelProcess<View>::multiThreadProcessImages(const OfxRectI& procWindowRoW)
{
    using namespace boost::gil;
    OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates(procWindowRoW);
    const OfxPointI procWindowSize = {procWindowRoW.x2 - procWindowRoW.x1, procWindowRoW.y2 - procWindowRoW.y1};
    if(_params.mode == eColorWheelModeRainbow)
    {
        for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
        {
            typename RainbowVirtualView::x_iterator src_it = this->_srcRainbowView.x_at(procWindowOutput.x1, y);
            typename View::x_iterator dst_it = this->_dstView.x_at(procWindowOutput.x1, y);
            for(int x = procWindowOutput.x1; x < procWindowOutput.x2; ++x, ++src_it, ++dst_it)
            {
                (*dst_it) = (*src_it);
            }
            if(this->progressForward(procWindowSize.x))
                return;
        }
    }
    else
    {
        for(int y = procWindowOutput.y1; y < procWindowOutput.y2; ++y)
        {
            typename ColorWheelVirtualView::x_iterator src_it = this->_srcColorWheelView.x_at(procWindowOutput.x1, y);
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
}
